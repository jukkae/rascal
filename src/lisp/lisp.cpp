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
    expr = *tail(&expr);
  }
  return true;
}

Atom lisp::cons(Atom head, Atom tail) {
  return Atom { new Pair { head, tail } };
}

Atom* lisp::head(Atom* atom) {
  if(!std::holds_alternative<Pair*>(*atom)) throw LispException("Head: expression does not refer to a pair");
  else return &(std::get<Pair*>(*atom)->head);
}

Atom* lisp::tail(Atom* atom) {
  if(!std::holds_alternative<Pair*>(*atom)) throw LispException("Tail: expression does not refer to a pair");
  else return &(std::get<Pair*>(*atom)->tail);
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
    a = *head(&p);
    if(std::get<std::string>(a) == s) {
      return a;
    }
    p = *tail(&p);
  }
  a = Atom{s};
  lisp::symbolTable = cons(a, symbolTable);
  return a;
}

Atom lisp::makeBuiltin(Builtin func) {
  return Atom { func };
}

// Make shallow copy
Atom lisp::copyList(Atom list) {
  if(nilp(list)) return makeNil();
  Atom a, p;
  a = cons(*head(&list), makeNil());
  p = a;
  list = *tail(&list);
  while(!nilp(list)) {
    *tail(&p) = cons(*head(&list), makeNil());
    p = *tail(&p);
    list = *tail(&list);
  }
  return a;
}

Atom lisp::apply(Atom func, Atom args) {
  if(std::holds_alternative<Builtin>(func)) return std::get<Builtin>(func)(args);
  else throw LispException("Apply: Tried to apply non-function");
}

void lisp::printExpr(Atom atom) {
  if(std::holds_alternative<Nil>(atom)) {
    std::cout << "NIL";
  }
  if(std::holds_alternative<Pair*>(atom)) {
    std::cout << "(";
    printExpr(*head(&atom));
    Atom currentAtom = *tail(&atom);
    while(!nilp(currentAtom)) {
      if(std::holds_alternative<Pair*>(currentAtom)) {
        std::cout << " ";
        printExpr(*head(&currentAtom));
        currentAtom = *tail(&currentAtom);
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
  if(std::holds_alternative<Builtin>(atom)) {
    std::cout << "<BUILTIN>: " << &std::get<Builtin>(atom);
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
  return std::regex_match(s, std::regex(R"(-?\d+)"));
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
  Atom result = makeNil();
  Atom* current = &result;
  //if(tokens.empty()) return makeNil();

  for(;;) {
    // std::cout << "RL: toks: ";
    // for(auto& a: tokens) std::cout << a << " ";
    // std::cout << "\n";

    std::string token(tokens.front());
    // std::cout << "tok: " << token << "\n";
    //tokens.pop_front();
    if(token == ")") {
      tokens.pop_front(); // consume rparen
      break;
    }
    else if(token == ".") {
      tokens.pop_front();
      token = tokens.front();
      *current = readFrom(tokens);
    }
    else {
      Atom a = readFrom(tokens);
      if(nilp(result)) { // first item
        result = cons(makeNil(), makeNil());
        *head(current) = a;
        current = tail(current);
      } else {
        *current = cons(a, makeNil());
        current = tail(current);
      }
    }
  }
  return result;
}

Atom lisp::readFrom(std::list<std::string>& tokens) {
  // std::cout << "\n\nREADFROM\n";
  // std::cout << "RF: toks: ";
  // for(auto& a: tokens) std::cout << a << " ";
  // std::cout << "\n";
  //if(tokens.empty()) return makeNil();

  std::string token(tokens.front());
  // std::cout << "tok: " << token << "\n";

  tokens.pop_front();
  TokenType tokenType = getTokenType(token);
  if(tokenType == TokenType::LPAREN) {
    return readList(tokens);
  } else if (tokenType == TokenType::NIL) {
    return makeNil();
  } else if (tokenType == TokenType::SYMBOL) {
    return makeSymbol(token);
  } else if (tokenType == TokenType::INTEGER) {
    long l = std::stol(token);
    return makeInt(l);
  } else {
    throw LispException("readFrom: couldn't match token");
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

  Atom parent = *head(&env);
  Atom bs = *tail(&env);

  while(!nilp(bs)) {
    if(!std::holds_alternative<Pair*>(bs)) throw LispException("getEnv: bs not pair");
    Atom b = *head(&bs);
    if(!std::holds_alternative<Pair*>(b)) throw LispException("getEnv: b not pair");
    if(std::get<std::string>(*head(&b)) == std::get<std::string>(symbol)) {
      return *tail(&b);
    }
    bs = *tail(&bs);
  }
  if(nilp(parent)) throw LispException("getEnv: Symbol unbound");
  return getEnv(parent, symbol);
}

void lisp::setEnv(Atom env, Atom symbol, Atom value) {
  if(!std::holds_alternative<Pair*>(env)) throw LispException("getEnv: Env not pair");
  if(!std::holds_alternative<Symbol>(symbol)) throw LispException("getEnv: Symbol not symbol");

  Atom bs = *tail(&env);
  Atom b = makeNil();

  while(!nilp(bs)) {
    b = *head(&bs);
    if(!std::holds_alternative<Pair*>(b)) throw LispException("getEnv: b not pair");
    if(std::get<std::string>(*head(&b)) == std::get<std::string>(symbol)) {
      *tail(&b) = value;
      return;
    }
    bs = *tail(&bs);
  }
  b = cons(symbol, value);
  *tail(&env) = cons(b, *tail(&env));
}

Atom lisp::builtinHead(Atom args) {
  Atom result;
  if(nilp(args) || !nilp(*tail(&args))) throw LispException("builtin head: wrong number of args");

  if(nilp(*head(&args))) result = makeNil();
  else if(!std::holds_alternative<Pair*>(*head(&args))) throw LispException("builtin head: not pair");
  else result = *head(head(&args));
  return result;
}

Atom lisp::builtinTail(Atom args) {
  Atom result;
  if(nilp(args) || !nilp(*tail(&args))) throw LispException("builtin tail: wrong number of args");

  if(nilp(*head(&args))) result = makeNil();
  else if(!std::holds_alternative<Pair*>(*head(&args))) throw LispException("builtin tail: not pair");
  else result = *tail(head(&args));
  return result;
}

Atom lisp::builtinCons(Atom args) {
  if(nilp(args) || nilp(*tail(&args)) || !nilp(*tail(tail(&args)))) throw LispException("builtin cons: wrong number of args");
  return cons(*head(&args), *head(tail(&args)));
}

Atom lisp::builtinAdd(Atom args) {
  if(nilp(args) || nilp(*tail(&args)) || !nilp(*tail(tail(&args)))) throw LispException("builtin add: wrong number of args");
  Atom a = *head(&args);
  Atom b = *head(tail(&args));
  if(!std::holds_alternative<Integer>(a) || !std::holds_alternative<Integer>(b)) throw LispException("builtin add: not a number");
  return makeInt(std::get<Integer>(a) + std::get<Integer>(b));
}

Atom lisp::builtinSubtract(Atom args) {
  if(nilp(args) || nilp(*tail(&args)) || !nilp(*tail(tail(&args)))) throw LispException("builtin subtract: wrong number of args");
  Atom a = *head(&args);
  Atom b = *head(tail(&args));
  if(!std::holds_alternative<Integer>(a) || !std::holds_alternative<Integer>(b)) throw LispException("builtin subtract: not a number");
  return makeInt(std::get<Integer>(a) - std::get<Integer>(b));
}

Atom lisp::builtinMultiply(Atom args) {
  if(nilp(args) || nilp(*tail(&args)) || !nilp(*tail(tail(&args)))) throw LispException("builtin multiply: wrong number of args");
  Atom a = *head(&args);
  Atom b = *head(tail(&args));
  if(!std::holds_alternative<Integer>(a) || !std::holds_alternative<Integer>(b)) throw LispException("builtin multiply: not a number");
  return makeInt(std::get<Integer>(a) * std::get<Integer>(b));
}

Atom lisp::builtinDivide(Atom args) {
  if(nilp(args) || nilp(*tail(&args)) || !nilp(*tail(tail(&args)))) throw LispException("builtin divide: wrong number of args");
  Atom a = *head(&args);
  Atom b = *head(tail(&args));
  if(!std::holds_alternative<Integer>(a) || !std::holds_alternative<Integer>(b)) throw LispException("builtin divide: not a number");
  return makeInt(std::get<Integer>(a) / std::get<Integer>(b));
}

Atom lisp::builtinModulo(Atom args) {
  if(nilp(args) || nilp(*tail(&args)) || !nilp(*tail(tail(&args)))) throw LispException("builtin modulo: wrong number of args");
  Atom a = *head(&args);
  Atom b = *head(tail(&args));
  if(!std::holds_alternative<Integer>(a) || !std::holds_alternative<Integer>(b)) throw LispException("builtin modulo: not a number");
  return makeInt(std::get<Integer>(a) % std::get<Integer>(b));
}

Atom lisp::evaluateExpression(Atom expr, Atom env) {
  Atom op;
  Atom args;
  Atom p;

  if(std::holds_alternative<Symbol>(expr)) {
    return getEnv(env, expr);
  } else if(!std::holds_alternative<Pair*>(expr)) {
    return expr;
  }

  if(!listp(expr)) throw LispException("evaluateExpression: Improper list");

  op = *head(&expr);
  args = *tail(&expr);

  if(std::holds_alternative<Symbol>(op)) {
    if(std::get<Symbol>(op) == "quote") {
      if(nilp(args) || !nilp(*tail(&args))) {
        throw LispException("evaluateExpression: Argument error");
      }
      return *head(&args);
    } else if(std::get<Symbol>(op) == "def") {
      Atom sym, val;
      if(nilp(args) || nilp(*tail(&args))
      || !nilp(*tail(tail(&args)))) {
        throw LispException("evaluateExpression: Argument error");
      }
      sym = *head(&args);
      if(!std::holds_alternative<Symbol>(sym)) throw LispException("evaluateExpression: Type error");

      val = evaluateExpression(*head(tail(&args)), env);
      setEnv(env, sym, val);
      return sym;
    }
  }
  // Evaluate operator
  op = evaluateExpression(op, env);
  // Evaluate arguments
  args = copyList(args);
  p = args;
  while(!nilp(p)) {
    *head(&p) = evaluateExpression(*head(&p), env);
    p = *tail(&p);
  }
  return apply(op, args);

  throw LispException("evaluateExpression: Illformed syntax");
}
