#ifndef VEC_HPP
#define VEC_HPP

template <typename T>
struct Vec {
	T x;
	T y;

	explicit Vec(T x, T y) : x(x), y(y) {;}
};

#endif /* VEC_HPP */
