#ifndef LISP_HPP
#define LISP_HPP
#include <variant>


namespace lisp {

enum class AtomType { NIL, PAIR, SYMBOL, INTEGER };

struct Pair;
struct Nil { };
struct Atom {
  std::variant<Nil, Pair*, std::string, long> value;
};

struct Pair {
  Atom head;
  Atom tail;
};

bool nilp(Atom atom);
Atom cons(Atom head, Atom tail);

// TODO this can be templated
Atom makeNil();
Atom makeInt(long x);
Atom makeSymbol(std::string const s);

void printExpr(Atom atom);

// TODO this could use e.g. std::set instead, but this is good enough for now
extern Atom symbolTable;

} // namespace lisp
#endif /* LISP_HPP */
