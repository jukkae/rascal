class Gui {
public:
	Gui();
	~Gui();

	void render();
	void message(const TCODColor &col, std::string text, ...);

protected:
	TCODConsole* con;
	struct Message {
		std::string text;
		TCODColor col;

		Message(std::string text, const TCODColor& col);
		Message(); // TODO dirty hack
		~Message();

		template<typename Archive>
		void serialize(Archive & ar, const unsigned int version) {
			ar & text;
			ar & col;
		}
	};
	std::vector<Message*> log;

	void renderBar(int x, int y, int width, std::string name, float value, float maxValue, const TCODColor& barColor, const TCODColor& backColor);
	void renderMouseLook();

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & log;
	}
};
