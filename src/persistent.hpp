class Persistent {
public:
	//virtual void save(TCODZip& zip) = 0;
	//virtual void load(TCODZip& zip) = 0;
};

// Non-intrusive serializations for TCOD classes
template<class Archive>
inline void serialize (
    Archive & ar,
    TCODColor & t,
    const unsigned int file_version
) {
    ar & t.r;
	ar & t.g;
	ar & t.b;
}
