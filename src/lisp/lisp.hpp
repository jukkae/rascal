#ifndef LISP_HPP
#define LISP_HPP
#include <variant>
#include <exception>

namespace lisp {

enum class AtomType { NIL, PAIR, SYMBOL, INTEGER };

enum class TokenType { NIL, SYMBOL, INTEGER, LPAREN, RPAREN, PERIOD, UNKNOWN };

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
struct Nil { };
using Symbol = std::string;
using Integer = long;
struct Builtin;

// Atom should be called Cell, true Atoms are Nil, Symbol, Integer
using Atom = std::variant<Nil, Pair*, Symbol, Integer, Builtin>;
struct Builtin {
  std::add_pointer_t<Atom(Atom args)> func;
  Atom operator()(Atom args);
};

struct Pair {
  Atom head;
  Atom tail;
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

void printExpr(Atom atom);

std::list<std::string> tokenize(std::string const str);

bool isInteger(std::string const s);
bool isNil(std::string const s);
bool isSymbol(std::string const s);
TokenType getTokenType(std::string token);
Atom readList(std::list<std::string>& tokens);
Atom readFrom(std::list<std::string>& tokens);
Atom readExpression(std::string const expression);

Atom createEnv(Atom parent);
Atom getEnv(Atom env, Atom symbol);
void setEnv(Atom env, Atom symbol, Atom value);

Atom evaluateExpression(Atom expr, Atom env);

// TODO this could use e.g. std::set instead, but this is good enough for now
extern Atom symbolTable;

} // namespace lisp
#endif /* LISP_HPP */
