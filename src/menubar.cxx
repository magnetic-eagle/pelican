#include "menubar.hxx"

namespace pelican {
	void MenuBar::initImpl() {
		LOG(DEBUG, "Initializing pelican::MenuBar with name " << _name);
		std::string file;
		if (_name == "pelican::GlobalMenuBar") {
			file = "res:ui/menubar.xml";
		}
		if (!file.empty()) {
			loadFromFile(file);
		}
	}
}

