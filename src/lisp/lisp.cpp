#include "lisp.hpp"
#include <cctype>
#include <regex>
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

std::list<std::string> lisp::tokenize(std::string const str) {
  std::list<std::string> result;
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

bool lisp::isInteger(std::string const s) {
    return std::regex_match(s, std::regex("[(-|+)|][0-9]+"));
}

bool lisp::isNil(std::string const s) {
    return s == "nil";
}

bool lisp::isSymbol(std::string const s) {
  if(s.length() > 1) return true;
  if(s.length() == 1) {
    if(s[0] == ')' || s[0] == '(') {
      return false;
    }
    else return true;
  }
  else throw LispException("tokenize: Parsing token with length 0");
}

Atom lisp::parseSimple(std::string const str) {
  if(isInteger(str)){
    long l = std::stol(str);
    std::cout << "    found integer: " << l << "\n";
    return makeInt(l);
  }
  if(isNil(str)) {
    return Atom{ Nil{} };
  }
  if(isSymbol(str)) {
    return makeSymbol(str);
  }
  if(str == "(" || str == ")") {
    // TODO does this make sense?
    //return Atom{ Nil{} };
  }
  throw LispException("parseSimple: Simple token type not implemented for string: " + str);
}

Atom lisp::readFrom(std::list<std::string> tokens) {
  // std::cout << "--tokens--\n";
  // for(auto& a: tokens) std::cout << a << "\n";
  // std::cout << "--\n\n";

  // Empty list => nil
  if(tokens.size() == 0) {
    return makeNil();
  }

  // If just one token, parse it
  if(tokens.size() == 1) {
    return parseSimple(tokens.front());
  }

  // If two tokens, parse those
  else if(tokens.size() == 2) {
    if(tokens.front() == "(" && tokens.back() == ")") return makeNil();
    return cons(parseSimple(tokens.front()), parseSimple(tokens.back()));
  }

  // Else strip parens and recurse
  else {
    {
      int numberOfLParens = 0;
      int numberOfRParens = 0;
      for(auto& token : tokens) {
        if(token == "(") ++numberOfLParens;
        if(token == ")") ++numberOfRParens;
      }
      if(numberOfLParens != numberOfRParens) {
        throw LispException("readFrom: Mismatched parens");
      }
    }
    // Number of parens looks good

    std::string token(tokens.front());
    if(token == "(") {
      std::list<std::string> tokensInFirstParens;
      int noLs = 0;
      int noRs = 0;
      do {
        token = tokens.front();
        tokens.pop_front();
        tokensInFirstParens.push_back(token);
        if(token == "(") ++noLs;
        if(token == ")") ++noRs;
      } while(noLs != noRs && tokens.size() != 0);
      if(tokensInFirstParens.front() == "(" && tokensInFirstParens.back() == ")") {
        tokensInFirstParens.pop_front();
        tokensInFirstParens.pop_back();
      }
      // std::cout << "--toks in first parens--\n";
      // for(auto& a: tokensInFirstParens) std::cout << a << "\n";
      // std::cout << "--\n\n";
      //
      // std::cout << "--rest of toks--\n";
      // for(auto& a: tokens) std::cout << a << "\n";
      // std::cout << "--\n\n";
      return cons(readFrom(tokensInFirstParens), readFrom(tokens));
    } else {
      tokens.pop_front();
      return cons(parseSimple(token), readFrom(tokens));
    }
  }
}

Atom lisp::readExpression(std::string const expression) {
  std::list<std::string> tokens = tokenize(expression);
  return readFrom(tokens);
}
