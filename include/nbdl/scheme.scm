; The functions defined for the DSL typically take objects that represent
; either compile-time constants or objects that are resolved at runtime
; using paths.

; Path creation and manipulation
; - A Path is a list of Keys. This is used to recursively visit contained
;   objects within the applications state.
; - A Key is an object used to resolve an object within
;   a Store or State object. Keys can be compile-time constants
;   or resolved objects. A key is like an edge in a graph.

; get
;   - Specify a path
(define get keys
  "TODO Implement")

; key-at
;   - Resolve a key that has run-time state.
;     An example is the key to a std::unordered_map.
(define key-at path
  "TODO Implement")

; Store object resolution.

; match-cond
;   - Match an object using cond like syntax.
;     Predicates are applied to the resolved object.
;     The (pred => expr) form expects expr to yield
;     function that receives the matched value as an argument.
;   - Accept the else auxilary syntax.
;   - Fail if no condition is satisfied.
(define match-cond (path . conds)
  "TODO Implement via define-syntax")

; match
;   - Match an object by its type or its type applied to a concept
;     instead of raw predicates.
;     So, the syntax for each condition is like the following:
;       (typename|concept => expr)
;   - Accept the else auxilary syntax.
;   - Fail if no test matches the objects type.
(define match (path . key-conds)
  "TODO Implement via define-syntax")

; for-each
;   - Visit each element an iterable object
;     using C++ function.
(define for-each (path . fn)
  "TODO")

; apply
;   - Transform a set of resolved objects to a single object.
;     (Params are paths)
(define apply (fn . params)
  "TODO")

; Predicates

; prepend
;   - Prepend text via curried interface.
(define prepend (arg)
  "TODO implement in terms of concat?")

; equal
;   - Compare value equality with curried interface.
;     Implement via operator==
(define equal (arg)
  "TODO make unary equal possibly curried")

; concat 
;   - Generate concatentated text implicitly converting arguments to strings.
(define concat pstrings
  "TODO")

