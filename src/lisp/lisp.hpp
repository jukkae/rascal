#ifndef LISP_HPP
#define LISP_HPP
#include <variant>
#include <exception>

namespace lisp {

enum class AtomType { NIL, PAIR, SYMBOL, INTEGER };

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
using Atom = std::variant<Nil, Pair*, std::string, long>;

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

std::list<std::string> tokenize(std::string const str);

bool isInteger(std::string const s);
bool isNil(std::string const s);
bool isSymbol(std::string const s);
Atom parseSimple(std::string const str);
Atom readFrom(std::list<std::string> tokens);
Atom readExpression(std::string const expression);

Atom createEnv(Atom parent);
Atom getEnv(Atom env, Atom symbol);
void setEnv(Atom env, Atom symbol, Atom value);

// TODO this could use e.g. std::set instead, but this is good enough for now
extern Atom symbolTable;

} // namespace lisp
#endif /* LISP_HPP */
