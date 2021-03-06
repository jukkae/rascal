(def (abs x) (if (< x 0) (* -1 x) x))

(def (foldl proc init list)
  (if list
      (foldl proc
             (proc init (car list))
             (cdr list))
      init))

(def (foldr proc init list)
  (if list
      (proc (car list)
            (foldr proc init (cdr list)))
      init))

(def (list . items)
  (foldr cons nil items))

(def (reverse list)
  (foldl (lambda (a x) (cons x a)) nil list))

(def (unary-map proc list)
  (foldr (lambda (x rest) (cons (proc x) rest))
         nil
         list))

(def (map proc . arg-lists)
  (if (car arg-lists)
      (cons (apply proc (unary-map car arg-lists))
            (apply map (cons proc
                             (unary-map cdr arg-lists))))
      nil))

(def (append a b) (foldr cons b a))

(def (caar x) (car (car x)))

(def (cadr x) (car (cdr x)))

(defmacro (quasiquote x)
  (if (pair? x)
      (if (eq? (car x) 'unquote)
          (cadr x)
          (if (and (pair? (car x)) (eq? (caar x) 'unquote-splicing))
              (list 'append
                    (cadr (car x))
                    (list 'quasiquote (cdr x)))
              (list 'cons
                    (list 'quasiquote (car x))
                    (list 'quasiquote (cdr x)))))
      (list 'quote x)))

(defmacro (let defs . body)
  `((lambda ,(map car defs) ,@body)
    ,@(map cadr defs)))

(def +
  (let ((old+ +))
    (lambda xs (foldl old+ 0 xs))))

(def (not a) (if a nil t))
