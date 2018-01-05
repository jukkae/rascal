#ifndef IGNORE_HPP
#define IGNORE_HPP

//Use to silence compiler warnings and be explicit about not using it
template <typename T>
void ignore(T &&)
{ }

#endif /* IGNORE_HPP */
