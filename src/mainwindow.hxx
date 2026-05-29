#ifndef PELICAN_MAINWINDOW_HXX
#define PELICAN_MAINWINDOW_HXX

#include <QMainWindow>

#include <easyqt/object.hxx>

namespace pelican {
	class MainWindow: public easyqt::Object<QMainWindow> {
		Q_OBJECT
		public:
			MainWindow();
			~MainWindow();
			
		protected:
			void initImpl() override;
		
		private:
			QWidget* mainWidget;
	};
}

#endif

