#ifndef LISP_HPP
#define LISP_HPP
#include <variant>
#include <exception>

namespace lisp {

enum class TokenType {
  NIL, SYMBOL, INTEGER, LPAREN, RPAREN, PERIOD, QUOTE, QUASIQUOTE, UNQUOTE, UNQUOTE_SPLICING, UNKNOWN
};

struct LispException : public std::exception {
private:
  std::string m_message;
public:
  explicit LispException(const std::string& message): m_message(message) {}
  virtual const char* what() const throw()
  {
    return m_message.c_str();
  }
};

struct Pair;
struct Closure;
struct Macro;
struct Nil { };
using Symbol = std::string;
using Integer = long;
struct Builtin;

// Atom should be called Cell, true Atoms are Nil, Symbol, Integer
// Are _pointers_ to Pair necessary, since everything is allocated via cons?
using Atom = std::variant<Nil, Pair*, Symbol, Integer, Builtin, Closure*, Macro*>;

struct Builtin {
  std::add_pointer_t<Atom(Atom args)> func;
  Atom operator()(Atom args) { return func(args); }
};

struct Pair {
  Atom head;
  Atom tail;
};

struct Closure : public Pair {
  using Pair::Pair;
  Closure(Atom head, Atom tail) : Pair { head, tail } {}
  Closure(Pair* pair) : Pair { pair->head, pair->tail } {}
};

struct Macro : public Pair {
  using Pair::Pair;
  Macro(Atom head, Atom tail) : Pair { head, tail } {}
  Macro(Pair* pair) : Pair { pair->head, pair->tail } {}
};

bool nilp(Atom atom);
bool listp(Atom expr);
Atom cons(Atom head, Atom tail);
Atom* head(Atom* atom);
Atom* tail(Atom* atom);

// TODO this can be templated
Atom makeNil();
Atom makeInt(long x);
Atom makeSymbol(std::string const s);
Atom makeBuiltin(Builtin func);
Atom makeClosure(Atom env, Atom args, Atom body);

Atom copyList(Atom list);
Atom apply(Atom func, Atom args);

void printExpr(Atom atom);

std::list<std::string> tokenize(std::string const str);

bool isInteger(std::string const s);
bool isNil(std::string const s);
bool isSymbol(std::string const s);
TokenType getTokenType(std::string token);
Atom readList(std::list<std::string>& tokens);
Atom readFrom(std::list<std::string>& tokens);
Atom readExpression(std::string const expression);
std::list<Atom> readExpressions(std::string const expressions);

Atom createEnv(Atom parent);
Atom getEnv(Atom env, Atom symbol);
void setEnv(Atom env, Atom symbol, Atom value);

Atom builtinHead(Atom args);
Atom builtinTail(Atom args);
Atom builtinCons(Atom args);
Atom builtinAdd(Atom args);
Atom builtinSubtract(Atom args);
Atom builtinMultiply(Atom args);
Atom builtinDivide(Atom args);
Atom builtinModulo(Atom args); // TODO should integer divide return the quotient as well as modulo?
Atom builtinNumEq(Atom args);
Atom builtinNumLess(Atom args);
Atom builtinApply(Atom args);
Atom builtinEq(Atom args);
Atom builtinPair(Atom args);

// Continuations and tail recursion-related stuff
Atom getFromList(Atom list, int k);
void setToList(Atom list, int k, Atom value);
void reverseList(Atom* list); // TODO reversing in-place is dirty, nasty and dirty
Atom makeFrame(Atom parent, Atom env, Atom tailAtom);


Atom evaluateExpression(Atom expr, Atom env);

std::string readFile(std::string path);
void loadFile(Atom env, std::string path);

// TODO this could use e.g. std::set instead, but this is good enough for now
extern Atom symbolTable;

} // namespace lisp
#endif /* LISP_HPP */
