#ifndef PELICAN_MAINWINDOW_HXX
#define PELICAN_MAINWINDOW_HXX

#include <QMainWindow>

#include <easyqt/singleton.hxx>

namespace pelican {
	class MainWindow: public QMainWindow, public easyqt::NamedSingleton<MainWindow> {
		Q_OBJECT
		public:
			MainWindow();
			~MainWindow();
			void initUI();
		
		private:
			QWidget* mainWidget;
	};
}

#endif

