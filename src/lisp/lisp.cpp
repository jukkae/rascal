#include "lisp.hpp"
#include <cctype>
#include <regex>
using namespace lisp;

Atom lisp::symbolTable {Nil{}};
Pair* globalAllocations = nullptr;

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

void* Pair::operator new(size_t size) {
  void* p;
  p = ::operator new(size);
  if(!p)
  {
      throw LispException("Bad allocation");
  }
  ((Pair*)p)->mark = false;
  ((Pair*)p)->next = globalAllocations;
  globalAllocations = (Pair*)p;
  return p;
}

void Pair::operator delete(void* p) {
  ::operator delete(p);
}

Atom lisp::cons(Atom head, Atom tail) {
  Pair* p = new Pair { head, tail };
  return Atom { p };
}

Atom* lisp::head(Atom* atom) {
  if(std::holds_alternative<Pair*>(*atom)) {
    return &(std::get<Pair*>(*atom)->head);
  } else if(std::holds_alternative<Closure*>(*atom)) {
    return &(std::get<Closure*>(*atom)->head);
  } else if(std::holds_alternative<Macro*>(*atom)) {
    return &(std::get<Macro*>(*atom)->head);
  } else {
    throw LispException("Head: expression does not refer to a pair");
  }
}

Atom* lisp::tail(Atom* atom) {
  if(std::holds_alternative<Pair*>(*atom)) {
    return &(std::get<Pair*>(*atom)->tail);
  } else if(std::holds_alternative<Closure*>(*atom)) {
    return &(std::get<Closure*>(*atom)->tail);
  } else if(std::holds_alternative<Macro*>(*atom)) {
    return &(std::get<Macro*>(*atom)->tail);
  } else {
    throw LispException("Tail: expression does not refer to a pair");
  }
}

void lisp::gc_mark(Atom root) {
  Pair* a;
  if(std::holds_alternative<Pair*>(root)){
    a = std::get<Pair*>(root);
  } else if(std::holds_alternative<Closure*>(root)) {
    a = static_cast<Pair*>(std::get<Closure*>(root));
  } else if(std::holds_alternative<Macro*>(root)) {
    a = static_cast<Pair*>(std::get<Macro*>(root));
  } else { return; }
  if(a->mark) { return; }
  a->mark = true;
  if(std::holds_alternative<Pair*>(root)) {
    Atom at = Atom { std::get<Pair*>(root) };
    gc_mark(*head(&at));
    gc_mark(*tail(&at));
  } else if(std::holds_alternative<Closure*>(root)) {
    Atom at = Atom { std::get<Closure*>(root) };
    gc_mark(*head(&at));
    gc_mark(*tail(&at));
  } else if(std::holds_alternative<Macro*>(root)) {
    Atom at = Atom { std::get<Macro*>(root) };
    gc_mark(*head(&at));
    gc_mark(*tail(&at));
  }
}

void lisp::gc() {
  Pair* a;
  Pair** p;

  p = &globalAllocations;
  while(*p != nullptr) {
    a = *p;
    if(!a->mark) {
      *p = a->next;
      delete a;
    } else {
      p = &a->next;
    }
  }

  a = globalAllocations;
  while (a != nullptr) {
    a->mark = false;
    a = a->next;
  }
}

void lisp::gc_run(Atom expr, Atom result, Atom env) {

  gc_mark(lisp::symbolTable);
  gc_mark(expr);
  gc_mark(result);
  gc_mark(env);

  gc();
}

Atom lisp::makeNil() {
  return Atom { Nil{} };
}

Atom lisp::makeInt(long x) {
  return Atom { x };
}

Atom lisp::makeSymbol(std::string const s) {
  Atom a, p;
  p = lisp::symbolTable;
  while(!nilp(p)) {
    a = *head(&p);
    if(std::get<std::string>(a) == s) {
      return a;
    }
    p = *tail(&p);
  }
  a = Atom{s};
  lisp::symbolTable = cons(a, lisp::symbolTable);
  return a;
}

Atom lisp::makeBuiltin(Builtin func) {
  return Atom { func };
}

Atom lisp::makeClosure(Atom env, Atom args, Atom body) {
  if(!listp(body)) throw LispException("makeClosure: body not list");

  Atom p = args;
  while(!nilp(p)) {
    if (std::holds_alternative<Symbol>(p)) break;
    else if(!std::holds_alternative<Pair*>(p) || !std::holds_alternative<Symbol>(*head(&p))) throw LispException("makeClosure: type error in args");
    p = *tail(&p);
  }
  // TODO check this
  return Atom { static_cast<Closure*>(std::get<Pair*>(cons(env, cons(args, body)))) };
}

// Make shallow copy
// Do I actually need this?
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
  else if(!std::holds_alternative<Closure*>(func)) throw LispException("Apply: Tried to apply non-function or non-lambda");

  Atom env = createEnv(*head(&func));
  Atom argNames = *head(tail(&func));
  Atom body = *tail(tail(&func));
  Atom result;

  // Bind arguments
  while(!nilp(argNames)) {
    if(std::holds_alternative<Symbol>(argNames)) {
      setEnv(env, argNames, args);
      args = makeNil();
      break;
    }

    if(nilp(args)) throw LispException("Apply: Argument error");
    setEnv(env, *head(&argNames), *head(&args));
    argNames = *tail(&argNames);
    args = *tail(&args);
  }
  if(!nilp(args)) throw LispException("Apply: Argument error");

  // Evaluate body
  while(!nilp(body)) {
    result = evaluateExpression(*head(&body), env);
    body = *tail(&body);
  }
  return result;
}

void lisp::printExpr(Atom atom) {
  if(std::holds_alternative<Nil>(atom)) {
    std::cout << "NIL";
  }
  else if(std::holds_alternative<Pair*>(atom) || std::holds_alternative<Closure*>(atom) || std::holds_alternative<Macro*>(atom)) {
    std::cout << "(";
    printExpr(*head(&atom));
    Atom currentAtom = *tail(&atom);
    while(!nilp(currentAtom)) {
      if(std::holds_alternative<Pair*>(currentAtom) || std::holds_alternative<Closure*>(currentAtom) || std::holds_alternative<Macro*>(currentAtom)) {
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
  else if(std::holds_alternative<Symbol>(atom)) {
    std::cout << std::get<Symbol>(atom);
  }
  else if(std::holds_alternative<Integer>(atom)) {
    std::cout << std::get<Integer>(atom);
  }
  else if(std::holds_alternative<Builtin>(atom)) {
    std::cout << "<BUILTIN>: " << &std::get<Builtin>(atom);
  }
  else {
    std::cout << "expr-type: " << atom.index();
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
    } else if(str[i] == '\'') {
      result.push_back("\'");
    } else if(str[i] == '`') {
      result.push_back("`");
    } else if(str[i] == ',') {
      if(str[i+1] == '@') { // TODO bad form, this will blow up
        result.push_back(",@");
        ++i;
      }
      else {
        result.push_back(",");
      }
    } else if(std::isspace(str[i])) {
      // do nothing
    } else {
      std::string token;
      while(i < str.length()
      && str[i] != '('
      && str[i] != ')'
      && str[i] != '\'' // TODO this is a shit implementation, should be defined as prefix characters
      && str[i] != '`' // TODO this is a shit implementation, should be defined as prefix characters
      && str[i] != '@' // Does @ count as a prefix?
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

// TODO this is shit
bool lisp::isSymbol(std::string const s) {
  if(s.length() > 1) {
    if(s[0] != ',') return true;
    else return false;
  } else if(s.length() == 1) {
    if(s[0] == ')' || s[0] == '(' || s[0] == '\'' || s[0] == '`' || s[0] == ',') {
      return false;
    }
    else return true;
  }
  else throw LispException("tokenize: Parsing token with length 0");
}

TokenType lisp::getTokenType(std::string token) {
  if(isNil(token)) return TokenType::NIL;
  else if(isInteger(token)) return TokenType::INTEGER;
  else if(isSymbol(token)) return TokenType::SYMBOL;
  else if(token == "(") return TokenType::LPAREN;
  else if(token == ")") return TokenType::RPAREN;
  else if(token == ".") return TokenType::PERIOD;
  else if(token == "\'") return TokenType::QUOTE;
  else if(token == "`") return TokenType::QUASIQUOTE;
  else if(token == ",@") return TokenType::UNQUOTE_SPLICING;
  else if(token == ",") return TokenType::UNQUOTE;
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
  } else if (tokenType == TokenType::QUOTE) {
    Atom result = cons(makeSymbol("quote"), cons(makeNil(), makeNil()));
    *head(tail(&result)) = readFrom(tokens);
    return result;
  } else if (tokenType == TokenType::QUASIQUOTE) {
    Atom result = cons(makeSymbol("quasiquote"), cons(makeNil(), makeNil()));
    *head(tail(&result)) = readFrom(tokens);
    return result;
  } else if (tokenType == TokenType::UNQUOTE) {
    Atom result = cons(makeSymbol("unquote"), cons(makeNil(), makeNil()));
    *head(tail(&result)) = readFrom(tokens);
    return result;
  } else if (tokenType == TokenType::UNQUOTE_SPLICING) {
    Atom result = cons(makeSymbol("unquote-splicing"), cons(makeNil(), makeNil()));
    *head(tail(&result)) = readFrom(tokens);
    return result;
  }
  else {
    throw LispException("readFrom: couldn't match token");
  }
}

Atom lisp::readExpression(std::string const expression) {
  std::list<std::string> tokens = tokenize(expression);
  return readFrom(tokens);
}

std::list<Atom> lisp::readExpressions(std::string const expressions) {
  std::list<std::string> tokens = tokenize(expressions);
  std::list<Atom> results;
  while(!tokens.empty()) {
    Atom result = readFrom(tokens);
    results.push_back(result);
  }
  return results;
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
  if(nilp(parent)) {
    std::cout << "symbol unbound:\n";
    printExpr(symbol);
    std::cout << "\n";
    throw LispException("getEnv: Symbol unbound");
  }
  return getEnv(parent, symbol);
}

void lisp::setEnv(Atom env, Atom symbol, Atom value) {
  if(!std::holds_alternative<Pair*>(env)) throw LispException("setEnv: Env not pair");
  if(!std::holds_alternative<Symbol>(symbol)) throw LispException("setEnv: Symbol not symbol");

  Atom bs = *tail(&env);
  Atom b = makeNil();

  while(!nilp(bs)) {
    b = *head(&bs);
    if(!std::holds_alternative<Pair*>(b)) throw LispException("setEnv: b not pair");
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

Atom lisp::builtinNumEq(Atom args) {
  if(nilp(args) || nilp(*tail(&args)) || !nilp(*tail(tail(&args)))) throw LispException("builtin numeq: wrong number of args");
  Atom a = *head(&args);
  Atom b = *head(tail(&args));
  if(!std::holds_alternative<Integer>(a) || !std::holds_alternative<Integer>(b)) throw LispException("builtin numeq: not a number");
  bool result = std::get<Integer>(a) == std::get<Integer>(b);
  return result ? makeSymbol("t") : makeNil();
}

Atom lisp::builtinNumLess(Atom args) {
  if(nilp(args) || nilp(*tail(&args)) || !nilp(*tail(tail(&args)))) throw LispException("builtin numless: wrong number of args");
  Atom a = *head(&args);
  Atom b = *head(tail(&args));
  if(!std::holds_alternative<Integer>(a) || !std::holds_alternative<Integer>(b)) throw LispException("builtin numless: not a number");
  bool result = std::get<Integer>(a) < std::get<Integer>(b);
  return result ? makeSymbol("t") : makeNil();
}

Atom lisp::builtinApply(Atom args) {
  if(nilp(args) || nilp(*tail(&args)) || !nilp(*tail(tail(&args)))) throw LispException("builtin apply: wrong number of args");
  Atom fn = *head(&args);
  args = *head(tail(&args));
  if(!listp(args)) throw LispException("builtin apply: syntax error");
  return apply(fn, args);
}

Atom lisp::builtinEq(Atom args) {
  if(nilp(args) || nilp(*tail(&args)) || !nilp(*tail(tail(&args)))) throw LispException("builtin eq: wrong number of args");

  Atom a = *head(&args);
  Atom b = *head(tail(&args));
  bool result = false;
  if(a.index() == b.index()) {
    if(std::holds_alternative<Nil>(a)) {
      return makeSymbol("t");
    } else if(std::holds_alternative<Pair*>(a)) {
      if(std::get<Pair*>(a) == std::get<Pair*>(b)) result = true;
      else result = false;;
    } else if(std::holds_alternative<Symbol>(a)) {
      if(std::get<Symbol>(a) == std::get<Symbol>(b)) result = true;
      else result = false;;
    } else if(std::holds_alternative<Integer>(a)) {
      if(std::get<Integer>(a) == std::get<Integer>(b)) result = true;
      else result = false;;
    } else if(std::holds_alternative<Builtin>(a)) {
      if(std::get<Builtin>(a).func == std::get<Builtin>(b).func) result = true;
      else result = false;;
    } else if(std::holds_alternative<Closure*>(a)) {
      if(std::get<Closure*>(a) == std::get<Closure*>(b)) result = true;
      else result = false;;
    } else if(std::holds_alternative<Macro*>(a)) {
      if(std::get<Macro*>(a) == std::get<Macro*>(b)) result = true;
      else result = false;;
    } else {
       throw LispException("builtin eq: comparison for type not implemented");
    }
  } else {
    result = false;
  }
  return result ? makeSymbol("t") : makeNil();
}

Atom lisp::builtinPair(Atom args) {
  if(nilp(args) || !nilp(*tail(&args))) throw LispException("builtin pair: wrong number of args");
  bool result = std::holds_alternative<Pair*>(*head(&args));
  return result ? makeSymbol("t") : makeNil();
}

Atom lisp::getFromList(Atom list, int k) {
  while(k-- > 0) {
    list = *tail(&list);
  }
  return *head(&list);
}

void lisp::setToList(Atom list, int k, Atom value) {
  while(k-- > 0) {
    list = *tail(&list);
  }
  *head(&list) = value;
}

void lisp::reverseList(Atom* list) {
  Atom tailAtom = makeNil();
  while(!nilp(*list)) {
    Atom p = *tail(list);
    *tail(list) = tailAtom;
    tailAtom = *list;
    *list = p;
  }
  *list = tailAtom;
}

Atom lisp::makeFrame(Atom parent, Atom env, Atom tailAtom) {
  return cons(parent,
           cons(env,
           cons(makeNil(), /* OP */
           cons(tailAtom,
           cons(makeNil(), /* args */
           cons(makeNil(), /* body */
           makeNil()))))));
}

void lisp::evalDoExec(Atom* stack, Atom* expr, Atom* env) {
  Atom body;
  *env = getFromList(*stack, 1);
  body = getFromList(*stack, 5);
  *expr = *head(&body);
  body = *tail(&body);
  if(nilp(body)) {
    // Finished function, pop stack
    *stack = *head(stack);
  } else {
    setToList(*stack, 5, body);
  }
}

// Bind function args to new env if not bound already, then call evalDoExec
void lisp::evalDoBind(Atom* stack, Atom* expr, Atom* env) {
  Atom op, args, argNames, body;
  body = getFromList(*stack, 5);
  if(!nilp(body)) {
    evalDoExec(stack, expr, env);
  } else {
    op = getFromList(*stack, 2);
    args = getFromList(*stack, 4);
    *env = createEnv(*head(&op));
    argNames = *head(tail(&op));
    body = *tail(tail(&op));
    setToList(*stack, 1, *env);
    setToList(*stack, 5, body);
    // Bind args
    while(!nilp(argNames)) {
      if(std::holds_alternative<Symbol>(argNames)) {
        setEnv(*env, argNames, *head(&args));
        args = makeNil();
        break;
      }
      if(nilp(args)) throw LispException("evalDoBind: Arguments error");
      setEnv(*env, *head(&argNames), *head(&args));
      argNames = *tail(&argNames);
      args = *tail(&args);
    }
    if(!nilp(args)) throw LispException("evalDoBind: Arguments error");
    setToList(*stack, 4, makeNil());
    evalDoExec(stack, expr, env);
  }
  return;
}

// Either generate expression to call a builtin or call evalDoBind
// Called once all args have been evaluated
void lisp::evalDoApply(Atom* stack, Atom* expr, Atom* env) {
  Atom op, args;
  op = getFromList(*stack, 2);
  args = getFromList(*stack, 4);
  if(!nilp(args)) {
    reverseList(&args);
    setToList(*stack, 4, args);
  }

  if(std::holds_alternative<Symbol>(op)) {
    if(std::get<Symbol>(op) == "apply") {
      // Replace current stack frame
      *stack = *head(stack);
      *stack = makeFrame(*stack, *env, makeNil());
      op = *head(&args);
      args = *head(tail(&args));
      if(!listp(args)) {
        throw LispException("evalDoApply: Syntax error");
      }
      setToList(*stack, 2, op);
      setToList(*stack, 4, args);
    }
  }

  if(std::holds_alternative<Builtin>(op)) {
    *stack = *head(stack);
    *expr = cons(op, args);
    return;
  } else if(!std::holds_alternative<Closure*>(op)) {
    throw LispException("evalDoApply: Type error");
  }
  evalDoBind(stack, expr, env);
  return;
}

// TODO figure out the whole Atom* temp business
Atom lisp::evalDoReturn(Atom* stack, Atom* expr, Atom* env, Atom* temp) {
  Atom op, args, body, result;
  *env = getFromList(*stack, 1);
  op = getFromList(*stack, 2);
  body = getFromList(*stack, 5);

  if(!nilp(body)) {
    // Still running a procedure
    evalDoApply(stack, expr, env);
    return *temp;
  }

  if(nilp(op)) {
    // Finished evaluating operator
    op = *temp;
    setToList(*stack, 2, op);
    if(std::holds_alternative<Macro*>(op)) {
      // Don't evaluate macro args
      args = getFromList(*stack, 3);
      *stack = makeFrame(*stack, *env, makeNil());
      op = Atom { static_cast<Closure*>(static_cast<Pair*>(std::get<Macro*>(op))) };
      setToList(*stack, 2, op);
      setToList(*stack, 4, args);
      evalDoBind(stack, expr, env);
      return *temp;
    }
  } else if (std::holds_alternative<Symbol>(op)) {
    // Finished working on special form
    if(std::get<Symbol>(op) == "def") {
      Atom sym = getFromList(*stack, 4);
      setEnv(*env, sym, *temp);
      *stack = *head(stack);
      *expr = cons(makeSymbol("quote"), cons(sym, makeNil()));
      return *temp;
    } else if(std::get<Symbol>(op) == "if") {
      args = getFromList(*stack, 3);
      *expr = nilp(*temp) ? *head(tail(&args)) : *head(&args);
      *stack = *head(stack);
      return *temp;
    } else if(std::get<Symbol>(op) == "and") {
      // TODO implement
      throw LispException("evalDoReturn: and special form not implemented");
    } else {
      goto storeArg;
    }
  } else if (std::holds_alternative<Macro*>(op)) {
    // Finished evaluating macro
    *expr = *temp;
    *stack = *head(stack);
    return *temp;
  } else {
    storeArg:
    // Store evaluated arg
    args = getFromList(*stack, 4);
    setToList(*stack, 4, cons(*temp, args));
  }

  args = getFromList(*stack, 3);
  if(nilp(args)) {
    // No more args to evaluate
    evalDoApply(stack, expr, env);
    return *temp;
  }
  // Eval next argument
  *expr = *head(&args);
  setToList(*stack, 3, *tail(&args));
  return *temp;
}


Atom lisp::evaluateExpression(Atom expr, Atom env) {

  Atom op;
  Atom args;
  Atom p;
  // std::cout << "expr:\n";
  // printExpr(expr);
  // std::cout << "\n";

  if(std::holds_alternative<Symbol>(expr)) {
    return getEnv(env, expr);
  } else if(!std::holds_alternative<Pair*>(expr)) {
    return expr;
  }

  if(!listp(expr)) throw LispException("evaluateExpression: Improper list");

  op = *head(&expr);
  args = *tail(&expr);

  if(std::holds_alternative<Symbol>(op)) {
    // Special forms
    if(std::get<Symbol>(op) == "quote") {
      if(nilp(args) || !nilp(*tail(&args))) {
        throw LispException("evaluateExpression: Argument error");
      }
      return *head(&args);
    } else if(std::get<Symbol>(op) == "defmacro") {
      if(nilp(args) || nilp(*tail(&args))) throw LispException("evaluateExpression: Argument error");
      if(!std::holds_alternative<Pair*>(*head(&args))) throw LispException("evaluateExpression: Syntax error");

      Atom name = *head(head(&args));
      if(!std::holds_alternative<Symbol>(name))throw LispException("evaluateExpression: Type error");

      Atom macro = Atom { static_cast<Macro*>(static_cast<Pair*>(std::get<Closure*>(makeClosure(env, *tail(head(&args)), *tail(&args))))) };
      setEnv(env, name, macro);
      return name;

    } else if(std::get<Symbol>(op) == "def") {
      Atom sym, val;
      if(nilp(args) || nilp(*tail(&args))) {
        throw LispException("evaluateExpression: Argument error");
      }
      sym = *head(&args);
      if(std::holds_alternative<Pair*>(sym)) {
        val = makeClosure(env, *tail(&sym), *tail(&args));
        sym = *head(&sym);
        if(!std::holds_alternative<Symbol>(sym)) throw LispException("evaluateExpression: Argument error");
      } else if(std::holds_alternative<Symbol>(sym)) {
        if(!nilp(*tail(tail(&args)))) throw LispException("evaluateExpression: Argument error");
        val = evaluateExpression(*head(tail(&args)), env);
      } else throw LispException("evaluateExpression: Type error");

      setEnv(env, sym, val);
      return sym;
    } else if(std::get<Symbol>(op) == "lambda") {
      if(nilp(args) || nilp(*tail(&args))) throw LispException("evaluateExpression: Argument error");
      return makeClosure(env, *head(&args), *tail(&args));
    } else if(std::get<Symbol>(op) == "if") {
      Atom cond, val;
      if(nilp(args) || nilp(*tail(&args)) || nilp(*tail(tail(&args))) || !nilp(*tail(tail(tail(&args))))) throw LispException("evaluateExpression: Argument error");
      cond = evaluateExpression(*head(&args), env);
      val = nilp(cond) ? *head(tail(tail(&args))) : *head(tail(&args));
      return evaluateExpression(val, env);
    } else if(std::get<Symbol>(op) == "and") { // Short-circuiting and
      Atom res = makeSymbol("t");
      while(!nilp(args)) {
        res = evaluateExpression(*head(&args), env);
        if(nilp(res)) return res;
        args = *tail(&args);
      }
      return res;
    }
  }
  // Evaluate operator
  op = evaluateExpression(op, env);
  if(std::holds_alternative<Macro*>(op)) {
    op = Atom { static_cast<Closure*>(static_cast<Pair*>(std::get<Macro*>(op))) };
    Atom expansion = apply(op, args);
    return evaluateExpression(expansion, env);
  }
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

std::string lisp::readFile(std::string path) {
  std::cout << "Reading file " << path << "\n";
	std::ifstream fin(path);
	std::stringstream buffer;
	buffer << fin.rdbuf();
	std::string text = buffer.str();
  std::cout << "Read file " << path << "\n";
  return text;
}

void lisp::loadFile(Atom env, std::string path) {
  std::cout << "Loading file " << path << "\n";
  std::string text = readFile(path);
  std::list<Atom> exprs = readExpressions(text);
  for(auto& expr : exprs) {
    Atom result = evaluateExpression(expr, env);
  }
  std::cout << "Loaded file " << path << "\n";
}
