#include "lisp.hpp"
#include <cctype>
#include <regex>
using namespace lisp;

Atom lisp::symbolTable {Nil{}};

bool lisp::nilp(Atom atom) {
  return std::holds_alternative<Nil>(atom);
}

bool lisp::listp(Atom expr) {
  while (!nilp(expr)) {
    if(!std::holds_alternative<Pair*>(expr)) return false;
    expr = std::get<Pair*>(expr)->tail;
  }
  return true;
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
    a = std::get<Pair*>(p)->head;
    if(std::get<std::string>(a) == s) {
      return a;
    }
    p = std::get<Pair*>(p)->tail;
  }
  a = Atom{s};
  lisp::symbolTable = cons(a, symbolTable);
  return a;
}

void lisp::printExpr(Atom atom) {
  if(std::holds_alternative<Nil>(atom)) {
    std::cout << "NIL";
  }
  if(std::holds_alternative<Pair*>(atom)) {
    std::cout << "(";
    printExpr(std::get<Pair*>(atom)->head);
    Atom currentAtom = std::get<Pair*>(atom)->tail;
    while(!nilp(currentAtom)) {
      if(std::holds_alternative<Pair*>(currentAtom)) {
        std::cout << " ";
        printExpr(std::get<Pair*>(currentAtom)->head);
        currentAtom = std::get<Pair*>(currentAtom)->tail;
      } else {
        std::cout << " . ";
        printExpr(currentAtom);
        break;
      }
    }
    std::cout << ")";
  }
  if(std::holds_alternative<Symbol>(atom)) {
    std::cout << std::get<Symbol>(atom);
  }
  if(std::holds_alternative<Integer>(atom)) {
    std::cout << std::get<Integer>(atom);
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
    //std::cout << "    found integer: " << l << "\n";
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
    if(tokens.front() == ".") return parseSimple(tokens.back());
    return cons(parseSimple(tokens.front()), cons(parseSimple(tokens.back()), makeNil()));
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

Atom lisp::createEnv(Atom parent) {
  return cons(parent, makeNil());
}

Atom lisp::getEnv(Atom env, Atom symbol) {
  if(!std::holds_alternative<Pair*>(env)) throw LispException("getEnv: Env not pair");
  if(!std::holds_alternative<Symbol>(symbol)) throw LispException("getEnv: Symbol not symbol");

  Atom parent = std::get<Pair*>(env)->head;
  Atom bs = std::get<Pair*>(env)->tail;

  while(!nilp(bs)) {
    if(!std::holds_alternative<Pair*>(bs)) throw LispException("getEnv: bs not pair");
    Atom b = std::get<Pair*>(bs)->head;
    if(!std::holds_alternative<Pair*>(b)) throw LispException("getEnv: b not pair");
    if(std::get<std::string>(std::get<Pair*>(b)->head) == std::get<std::string>(symbol)) {
      return std::get<Pair*>(b)->tail;
    }
    bs = std::get<Pair*>(bs)->tail;
  }
  if(nilp(parent)) throw LispException("getEnv: Symbol unbound");
  return getEnv(parent, symbol);
}

void lisp::setEnv(Atom env, Atom symbol, Atom value) {
  if(!std::holds_alternative<Pair*>(env)) throw LispException("getEnv: Env not pair");
  if(!std::holds_alternative<Symbol>(symbol)) throw LispException("getEnv: Symbol not symbol");

  Atom bs = std::get<Pair*>(env)->tail;
  Atom b = makeNil();

  while(!nilp(bs)) {
    b = std::get<Pair*>(bs)->head;
    if(!std::holds_alternative<Pair*>(b)) throw LispException("getEnv: b not pair");
    if(std::get<std::string>(std::get<Pair*>(b)->head) == std::get<std::string>(symbol)) {
      std::get<Pair*>(b)->tail = value;
      return;
    }
    bs = std::get<Pair*>(bs)->tail;
  }
  b = cons(symbol, value);
  std::get<Pair*>(env)->tail = cons(b, std::get<Pair*>(env)->tail);
}

Atom lisp::evaluateExpression(Atom expr, Atom env) {
  Atom op;
  Atom args;

  if(std::holds_alternative<Symbol>(expr)) {
    return getEnv(env, expr);
  } else if(!std::holds_alternative<Pair*>(expr)) {
    return expr;
  }

  if(!listp(expr)) throw LispException("evaluateExpression: Illformed syntax");

  op = std::get<Pair*>(expr)->head;
  args = std::get<Pair*>(expr)->tail;

  if(std::holds_alternative<Symbol>(op)) {
    if(std::get<Symbol>(op) == "quote") {
      if(nilp(args) || !nilp(std::get<Pair*>(args)->tail)) throw LispException("evaluateExpression: Argument error");
      return std::get<Pair*>(args)->head;
    } else if(std::get<Symbol>(op) == "def") {
      Atom sym, val;
      if(nilp(args) || nilp(std::get<Pair*>(args)->tail)
      || !nilp(std::get<Pair*>(std::get<Pair*>(args)->tail)->tail)) {
        throw LispException("evaluateExpression: Argument error");
      }
      sym = std::get<Pair*>(args)->head;
      if(!std::holds_alternative<Symbol>(sym)) throw LispException("evaluateExpression: Type error");

      val = evaluateExpression(std::get<Pair*>(std::get<Pair*>(args)->tail)->head, env);
      setEnv(env, sym, val);
      return sym;
    }
  }
  throw LispException("evaluateExpression: Illformed syntax");
}
