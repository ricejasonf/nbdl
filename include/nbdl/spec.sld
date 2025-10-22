(import (heavy base))

(define-library (nbdl spec)
  (import (only (heavy builtins)
                syntax-error)) ; FIXME (heavy base) should export these
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
    (define !nbdl.unit (type "!nbdl.unit"))
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

    (define (build-unit)
      (result
        (create-op "nbdl.unit"
          (loc: 0)
          (operands:)
          (attributes:)
          (result-types: !nbdl.unit))))

    (define (build-literal Loc Arg)
      (result
        (create-op "nbdl.literal"
          (loc: Loc)
          (operands:)
          (attributes: ("value" Arg))
          (result-types: !nbdl.opaque))))

    (define (build-constexpr Loc ExprStr)
      (result
        (create-op "nbdl.constexpr"
          (loc: Loc)
          (operands:)
          (attributes: ("expr" (string-attr ExprStr)))
          (result-types: !nbdl.opaque))))


    (define (build-member-name Loc Name)
      (result
        (create-op "nbdl.member_name"
          (loc: Loc)
          (operands:)
          (attributes: ("name" (string-attr Name)))
          (result-types: !nbdl.symbol))))

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
                      (loc: 0)
                      (operands:)
                      (attributes:)
                      (result-types: !nbdl.empty))))
              (define (BuildCont)
                (create-op "nbdl.cont"
                  (loc: 0)
                  (operands: parent)
                  (attributes:)
                  (result-types:)
                  ))
              (define (member key typename . init-args)
                (define NewStore
                  (result
                    (create-op "nbdl.store"
                      (loc: typename)
                      (operands: (map maybe-literal-op* init-args))
                      (attributes: ("name" (flat-symbolref-attr typename)))
                      (result-types: !nbdl.store)
                      )))
                (set! parent
                  (result
                    (create-op
                      "nbdl.store_compose"
                      (loc: key)
                      (operands: (build-member-name key key) NewStore parent)
                      (attributes:)
                      (result-types: !nbdl.store)))))
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
                (if (verify FuncOp)
                  (translate-cpp FuncOp lexer-writer)
                  (begin
                    (translate-cpp FuncOp)
                    (error "verification failed: {0} {1}" name FuncOp)))
                FuncOp))
              )))))

    ; Note that some of these internal procedures alter
    ; the builder insertion point without reverting.
    (define (%match-path-spec PathSpec)
      (dump PathSpec)
      (cond
        ((or (value? !nbdl.store PathSpec)
             (value? !nbdl.unit PathSpec))
           PathSpec)
        ((and (pair? PathSpec)
              (eqv? '%nbdl-path (car PathSpec)))
         (let ((RootStore (cadr PathSpec))
               (PathNodes (cddr PathSpec)))
          (if (value? RootStore)
            (%match-path-spec-rec RootStore PathNodes)
            (error "expecting a root store object in pathspec: {}" PathSpec))))
        (else (error "expecting nbdl pathspec: {}" PathSpec))))

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
                (base.apply build-node-get Store Args)
              (if (eqv? Kind '%overload)
                (base.apply build-node-overload Store Args)
              (if (eqv? Kind '%match)
                (base.apply build-node-match Store Args)
              (if (eqv? Kind '%apply)
                (base.apply build-node-apply Store Args)
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
      (error "unsupported path node kind: {}" Key)
        ))))))

    (define (build-node-get Store Loc Key)
      (define KeyVal (build-node-key Loc Key))
      (define Op
        (create-op "nbdl.get"
          (loc: Loc)
          (operands: Store KeyVal)
          (attributes:)
          (result-types: !nbdl.store)))
      (result Op))

    (define (build-node-match Store Loc Key TypeStr)
      ; TODO If TypeStr is empty mark the yielded store as "Noncanonical".
      ;      Also add that "Noncanonical" attribute to dependent store objects
      ;      everywhere we build them.
      (define KeyVal (build-node-key Loc Key))
      (create-op "nbdl.match"
        (loc: Loc)
        (operands: Store KeyVal)
        (attributes:)
        (result-types:)
        #;(region "overloads" (store : !nbdl.store)
          (build-node-overload store Loc TypeStr)
          (dump "stuff")
          )))

    (define (build-node-overload Store Loc TypeStr)
      (create-op "nbdl.overload"
        (loc: Loc)
        (operands:)
        (attributes: ("type" (string-attr TypeStr)))
        (result-types:)
        #;(region "body" (store : !nbdl.store)
          (dump "overload stuff")
          )))

    (define (build-node-apply Loc Store Key)
      (error "TODO implement build-node-apply"))

    (define (build-visit-params Loc FnVal ParamVals)
      (define OrderedParamVals
        (reverse ParamVals))
      (create-op "nbdl.visit"
        (loc: Loc)
        (operands: FnVal OrderedParamVals)
        (attributes:)
        (result-types:)))

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

    ;; Construct an "overload" as a key to a path.
    ;; The visitor will not be called if the type does not match.
    (define (overload typename)
      (define Loc (source-loc typename))
      (list '%overload Loc typename))


    ;; Given a C++ function, return a lambda that takes a list of paths
    ;; that resolve to a list of parameters to apply to the function.
    ;; - This will have runtime effects that are not necessarily stored.
    ;; - (e.g. string concatentation for creating an attribute.)
    (define (applyz cpp-func)
      (lambda paths
        (base.apply nbdl-impl-apply cpp-func paths)))
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

    (define-syntax match-aux
      (syntax-rules (=>)
        ((match-aux S K
          (TypeN => FnN) ...)
         (let ((ResolvedStore (%match-path-spec S))
               (ResolvedKey   (%match-path-spec K)))
          (create-op "nbdl.match"
            (loc: 0)
            (operands: ResolvedStore ResolvedKey)
            (attributes:)
            (result-types:)
            (region: "overloads" ()
              (create-op "nbdl.overload"
                (loc: (source-loc TypeN))
                (operands:)
                (attributes: ("type" (string-attr TypeN)))
                (result-types:)
                (region: "body" ((OverloadArg : !nbdl.store))
                  (FnN OverloadArg))) ...))))))
    

    ;; Match a resolved object by its type.
    ;; - It is an error if a type appears more that once as an alternative.
    ;;   (Think type switch)
    ;; - Each clause should be
    ;;    (<cpp-typename> => proc) or
    ;;    (else => proc)
    ;;   where proc is a unary lambda receiving the matched store.
    ;; - All Types should not have qualifiers.
    (define-syntax match
      (syntax-rules (else => store: key:)
        ((match Path
          (else => DefaultFn))
         (match Path
           ; FIXME This should be an empty string
           ;       but NbdlWriter has to support that.
           ("auto&&" => DefaultFn)))
        ((match Path
          (Type1 => Fn1)
          (TypeN => FnN) ...
          (else => DefaultFn))
         (match Path
           (Type1 => Fn1)
           (TypeN => FnN) ...
           ("" => DefaultFn)))
        ; FIXME Consider not providing this overload
        ;       and using (get ...) for keys.
        ((match ((store: S)
                 (key: K))
           (Type1 => Fn1)
           (TypeN => FnN) ...)
         (match-aux S K
           (Type1 => Fn1)
           (TypeN => FnN) ...))
        ((match StoreOnly
           (Type1 => Fn1)
           (TypeN => FnN) ...)
         (let ((K (build-unit)))
           (match-aux StoreOnly K
             (Type1 => Fn1)
             (TypeN => FnN) ...)))
        ))

    (define (noop x)
      ; Do nothing.
      (when #f #f))

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

    (define (dump-cpp name)
      (define Op
        (module-lookup current-nbdl-module name))
      (translate-cpp Op))

    (define (dump-op name)
      (define Op
        (module-lookup current-nbdl-module name))
      (dump Op))


  ) ; end of... begin
  (export
    ;nbdl.apply
    context
    get
    key-at
    match
    match-cond
    match-params-fn
    dump-cpp
    dump-op
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
    source-loc
    dump
    )
)  ; end of (nbdl spec)
