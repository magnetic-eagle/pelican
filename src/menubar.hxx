#ifndef PELICAN_MENUBAR_HXX
#define PELICAN_MENUBAR_HXX

#include <easyqt/logging.hxx>
#include <easyqt/menubar.hxx>
#include <easyqt/singleton.hxx>

namespace pelican {
	class MenuBar: public easyqt::MenuBar, public easyqt::NamedSingleton<MenuBar>{
		Q_OBJECT
		protected:
			virtual void initImpl() override;
	};
}

#endif
