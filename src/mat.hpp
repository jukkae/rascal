#ifndef MAT_HPP
#define MAT_HPP

namespace boost { namespace serialization { class access; } }

template<class T>
class Mat2d {
public:
	using data_type       = std::vector<T>;
	using value_type      = typename std::vector<T>::value_type;
	using size_type       = typename std::vector<T>::size_type;
	using reference       = typename std::vector<T>::reference;
	using const_reference = typename std::vector<T>::const_reference;
	using iterator        = typename std::vector<T>::iterator;
	using const_iterator  = typename std::vector<T>::const_iterator;

	Mat2d() = default;
	Mat2d(size_type width, size_type height): w(width), h(height), contents(width * height) {;}

	iterator begin() { return contents.begin(); }
	iterator end() { return contents.end(); }
	const_iterator begin() const { return contents.begin(); }
	const_iterator end() const { return contents.end(); }

	reference operator() (size_type const x, size_type const y) {
		return contents[x + y*w];
	}
	const_reference operator() (size_type const x, size_type const y) const {
		return contents[x + y*w];
	}
	reference at(size_type const x, size_type const y) {
		return contents.at(x + y*w);
	}
	const_reference at(size_type const x, size_type const y) const {
		return contents.at(x + y*w);
	}

	int w;
	int h;
	std::vector<T> contents; //sb private

private:
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & w;
		ar & h;
		ar & contents;
	}

};

#endif /* MAT_HPP */
