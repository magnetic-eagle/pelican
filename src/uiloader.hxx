#ifndef PELICAN_UILOADER_HXX
#define PELICAn_UILOADER_HXX

#include <QFile>

#include <easyqt/uiloader.hxx>

namespace pelican {
	class UiLoader: public easyqt::UiLoader {
		public:
			QWidget* createWidget(const QString &widgetName, QWidget* parentWidget, const QString &name);
	};


	template<typename T>
	T* loadWidgetFromFile(const char* path) {
		UiLoader loader;
		QFile file(path);
		file.open(QFile::ReadOnly);
		T* w = qobject_cast<T*>(loader.load(&file));
		file.close();
		
		return w;
	}
}

#endif

