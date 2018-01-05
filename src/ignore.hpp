#ifndef IGNORE_HPP
#define IGNORE_HPP

//Use to silence compiler warnings and be explicit about not using it
//My clang doesn't yet support [[maybe_unused]] attribute, should use that when available
template <typename T>
inline void ignore(T&&) { }

#endif /* IGNORE_HPP */
