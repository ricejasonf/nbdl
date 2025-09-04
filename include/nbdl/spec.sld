(import (heavy base))

(define-library (nbdl spec)
  (import (prefix (heavy base) base.)
          (only (heavy base) define
                             define-syntax
                             if
                             lambda
                             set!
                             syntax-rules
                             quote
                             quasiquote)
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
    (define (maybe-literal-op* arg)
      (define AttrVal
        (if (base.number? arg)
          (attr (base.number->string arg) i32)
          (if (base.symbol? arg)
            (string-attr arg)
            (if (base.string? arg)
              (string-attr arg)
              #f))))
      (if AttrVal
        (build-literal AttrVal)
        arg))

    (define (build-literal arg)
      (result
        (create-op "nbdl.literal"
                   (attributes
                     `("value", arg))
                   (result-types !nbdl.opaque)))

    (define (build-constexpr arg)
      (result
        (create-op "nbdl.constexpr"
                   (attributes
                     `("expr", (string-attr arg))
                   (result-types !nbdl.opaque)))))


    (define (build-member-name name)
      (result
        (create-op "nbdl.member_name"
                   (loc name)
                   (attributes
                     `("name", (string-attr name)))
                   (result-types !nbdl.symbol))))

    (define-syntax context
      (syntax-rules ()
        ((context name (formals ...) body ...)
          (%build-context
            name
            (base.length '(formals ...))
            (base.lambda (formals ...)
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
                      (operands (build-member-name key) NewStore parent)
                      (result-types !nbdl.store)))))
              body ...
              (translate-cpp
                (parent-op (BuildCont))
                lexer-writer)
              )))))

    ;; Take a list of paths and take the first node
    ;; off of the first path and reconstruct the
    ;; list of paths with that node removed.
    ;; FIXME Replace syntax with let-values.
    (define-syntax %take-path-node
      (syntax-rules ()
        ((%take-path-node
            InputPaths (CurPathNode NewPaths) body ...)
          (((lambda ()
            (define $CurPath (if (pair? InputPaths)
                                 (car InputPaths)
                                 '()))
            (define $CurPathNode (if (pair? $CurPath)
                                     (car $CurPath)
                                     '()))
            (define $NewPaths
              (if (pair? InputPaths)
                (if pair? $CurPath
                  (cons (cdr $CurPath) (cdr InputPaths))
                  (cdr InputPaths))
                '()))
            (lambda (CurPathNode NewPaths) body ...)
              $CurPathNode $NewPaths))))))

    ;; Define a function to receive a matched set of parameters.
    (define-syntax match-params-fn
      (syntax-rules ()
        ((match-params-fn name (stores ... fn) body ...)
          (%build-match-params
            name
            (base.length '(stores ...))
            (base.lambda (stores ... FnArg)
              (define (Fn . paths)
                (define ParamVals '())
                (lambda (???
                (%take-path-node paths (CurPathNode NewPaths)
                  ; The MatchedVal is the Store input for the next operation
                  ; or it is added to the ParamVals.
                  (define MatchedVal
                    (%build-path-node MatchedVal CurPathNode ParamVals))
                  (if (null? MatchedVal)
                      (create-op "nbdl.visit" (operands FnArg ParamVals))
                      'else???)

                )
              ; Note we are shadowing the stores names here.
              ((base.lambda (stores ... fn) body ...)
                stores ... Fn)
              )))))


    ; Note that some of these internal procedures alter
    ; the builder insertion point without reverting.
    (define (%match-path-spec PathSpec)
      (define RootStore #f)
      (define PathNodes #f)
      (if (pair? PathSpec)
        (if (eqv? '%nbdl-path (car PathSpec))
          (set! PathNodes (cdr PathSpec))))
      (if (pair? PathNodes)
        (begin (set! RootStore (car PathNodes))
               (set! PathNodes (cdr PathNodes)))
        (error "expecting nbdl pathspec" PathSpec))
      (if (mlir-operation? RootStore)
        (%match-path-spec-rec RootStore PathNodes)
        (error "expecting a root store object in pathspec" PathSpec)))

    (define (%match-path-spec-rec Store PathNodes)
      (define Rec (lambda ()
        (define PathNode (car PathNodes))
        (define Loc (source-loc PathNodes))
        (define NewStore #f)
        (define Key #f)
        (if (pair? PathNode)
          ((lambda ()
            (define Id (car PathNode))
            (if (eqv? Id '%nbdl-path)
              (set! Key (%match-path-spec PathNode))
            (if (eqv? Id '%constexpr)
              (set! Key (build-constexpr Loc (cdr PathNode)))
            (if (eqv? Id '%overload)
              (set! NewStore (%build-overload Loc (cdr PathNode)))
            (if (eqv? Id '%apply)
              (set! NewStore (%build-apply Loc (cdr PathNode)))
          ))))))
          (set! Key
            (if (number? PathNode)
              (build-literal PathNode)
            (if (string? PathNode)
              (literal PathNode)
            (if (symbol? PathNode)
              (build-member-name PathNode)
            (if (pair? PathNode)
              (define Id (car PathNode))
            (error "unsupported path node kind" PathNode)))))))
        (if Key
          (set! NewStore (%build-match Loc Store Key)))
        (if NewStore
          NewStore
          (error "failed to create new store from path node" PathNode))
        ))
      (if (null? PathNodes)
        Store
        (Rec)))

    ;; Given a C++ function, return a lambda that takes a list of paths
    ;; that resolve to a list of parameters to apply to the function.
    ;; - This will have runtime effects that are not necessarily stored.
    ;; - (e.g. string concatentation for creating an attribute.)
    (base.define (apply cpp-func)
      (lambda paths
        (base.apply nbdl-impl-apply cpp-func paths)))
    ;; TODO Figure out what this returns exactly.
    ;; Visit each element of a range.
    (base.define (for-each path proc)
      (if (path? path)
        (base.error "TODO Implement for-each")
        (base.error "expecting a path")))

    ;; Create a new path appending keys to the input path.
    ; TODO check mlir-operation? if not a path
    (base.define (get path . keys)
      (if (path? path)
        (base.append path keys)
        (base.append (base.cons '%nbdl-path path) keys)
        ))

    ;; Resolve a value at a path to use as a key.
    (base.define (key-at path)
      (if (path? path)
        (base.error "TODO Implement key")
        (base.error "expecting a path")))

    ;; Match a resolved object by its type.
    ;; - It is an error if a type appears more that once as an alternative.
    ;;   (Think type switch)
    ;; - Each clause should be ((type <cpp-typename>) proc)
    ;;   where proc is a unary lambda receiving the matched path.
    (base.define (match path . clauses)
      (if (path? path)
        (base.error "TODO implement match")
        (base.error "expecting a path")))

    ;; Match the first satisified condition.
    ;; - Each clause should be (unary-predicate proc)
    ;;   where proc is a unary lambda receiving the matched path.
    (base.define (match-cond path . clauses)
      (if (path? path)
        (base.error "TODO implement match-cond")
        (base.error "expecting a path")))

    ;; Check if a scheme value is a path.
    (base.define (path? path)
      (if (base.null? path)
        #t
        (if (base.pair? path)
          (base.eqv? (base.car path) '%nbdl-path)
          #f)))

  ) ; end of... begin
  (export
    apply
    context
    get
    key-at
    match
    match-cond
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
    syntax-rules
    )
)  ; end of (nbdl spec)
