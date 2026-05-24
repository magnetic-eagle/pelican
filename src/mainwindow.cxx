#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QSplitter>
#include <QVBoxLayout>

#include "mainwindow.hxx"
#include "uiloader.hxx"

namespace pelican {
	MainWindow::MainWindow() {
	}
	
	MainWindow::~MainWindow() {
		delete mainWidget;
	}
	
	void MainWindow::initUI() {
		setWindowTitle("Pelican");
		loadWidgetFromFile<MainWindow>("res:ui/mainwindow.ui");
		QSplitter* mainSplitter = findChild<QSplitter*>("MainSplitter");
		if (!mainSplitter) {
			LOG(ERROR, "Could not find widget 'mainSplitter'");
		} else {
			mainSplitter->setCollapsible(0, false);
			mainSplitter->setCollapsible(2, false);
		
			mainSplitter->setStretchFactor(1, 1);
		}
		//mainWidget = new QWidget();
		//QVBoxLayout mainLayout;
		//mainWidget->setLayout(&mainLayout);
		//setCentralWidget(mainWidget);
		//statusBar();
	}
}

