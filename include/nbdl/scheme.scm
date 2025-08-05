;;; DEPRECATED FILE FOR REFERENCE ONLY
; The functions defined for the DSL typically take objects that represent
; either compile-time constants or objects that are resolved at runtime
; using paths.

; Some definitions:

; Key
; - A Key is an object used to resolve an object within
;   a Store or State object. Keys can be compile-time constants
;   or resolved objects. A key is like an edge in a graph.

; Path
; - A Path is a list of Keys. This is used to recursively visit contained
;   objects within the applications state. A path is like a path in a graph.

; Visitor
; - A Visitor is a native (C++) function that visits a
;   resolved object or mutliple resolved objects at run-time.
;   Visitors may be composed using point-free programming techniques.


; Path creation and manipulation directives

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
;     The (pred => expr) form expects expr to be a Visitor.
;   - Accept the else auxilary syntax.
;   - Fail if no condition is satisfied.
(define match-cond (path . conds)
  "TODO Implement via define-syntax")

; match
;   - Match an object by its type or by a concept
;     instead of raw predicates.
;     So, the syntax for each condition is like the following:
;       (typename|concept exprs...)
;       (typename|concept => visitor)
;   - Accept the else auxilary syntax.
;   - Fail if no test matches the objects type.
;   - Matches are checked linearly.
(define match (path . key-conds)
  "TODO Implement via define-syntax")

; for-each
;   - Visit each element of a range.
(define for-each (path . visitor)
  "TODO")

; apply
;   - Transform a set of resolved objects to a single object.
;     This is useful for filtering or formatting data.
(define apply (fn . params)
  "TODO")

; concat 
;   - Generate concatentated text implicitly converting arguments to strings.
(define concat strings
  "TODO")

; prepend
;   - Prepend text via curried interface.
(define prepend (arg)
  "TODO implement in terms of concat?")

; store
;   - Transform a set of resolved objects to a single object
;     and store result in a local Store at a given path. The
;     path should have a prefix that does not associate it with
;     a Producer.
;   - Fail if the path already exists.
;     (Params are paths)
;   - Store "unresolved" (empty monostate) if the params are not matched.
;     This is useful for storing local state.
(define store (fn dest-path . params)
  "TODO Will this automatically create a variant type
    or must the user explicitly specify this?")

; Predicates

; equal?
;   - Compare value equality with curried interface.
;     Implement via operator== (or mabye hana::equal).
(define equal? (arg)
  "TODO make unary equal possibly curried")
