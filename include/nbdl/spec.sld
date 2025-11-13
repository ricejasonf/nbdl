(import (heavy base))

(define-library (nbdl spec)
  (import (heavy base)
          (heavy mlir)
          (heavy clang)
          (nbdl comp))
  (begin
    (load-dialect "func")
    (load-dialect "heavy")
    (load-dialect "nbdl")
    (define !nbdl.store (type "!nbdl.store"))
    ; FIXME change to "!nbdl.variant" once its in the compiler.
    (define !nbdl.variant (type "!nbdl.store")) ;"!nbdl.variant"))
    (define !nbdl.tag (type "!nbdl.tag"))
    (define !nbdl.member_name (type "!nbdl.member_name"))
    (define !nbdl.empty (type "!nbdl.empty"))
    (define !nbdl.unit (type "!nbdl.unit"))
    (define i32 (type "i32"))

    ; Maybe lift to a LiteralOp or ConstexprOp.
    (define (maybe-build-expr Loc Arg)
      (cond
        ((member-name-expr? Arg)
          (error "unexpected member name literal"))
        ((symbol? Arg)
          (build-constexpr Loc Arg))
        ((number? Arg)
          (build-literal Loc (attr (number->string Arg) i32)))
        ((string? Arg)
          (build-literal Loc (string-attr Arg)))
        (else Arg)))

    ;; Maybe lift to a LiteralOp, ConstexprOp, or MemberNameOp.
    (define (maybe-build-expr+ Loc Arg)
      (if (member-name-expr? Arg)
        (build-member-name Loc Arg)
        (maybe-build-expr Loc Arg)))

    (define (%match-expr-impl Expr Fn)
      (cond
        ((value? Expr)
         (Fn Expr))
        ((procedure? Expr)
         (Expr Fn))
        ((expr? Expr) ;; FIXME this go here?
         (let ((Loc (cadr Expr))
               (Thunk (car (cddr Expr))))
           (Thunk Loc Fn)))
        ((path? Expr)
         (%match-path-spec Expr Fn))
        (else (error "unable to resolve value: {}" Expr)))
      ; Return something... unspecified.
      (if #f #f))

    (define (%match-expr Loc ExprArg Fn)
      (define Expr (maybe-build-expr Loc ExprArg))
      (%match-expr-impl Expr Fn))

    (define (%match-expr+ Loc ExprArg Fn)
      (define Expr (maybe-build-expr+ Loc ExprArg))
      (%match-expr-impl Expr Fn))

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
          (result-types: !nbdl.store))))

    (define (build-constexpr Loc ExprStr)
      (when (member-name-expr? ExprStr)
        (error "unexpected member name: {}" ExprStr))
      (result
        (create-op "nbdl.constexpr"
          (loc: Loc)
          (operands:)
          (attributes: ("expr" (string-attr ExprStr)))
          (result-types: !nbdl.store))))

    ; Build a key for store-compose.
    (define (build-store-key Loc Key)
      (cond
        ((value? Key) Key)
        ((member-name-expr? Key)
           (build-member-name Loc Key))
        (else (build-constexpr Loc Key))))

    ;; Expects name begins with a .
    (define (build-member-name Loc Name)
      (define StrippedName
        (begin
          (unless (member-name-expr? Name)
            (error "expecting member name: {}" Name))
          (string-copy Name 1)))
      (result
        (create-op "nbdl.member_name"
          (loc: Loc)
          (operands:)
          (attributes: ("name" (string-attr StrippedName)))
          (result-types: !nbdl.member_name))))

    (define (build-cont ResultArg)
      (create-op "nbdl.cont"
        (loc: 0)
        (operands: ResultArg)
        (attributes:)
        (result-types:)
        ))

    (define-syntax store
      (syntax-rules (init-args:)
        ((store Typename)
         (store Typename (init-args:)))
        ((store Typename (init-args: InitArgs ...))
         (result
           (create-op "nbdl.store"
             (loc: (syntax-source-loc Typename))
             (operands: (map maybe-build-expr
                             '((syntax-source-loc InitArgs) ...)
                             (list InitArgs ...)))
             (attributes: ("name" (flat-symbolref-attr Typename)))
             (result-types: !nbdl.store)
             )))))

    (define-syntax store-compose
      (syntax-rules ()
        ((store-compose Key Store)
         (lambda (ParentStore)
           (let ((KeyLoc (syntax-source-loc Key)))
             (result
               (create-op "nbdl.store_compose"
                 (loc: KeyLoc)
                 (operands: (build-store-key KeyLoc Key) Store ParentStore)
                 (attributes:)
                 (result-types: !nbdl.store)
                 )))))))

    (define-syntax variant
      (syntax-rules ()
        ((variant Store1 StoreN ...)
          (result
            (create-op "nbdl.variant"
              (loc: (syntax-source-loc Store1))
              (operands: Store1 StoreN ...)
              (attributes:)
              (result-types: !nbdl.variant)
              )))))

    (define-syntax define-store
      (syntax-rules ()
        ((define-store Name (InitArgs ...) StoreFunctionalN ...)
          (with-builder
            (lambda ()
              (at-block-end (entry-block current-nbdl-module))
              (let ()
                (define Op
                  (create-op "nbdl.define_store"
                    (loc: (syntax-source-loc Name))
                    (operands:)
                    (attributes: ("sym_name" (string-attr Name)))
                    (result-types:)
                    (region: "body" ((InitArgs : !nbdl.store) ...)
                      (define Parent (build-unit))
                      (define (ProcessBody BodyEl)
                        (set! Parent
                          (cond
                            ((procedure? BodyEl)
                              (BodyEl Parent))
                            ; Allow specifying a single store
                            ((value? !nbdl.unit Parent)
                             (begin
                               ; TODO Delete the dangling nbdl.unit operation here.
                               BodyEl))
                            (else
                              (error "expecting store functional: {}" BodyEl)))))
                      (ProcessBody StoreFunctionalN) ...
                      (create-op "nbdl.cont"
                        (loc: (syntax-source-loc Name))
                        (operands: Parent)
                        (attributes:)
                        (result-types:))
                      )))
                (unless (verify Op)
                  (error "operation failed verification: {}" Op))
                (translate-cpp Op lexer-writer)
                Op))))))

    ;; For now, this is just an alternative interface to define-store.
    ;; The idea was to encapsulate a root node in the state graph
    ;; but the benefit is not apparent.
    (define-syntax context
      (syntax-rules (member: init-args:)
        ((context Name (Formals ...)
            (member: Key1 Typename1 (init-args: InitArgs1N ...))
            (member: KeyN TypenameN (init-args: InitArgsNN ...)) ...)
         (define-store Name (Formals ...)
           (store-compose Key1 (store Typename1 (init-args: InitArgs1N ...)))
           (store-compose KeyN (store TypenameN (init-args: InitArgsNN ...)))
           ...
           ))))

    ;; Define a function to receive a matched set of parameters.
    ;; Each path node should be of the format:
    ;;  (%Kind Loc Args...)
    ;; or specifically for nested pathspecs:
    ;;  (%nbdl-path PathNodes...)
    (define-syntax match-params-fn
      (syntax-rules ()
        ((match-params-fn name (stores ... fn) body ...)
         (let ((FuncOp
                (%build-match-params
                  name
                  (length '(stores ...))
                  (lambda (stores ... %FnVal)
                    (define (Fn . PathSpec)
                      (define Loc (source-loc name))
                      (define (VisitFn ParamVals)
                        (build-resolve-params Loc %FnVal ParamVals))
                      (%match-params-spec PathSpec VisitFn))
                    ((lambda (fn) body ...) Fn)
                    ))))
            (unless (verify FuncOp)
              (error "verification failed: {0} {1}" name FuncOp))
            (translate-cpp FuncOp lexer-writer)
            FuncOp))))

    ; ParamsSpec is a list of PathSpecs
    ; ParamsFn is the callback taking the list of results.
    (define (%match-params-spec ParamsSpec ParamsFn)
      (let Loop ((ParamValsRev '()) ; Reverse ordered
                 (CurPathSpec (car ParamsSpec))
                 (Rest (cdr ParamsSpec)))
        (define (NextFn ParamVal)
          (define NewParamValsRev
            (cons ParamVal ParamValsRev))
          (cond
            ((pair? Rest)
              (Loop NewParamValsRev
                    (car Rest)
                    (cdr Rest)))
            ((null? Rest)
              (ParamsFn (reverse NewParamValsRev)))
            (else (error "expecting proper list" Rest))))
        (%match-path-spec CurPathSpec NextFn)))

    (define (%match-path-spec PathSpec Fn)
      (cond
        ((value? PathSpec)
         (Fn PathSpec))
        ((expr? PathSpec)
         (let ((Loc (cadr PathSpec))
               (Thunk (car (cddr PathSpec))))
           (Thunk Loc Fn)))
        ((and (pair? PathSpec)
              (eqv? '%nbdl-path (car PathSpec)))
         (let ((RootStore (cadr PathSpec))
               (PathNodes (cddr PathSpec)))
          (cond
            ((and (value? RootStore)
                  (pair? PathNodes))
              (%match-path-spec-rec RootStore PathNodes Fn))
            ((value? RootStore)
              (match-unit RootStore Fn))
            (else
              (error "expecting a root store object in pathspec: {}"
                     PathSpec)))))
        (else (error "expecting nbdl pathspec: {}" PathSpec))))

    (define (%match-path-spec-rec Store PathNodes Fn)
      (let Loop ((Loc (source-loc PathNodes))
                 (PathNode (car PathNodes))
                 (Rest (cdr PathNodes))
                 (CurStore Store))
        (define (NextFn StoreResult)
          (cond
            ((pair? Rest)
             (Loop (source-loc Rest)
                   (car Rest)
                   (cdr Rest)
                   StoreResult))
            ; Finish by match with unit-key to "unwrap" store.
            ((null? Rest)
             (match-unit StoreResult Fn))
            (else (error "expecting proper list"))))
        (%match-path-node CurStore Loc PathNode NextFn)))

    ;; Simply apply the unit-key to a mlir.value Store.
    (define (match-unit Store Fn)
      (match-key (source-loc) Store (build-unit) Fn))

    ;; We have mlir.values for both Store and Key
    (define (match-key Loc Store Key Fn)
      (create-op "nbdl.match"
        (loc: Loc)
        (operands: Store Key)
        (attributes:)
        (result-types:)
        (region: "overloads" ()
          (create-op "nbdl.overload"
            (loc: Loc)
            (operands:)
            (attributes: ("type" (string-attr "")))
            (result-types:)
            (region: "body" ((ResolvedStore : !nbdl.store))
              (Fn ResolvedStore)
              )))))

    (define (member-name-expr? PathNode)
      (and (symbol? PathNode)
           (eq? (string-ref PathNode 0) #\.)))

    (define (%match-path-node Store Loc PathNode Fn)
      (close-previous-scope)
      (let ((PathNode
              (maybe-build-expr+ Loc PathNode)))
        (cond
          ; Member name is the only key kind where nbdl.get is required
          ; but we have to apply the identity first to unwrap the store.
          ; (Which means the member name is applied to all alternatives.)
          ((value? !nbdl.member_name PathNode)
            (match-unit Store
              (lambda (MatchedStore)
                (define MemberStore
                  (build-node-get MatchedStore Loc
                                  PathNode))
                (Fn MemberStore))))
          ; Any other resolved mlir.value.
          ((value? PathNode)
            (match-key Loc Store PathNode Fn))
          ; Match a nested PathSpec then continue.
          ((path? PathNode)
            (%match-path-spec PathNode
              (lambda (KeyVal)
                (%match-path-node Store Loc KeyVal Fn))))
          (else (error "unsupported path node kind: {}" PathNode))
          )))

    (define (build-node-get Store Loc KeyVal)
      (define Op
        (create-op "nbdl.get"
          (loc: Loc)
          (operands: Store KeyVal)
          (attributes:)
          (result-types: !nbdl.store)))
      (result Op))

    (define (build-resolve-params Loc FnVal ParamVals)
      (unless (pair? ParamVals)
        (error "expecting list of params: {}" ParamVals))
      (let ()
        (define Result
          (result
            (create-op "nbdl.visit"
                       (loc: Loc)
                       (operands: FnVal ParamVals)
                       (attributes:)
                       (result-types: !nbdl.unit))))
        (create-op "nbdl.discard"
                   (loc: Loc)
                   (operands: Result)
                   (attributes:)
                   (result-types:))))



    (define (path? obj)
      (and (pair? obj)
           (eqv? (car obj) '%nbdl-path)))

    ;; Create a new path appending keys to the input path.
    (define-syntax get
      (syntax-rules ()
        ((get path key ...)
         (cond
          ((value? path)
            (append (list '%nbdl-path path)
                    (source-cons key '() (syntax-source-loc key)) ...))
          ((path? path)
            (append path
                    (source-cons key '() (syntax-source-loc key)) ...))
          ((else (error "invalid path object: {}" path)))
          ))
        ))

    ;; Apply a "Store" function to a list of Store operands.
    ;; - This will have a return value that is not necessarily stored.
    ;; - (e.g. string concatentation for creating an html attribute.)
    ;; TODO REMOVE apply-func
    (define-syntax apply-func
      (syntax-rules ()
        ((apply-func FnStore Store1 StoreN ...)
          (create-op "nbdl.apply_func"
            (loc: (syntax-source-loc FnStore))
            (operands: FnStore Store1 StoreN ...)
            (attributes:)
            (result-types: !nbdl.store)))))

    (define matching-results? #f)

    ;; Indicate that we require intermediate result values
    ;; from a ParamsSpec usually to become operands
    ;; to a call to visit.
    (define (%match-results ParamsSpec Fn)
      (define prev matching-results?)
      (dynamic-wind
        (lambda ()
          (set! matching-results? #t))
        (lambda ()
          (%match-params-spec ParamsSpec Fn))
        (lambda ()
          (set! matching-results? prev))))

    (define (%top-level Thunk)
      (define prev matching-results?)
      (dynamic-wind
        (lambda ()
          (set! matching-results? #f))
        Thunk
        (lambda ()
          (set! matching-results? prev))))

    ;; Create a thunk that should receive a location and callback
    ;; to resolve a value once its dependencies are resolved.
    ;; (e.g. arguments to visit)
    (define-syntax %expr
      (syntax-rules ()
        ((%expr Loc Thunk)
         (list '%nbdl-expr Loc Thunk))))

    (define (expr? Arg)
      (and (pair? Arg) (eqv? '%nbdl-expr (car Arg))))

    (define (build-visit MatchingResults? Loc Results)
      (define ResultType
        (if MatchingResults?
          !nbdl.store
          !nbdl.unit))
      (define VisitResult
        (result
          (create-op "nbdl.visit"
                     (loc: Loc)
                     (operands: Results)
                     (attributes:)
                     (result-types: ResultType))))
      (unless MatchingResults?
        (create-op "nbdl.discard"
                   (loc: Loc)
                   (operands: VisitResult)
                   (attributes:)
                   (result-types:)))
      VisitResult)

    (define (visit-impl MatchingResults? Loc ParamsSpec)
      (close-previous-scope)
      (if MatchingResults?
        (%expr Loc
               (lambda (Loc Fn)
                 (%match-results
                   ParamsSpec
                   (lambda (Results)
                     (Fn (build-visit MatchingResults? Loc Results))))))
        (%match-results
          ParamsSpec
          (lambda (Results)
            (build-visit MatchingResults? Loc Results)))))

    ;; Analogous to std::visit but it takes stores
    ;; for all of its parameters including the callee.
    ;;
    ;; The callee accepts a member name which is mapped
    ;; to a member expression with the first argument as
    ;; the owning object.
    ;;
    ;; Return the result only if it is not discarded.
    (define-syntax visit
      (syntax-rules ()
        ((visit Callee StoreN ...)
         (let ()
           (define MatchingResults? matching-results?)
           (define CalleeLoc (syntax-source-loc Callee))
           (define ParamsSpec
             (list
               (%expr CalleeLoc
                 (lambda (Loc Fn)
                   (%match-expr+ Loc Callee Fn)))
               (%expr (syntax-source-loc StoreN)
                 (lambda (Loc Fn)
                   (%match-expr Loc StoreN Fn)))
               ...))
           (visit-impl matching-results? CalleeLoc ParamsSpec)
           ))))

    ;; TODO Figure out what this returns exactly.
    ;; Visit each element of a range.
    (define (for-each path proc)
      (if (path? path)
        (error "TODO Implement for-each")
        (error "expecting a path")))

    (define-syntax match-aux
      (syntax-rules (=>)
        ((match-aux PathSpec
          (TypeN => FnN) ...)
         (%match-path-spec PathSpec
          (lambda (Store)
            (%top-level
              (lambda()
                (define Key (build-unit))
                (close-previous-scope)
                (create-op "nbdl.match"
                  (loc: (syntax-source-loc PathSpec))
                  (operands: Store Key)
                  (attributes:)
                  (result-types:)
                  (region: "overloads" ()
                    (create-op "nbdl.overload"
                      (loc: (syntax-source-loc TypeN))
                      (operands:)
                      (attributes: ("type" (string-attr TypeN)))
                      (result-types:)
                      (region: "body" ((OverloadArg : !nbdl.store))
                        (FnN OverloadArg))) ...)))))))))

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
        ((match PathSpec
          (else => DefaultFn))
         (match PathSpec
           ("" => DefaultFn)))
        ((match Path
          (Type1 => Fn1)
          (TypeN => FnN) ...
          (else => DefaultFn))
         (match Path
           (Type1 => Fn1)
           (TypeN => FnN) ...
           ("" => DefaultFn)))
        ((match Store
           (Type1 => Fn1)
           (TypeN => FnN) ...)
         (match-aux Store
           (Type1 => Fn1)
           (TypeN => FnN) ...))
        ))

    ; Visit store and do nothing even if there is
    ; butterscotch in a crystal bowl on the table.
    (define (noop Store)
      (create-op "nbdl.noop"
        (loc: 0)
        (operands: Store)
        (attributes:)
        (result-types:))
      (when #f #f))

    ;; Match the first satisified condition.
    ;; - Each clause should be (unary-predicate proc)
    ;;   where proc is a unary lambda receiving the matched path.
    (define (match-cond path . clauses)
      (if (path? path)
        (error "TODO implement match-cond")
        (error "expecting a path")))

    ; FIXME Make this dump to error output.
    (define (dump-cpp name)
      (define Op
        (module-lookup current-nbdl-module name))
      (translate-cpp Op)
      (newline))

    (define (dump-op name)
      (define Op
        (module-lookup current-nbdl-module name))
      (dump Op)
      (newline))


  ) ; end of... begin
  (export
    context
    define-store
    store-compose
    variant
    store
    get
    key-at
    match
    match-cond
    match-params-fn
    visit
    noop
    dump-cpp
    dump-op
    ; reexport some base stuff
    define
    define-syntax
    error
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
