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

TokenType lisp::getTokenType(std::string token) {
  if(isNil(token)) return TokenType::NIL;
  if(isInteger(token)) return TokenType::INTEGER;
  if(isSymbol(token)) return TokenType::SYMBOL;
  if(token == "(") return TokenType::LPAREN;
  if(token == ")") return TokenType::RPAREN;
  if(token == ".") return TokenType::PERIOD;
  else return TokenType::UNKNOWN;
}

Atom lisp::readList(std::list<std::string>& tokens) {
  Atom result = cons(makeNil(), makeNil());
  Atom* current = &result;
  //if(tokens.empty()) return makeNil();

  for(;;) {
    std::string token(tokens.front());
    //tokens.pop_front();
    if(token == ")") break;
    else if(token == ".") {
      tokens.pop_front();
      token = tokens.front();
      *current = readFrom(tokens);
      // TODO improper list
    }
    else {
      Atom a = readFrom(tokens);
      if(nilp(std::get<Pair*>(result)->head)) { // first item
        std::get<Pair*>(*current)->head = a;
        current = &(std::get<Pair*>(*current)->tail);
      } else {
        *current = cons(a, makeNil());
        current = &(std::get<Pair*>(*current)->tail);
      }
    }
  }
  return result;
}

Atom lisp::readFrom(std::list<std::string>& tokens) {
  // std::cout << "\n\n----\n";
  // std::cout << "toks: ";
  // for(auto& a: tokens) std::cout << a << " ";
  // std::cout << "\n";
  if(tokens.empty()) return makeNil();

  std::string token(tokens.front());
  tokens.pop_front();
  switch(getTokenType(token)) {
    case TokenType::LPAREN: {
      return readList(tokens);
    }
    case TokenType::NIL: {
      return makeNil();
    }
    case TokenType::SYMBOL: {
      return makeSymbol(token);
    }
    case TokenType::INTEGER: {
      long l = std::stol(token);
      return makeInt(l);
    }
    case TokenType::RPAREN: {
      return makeNil();
    }
    case TokenType::PERIOD: {
      return makeNil();
    }
    case TokenType::UNKNOWN: {
      throw LispException("readFrom: Unknown token type: " + token);
    }
    default: {
      throw LispException("readFrom: Unknown token type: " + token);
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

  if(!listp(expr)) throw LispException("evaluateExpression: Improper list");

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
