(import (heavy base))

(define-library (nbdl spec)
  (import (heavy base r7rs-syntax)) ; FIXME (heavy base) should export these
  (import (rename (heavy base)
                  (apply base.apply))
          (heavy mlir)
          (heavy clang)
          (nbdl comp))
  (begin
    (load-dialect "func")
    (load-dialect "heavy")
    (load-dialect "nbdl")
    (define !nbdl.opaque (type "!nbdl.opaque"))
    (define !nbdl.store (type "!nbdl.store"))
    (define !nbdl.tag (type "!nbdl.tag"))
    (define !nbdl.symbol (type "!nbdl.symbol"))
    (define !nbdl.empty (type "!nbdl.empty"))
    (define i32 (type "i32"))

    ; Maybe lift a literal to a LiteralOp.
    (define (maybe-literal-op* Arg)
      (define AttrVal
        (if (number? Arg)
          (attr (number->string Arg) i32)
          (if (symbol? Arg)
            (string-attr Arg)
            (if (string? Arg)
              (string-attr Arg)
              #f))))
      (if AttrVal
        (build-literal Arg AttrVal)
        Arg))

    (define (build-literal Loc Arg)
      (result
        (create-op "nbdl.literal"
                   (loc Loc)
                   (attributes
                     `("value", Arg))
                   (result-types !nbdl.opaque))))

    (define (build-constexpr Loc ExprStr)
      (result
        (create-op "nbdl.constexpr"
                   (loc Loc)
                   (attributes
                     `("expr", (string-attr ExprStr))
                   (result-types !nbdl.opaque)))))


    (define (build-member-name Loc Name)
      (result
        (create-op "nbdl.member_name"
                   (loc Loc)
                   (attributes
                     `("name", (string-attr Name)))
                   (result-types !nbdl.symbol))))

    (define-syntax context
      (syntax-rules ()
        ((context name (member) (formals ...) body ...)
          (%build-context
            name
            (length '(formals ...))
            (lambda (formals ...)
              (define parent
                 (result
                   (create-op "nbdl.empty"
                              (result-types !nbdl.empty))))
              (define (BuildCont)
                (create-op
                  "nbdl.cont"
                  (operands parent)))
              (define (member key typename init-arg)
                (define NewStore
                  (result
                    (create-op
                      "nbdl.store"
                      (loc typename)
                      (attributes `("name", (flat-symbolref-attr typename)))
                      (operands (maybe-literal-op* init-arg))
                      (result-types !nbdl.store)
                      )))
                (set! parent
                  (result
                    (create-op
                      "nbdl.store_compose"
                      (loc key)
                      (operands (build-member-name key key) NewStore parent)
                      (result-types !nbdl.store)))))
              body ...
              (translate-cpp
                (parent-op (BuildCont))
                lexer-writer)
              )))))

    ;; Define a function to receive a matched set of parameters.
    ;; Each path node should be of the format:
    ;;  (%Kind Loc Args...)
    ;; or specifically for nested pathspecs:
    ;;  (%nbdl-path PathNodes...)
    (define-syntax match-params-fn
      (syntax-rules ()
        ((match-params-fn name (stores ... fn) body ...)
          (%build-match-params
            name
            (length '(stores ...))
            (lambda (stores ... %FnVal)
              (define (Fn . %Paths)
                (define ParamVals '()) ; Reverse ordered
                (define Loc (source-loc name))
                (define (FnRec Paths)
                  (if (pair? Paths)
                    (begin
                      (set! ParamVals
                        (cons (%match-path-spec (car Paths)) ParamVals))
                      (FnRec (cdr Paths)))
                  (if (null? Paths)
                    (build-visit-params Loc %FnVal ParamVals)
                    (error "expecting proper list" Paths))))
                (FnRec %Paths))
              ((lambda (fn) body ...) Fn)
              ((lambda ()
                (define FuncOp
                  (module-lookup current-nbdl-module name))
                (translate-cpp FuncOp lexer-writer)
                FuncOp))
              )))))

    ; Note that some of these internal procedures alter
    ; the builder insertion point without reverting.
    (define (%match-path-spec PathSpec)
      (define RootStore #f)
      (define PathNodes #f)
      (set! PathNodes (cdr PathSpec))
      (if (pair? PathSpec)
        (if (eqv? '%nbdl-path (car PathSpec))
          (set! PathNodes (cdr PathSpec))))
      (if (pair? PathNodes)
        (begin (set! RootStore (car PathNodes))
               (set! PathNodes (cdr PathNodes)))
        (error "expecting nbdl pathspec" PathSpec))
      (if (value? !nbdl.store RootStore)
        (%match-path-spec-rec RootStore PathNodes)
        (error "expecting a root store object in pathspec: {}" PathSpec)))

    (define (%match-path-spec-rec Store PathNodes)
      (define (Rec)
        (define PathNode (car PathNodes))
        (define RestPathNodes (cdr PathNodes))
        (define NewStore
          (if (pair? PathNode)
            ((lambda ()
              (define Kind (car PathNode))
              (define Args (cdr PathNode))
              ; These permit custom source locations.
              (if (eqv? Kind '%get)
                (apply build-node-get Store Args)
              (if (eqv? Kind '%overload)
                (apply build-node-overload Store Args)
              (if (eqv? Kind '%match)
                (apply build-node-match Store Args)
              (if (eqv? Kind '%apply)
                (apply build-node-apply Store Args)
              ))))))
            ((lambda () ; Raw %get key specs.
              (define Loc (source-loc PathNode PathNodes))
              (build-node-get Store Loc PathNode)))))
        (%match-path-spec-rec NewStore RestPathNodes))
      (if (null? PathNodes)
        Store
        (Rec)))

    ; Key is the "key spec". Return the KeyVal (ie mlir.value).
    (define (build-node-key Loc Key)
      (if (number? Key)
        (build-literal Loc Key)
      (if (string? Key)
        (build-literal Loc Key)
      (if (symbol? Key)
        (build-member-name Loc Key)
      (if (value? !nbdl.store Key)
        (error "unexpected root store in path" Key)
      (if (pair? Key)
        ((lambda ()
          (define Kind (car Key))
          (define Args (cdr Key))
          (if (eqv? Kind '%constexpr)
            (apply build-constexpr Args)
          (if (eqv? Kind '%nbdl-path)
            (%match-path-spec Store Key)
          (error "unrecognized path node")
            ))))
      (error "unsupported path node kind" Key)
        ))))))

    (define (build-node-get Store Loc Key)
      (define KeyVal (build-node-key Loc Key))
      (define Op
        (create-op "nbdl.get"
                   (loc Loc)
                   (result-types !nbdl.store)
                   (operands Store KeyVal)))
      (result Op))

    (define (build-node-match Store Loc Key TypeStr)
      ; TODO If TypeStr is empty mark the yielded store as "Noncanonical".
      ;      Also add that "Noncanonical" attribute to dependent store objects
      ;      everywhere we build them.
      (define KeyVal (build-node-key Loc Key))
      (define MatchOp
        (create-op "nbdl.match"
          (regions 1)
          (operands Store KeyVal)))
      (define Block (entry-block MatchOp))
      (at-block-begin Block)
      ; Return overloaded matched value.
      ((lambda ()
         (define MatchVal (add-argument Block))
         (build-node-overload MatchVal Loc TypeStr))))

    (define (build-node-overload Store Loc TypeStr)
      (define OverloadOp
        (create-op "nbdl.overload"
          (regions 1)
          (operands (string-attr TypeStr))))
      (define Block (entry-block OverloadOp))
      (at-block-begin Block)
      ; Return a newly added value in the body.
      (add-argument Block))

    (define (build-node-apply Loc Store Key)
      (error "TODO implement build-node-apply"))

    (define (build-visit-params Loc FnVal ParamVals)
      ; ParamsVals is a reverse ordered list.
      ; FIXME Use reverse when it becomes availabe.
      ; FIXME Fix crash with named-let.
      #;(define OrderedParamVals
        (let my-reverse ((List ParamVals)
                          (NewList '()))
          (if (pair? List)
            (my-reverse (cdr List)
                        (cons (car List) NewList))
            NewList)))
      (define OrderedParamVals
        ((lambda ()
          (define NewList '())
          (define (Loop List)
            (if (pair? List)
              (begin
                (set! NewList
                  (cons (car List) NewList))
                (Loop (cdr List)))
              NewList))
          (Loop ParamVals))))

      (create-op "nbdl.visit"
                 (loc Loc)
                 (operands FnVal OrderedParamVals)))

    ; FIXME Accept mlir.value with type !nbdl.store.
    (define (path? obj)
      (if (pair? obj)
        (if (eqv? (car obj) '%nbdl-path)
          #t #f) #f))

    ;; Create a new path appending keys to the input path.
    ;; TODO check that type of mlir value of path is !nbdl.store
    (define (get path . keys)
      (if (path? path)
        (append path keys)
          (append (list '%nbdl-path path) keys)
          ))

    ;; Given a C++ function, return a lambda that takes a list of paths
    ;; that resolve to a list of parameters to apply to the function.
    ;; - This will have runtime effects that are not necessarily stored.
    ;; - (e.g. string concatentation for creating an attribute.)
    (define (applyz cpp-func)
      (lambda paths
        (apply nbdl-impl-apply cpp-func paths)))
    ;; TODO Figure out what this returns exactly.
    ;; Visit each element of a range.
    (define (for-each path proc)
      (if (path? path)
        (error "TODO Implement for-each")
        (error "expecting a path")))

    ;; Resolve a value at a path to use as a key.
    (define (key-at path)
      (if (path? path)
        (error "TODO Implement key")
        (error "expecting a path")))

    ;; Match a resolved object by its type.
    ;; - It is an error if a type appears more that once as an alternative.
    ;;   (Think type switch)
    ;; - Each clause should be ((type <cpp-typename>) proc)
    ;;   where proc is a unary lambda receiving the matched path.
    (define (match path . clauses)
      (if (path? path)
        (error "TODO implement match")
        (error "expecting a path")))

    ;; Match the first satisified condition.
    ;; - Each clause should be (unary-predicate proc)
    ;;   where proc is a unary lambda receiving the matched path.
    (define (match-cond path . clauses)
      (if (path? path)
        (error "TODO implement match-cond")
        (error "expecting a path")))

    ;; Check if a scheme value is a path.
    (define (path? path)
      (if (null? path)
        #t
        (if (pair? path)
          (eqv? (car path) '%nbdl-path)
          #f)))

  ) ; end of... begin
  (export
    nbdl.apply
    context
    get
    key-at
    match
    match-cond
    match-params-fn
    member
    ; reexport some base stuff
    define
    define-syntax
    syntax-rules
    if
    lambda
    set!
    quote
    quasiquote
    unquote unquote-splicing
    dump
    )
)  ; end of (nbdl spec)
