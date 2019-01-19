#include "lisp.hpp"
#include <cctype>
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

std::vector<std::string> lisp::tokenize(std::string const str) {
  std::vector<std::string> result;
  int i = 0;
  while(i < str.length()) {
    if(str[i] == '(') {
      result.push_back("(");
    } else if(str[i] == ')') {
      result.push_back(")");
    } else if(std::isspace(str[i])) {
      // do nothing
    } else {
      std::string token;
      while(i < str.length()
      && str[i] != '('
      && str[i] != ')'
      && !std::isspace(str[i])) {
        token.push_back(str[i]);
        ++i;
      }
      result.push_back(token);
      continue; // avoid double increment
    }
    ++i;
  }
  return result;
}
