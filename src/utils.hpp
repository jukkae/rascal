#ifndef UTILS_HPP
#define UTILS_HPP
namespace utils {

template<class Container, class F>
auto erase_where(Container& c, F&& f) {
	return c.erase(std::remove_if(c.begin(), c.end(), std::forward<F>(f)), c.end());
}
}; // namespace utils
#endif /* UTILS_HPP */
