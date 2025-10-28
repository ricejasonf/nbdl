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
        (cond
          ((number? Arg)
            (attr (number->string Arg) i32))
          ((symbol? Arg)
            (string-attr Arg))
          ((string? Arg)
              (string-attr Arg))
          (else #f)))
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


    ;; Expects name begins with a .
    (define (build-member-name Loc Name)
      (define StrippedName
        (begin
          (unless (member-name-key? Name)
            (error "expecting member name: {}" Name))
          (string-copy Name 1)))
      (result
        (create-op "nbdl.member_name"
          (loc: Loc)
          (operands:)
          (attributes: ("name" (string-attr StrippedName)))
          (result-types: !nbdl.symbol))))

    (define (build-cont ResultArg)
      (create-op "nbdl.cont"
        (loc: 0)
        (operands: ResultArg)
        (attributes:)
        (result-types:)
        ))

    ;; Build a Store and StoreCompose it with Parent
    (define (build-store-node Parent Key Typename InitArgs)
      (define KeyLoc (source-loc Key))
      (define NewStore
        (result
          (create-op "nbdl.store"
            (loc: Typename)
            (operands: (map maybe-literal-op* InitArgs))
            (attributes: ("name" (flat-symbolref-attr Typename)))
            (result-types: !nbdl.store)
            )))
      (result
        (create-op
          "nbdl.store_compose"
          (loc: Key)
          (operands: (build-member-name KeyLoc Key) NewStore Parent)
          (attributes:)
          (result-types: !nbdl.store)))
      )

    (define-syntax context
      (syntax-rules (member: init-args:)
        ((context Name (Formals ...)
            (member: Key1 Typename1 (init-args: InitArgs1N ...))
            (member: KeyN TypenameN (init-args: InitArgsNN ...)) ...)
          (%build-context
            Name
            (length '(Formals ...))
            (lambda (Formals ...)
              (define Parent (build-unit))
              (define (member Key Typename . InitArgs)
                (set! Parent (build-store-node Parent Key Typename InitArgs)))
              (member Key1 Typename1 InitArgs1N ...)
              (member KeyN TypenameN InitArgsNN ...) ...
              (translate-cpp
                (parent-op (build-cont Parent))
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
              (define (Fn . PathSpec)
                (define Loc (source-loc name))
                (define (VisitFn ParamVals)
                  (build-visit-params Loc %FnVal ParamVals))
                (%match-params-spec PathSpec VisitFn))
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
        ((or (value? !nbdl.store PathSpec)
             (value? !nbdl.unit PathSpec))
           (Fn PathSpec))
        ((and (pair? PathSpec)
              (eqv? '%nbdl-path (car PathSpec)))
         (let ((RootStore (cadr PathSpec))
               (PathNodes (cddr PathSpec)))
          (if (value? RootStore)
            (%match-path-spec-rec RootStore PathNodes Fn)
            (error "expecting a root store object in pathspec: {}" PathSpec))))
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

    (define (member-name-key? PathNode)
      (and (symbol? PathNode)
           (eq? (string-ref PathNode 0) #\.)))

    (define (%match-path-node Store Loc PathNode Fn)
      (cond
        ; Member name is the only key kind where nbdl.get is required
        ; but we have to apply the identity first to unwrap the store.
        ; (Which means the member name is applied to all alternatives.)
        ((member-name-key? PathNode)
          (match-unit Store
            (lambda (MatchedStore)
              (define MemberStore
                (build-node-get MatchedStore Loc
                                (build-member-name Loc PathNode)))
              (Fn MemberStore))))
        ; All other symbols are treated as constexpr expressions.
        ((symbol? PathNode)
          (match-key Loc Store (build-constexpr Loc PathNode) Fn))
        ; Number literal
        ((number? PathNode)
          (match-key Loc Store (build-literal Loc PathNode) Fn))
        ; String literal
        ((string? PathNode)
          (match-key Loc Store (build-literal Loc PathNode) Fn))
        ; Mlir.value that is the Key.
        ((value? PathNode)
          (match-key Loc Store PathNode Fn))
        ; Match a nested PathSpec then continue
        ((path? PathNode)
          (%match-path-spec PathNode
            (lambda (KeyVal)
              (%match-path-node Store Loc KeyVal Fn))))
        (else (error "unsupported path node kind: {}" PathNode))
        ))

    (define (build-node-get Store Loc KeyVal)
      (define Op
        (create-op "nbdl.get"
          (loc: Loc)
          (operands: Store KeyVal)
          (attributes:)
          (result-types: !nbdl.store)))
      (result Op))

    (define (build-visit-params Loc FnVal ParamVals)
      (unless (pair? ParamVals)
        (error "expecting list of params: {}" ParamVals))
      (create-op "nbdl.visit"
        (loc: Loc)
        (operands: FnVal ParamVals)
        (attributes:)
        (result-types:)))

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

    ;; Given a C++ function, return a lambda that takes a list of paths
    ;; that resolve to a list of parameters to apply to the function.
    ;; - This will have runtime effects that are not necessarily stored.
    ;; - (e.g. string concatentation for creating an attribute.)
    (define (apply-fn cpp-func)
      (lambda paths
        (apply nbdl-impl-apply cpp-func paths)))

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
            (define Key (build-unit))
            (create-op "nbdl.match"
              (loc: 0)
              (operands: Store Key)
              (attributes:)
              (result-types:)
              (region: "overloads" ()
                (create-op "nbdl.overload"
                  (loc: (source-loc TypeN))
                  (operands:)
                  (attributes: ("type" (string-attr TypeN)))
                  (result-types:)
                  (region: "body" ((OverloadArg : !nbdl.store))
                    (FnN OverloadArg))) ...)))))))

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
      (translate-cpp Op))

    (define (dump-op name)
      (define Op
        (module-lookup current-nbdl-module name))
      (dump Op))


  ) ; end of... begin
  (export
    apply-fn
    context
    get
    key-at
    match
    match-cond
    match-params-fn
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
