#include "lisp.hpp"
using namespace lisp;

Atom lisp::symbolTable {Nil{}};

bool lisp::nilp(Atom atom) {
  return std::holds_alternative<Nil>(atom.value);
}

Atom lisp::cons(Atom head, Atom tail) {
  return Atom { new Pair { head, tail } };
}

Atom lisp::makeNil() {
  return Atom { Nil{} };
}

Atom lisp::makeInt(long x) {
  return Atom { x };
}

Atom lisp::makeSymbol(std::string const s) {
  Atom a, p;
  p = symbolTable;
  while(!nilp(p)) {
    a = std::get<Pair*>(p.value)->head;
    if(std::get<std::string>(a.value) == s) {
      return a;
    }
    p = std::get<Pair*>(p.value)->tail;
  }
  a = Atom{s};
  lisp::symbolTable = cons(a, symbolTable);
  return a;
}

void lisp::printExpr(Atom atom) {
  if(std::holds_alternative<Nil>(atom.value)) {
    std::cout << "NIL";
  }
  if(std::holds_alternative<Pair*>(atom.value)) {
    std::cout << "(";
    printExpr(std::get<Pair*>(atom.value)->head);
    Atom currentAtom = std::get<Pair*>(atom.value)->tail;
    while(!nilp(currentAtom)) {
      if(std::holds_alternative<Pair*>(currentAtom.value)) {
        std::cout << " ";
        printExpr(std::get<Pair*>(currentAtom.value)->head);
        currentAtom = std::get<Pair*>(currentAtom.value)->tail;
      } else {
        std::cout << " . ";
        printExpr(currentAtom);
        break;
      }
    }
    std::cout << ")";
  }
  if(std::holds_alternative<std::string>(atom.value)) {
    std::cout << std::get<std::string>(atom.value);
  }
  if(std::holds_alternative<long>(atom.value)) {
    std::cout << std::get<long>(atom.value);
  }
}
