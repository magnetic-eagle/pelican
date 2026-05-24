#include <QFile>
#include <QSplitter>

#include <easyqt/logging.hxx>
#include <easyqt/objectregistry.hxx>

#include "mainwindow.hxx"
#include "mediaview.hxx"
#include "mediainfopane.hxx"
#include "mediashowarea.hxx"
#include "menubar.hxx"
#include "uiloader.hxx"

namespace pelican {
	QWidget* UiLoader::createWidget(const QString& widgetName, QWidget* parentWidget, const QString& name) {
		QWidget* w;
		LOG(INFO, "Saw widget name " << widgetName);
		if (widgetName == "pelican::MenuBar") {
			w = easyqt::ObjectRegistry::get<MenuBar>(name.toStdString());
		} else if (widgetName == "pelican::MainWindow") {
			w = easyqt::ObjectRegistry::get<MainWindow>();
		} else if (widgetName == "pelican::MediaView") {
			w = easyqt::ObjectRegistry::get<MediaView>();
		} else if (widgetName == "pelican::MediaInfoPane") {
			w = easyqt::ObjectRegistry::get<MediaInfoPane>();
		} else if (widgetName == "pelican::MediaShowArea") {
			w = easyqt::ObjectRegistry::get<MediaShowArea>();
			w->hide();
		} else {
			w = easyqt::UiLoader::createWidget(widgetName, parentWidget, name);
		}
		if (!w) {
			LOG(ERROR, "Unable to create widget of type '"<< widgetName.toStdString() << "' with name '" << name.toStdString() << "' !")
			return nullptr;
		}
		w->setObjectName(name);
		w->setParent(parentWidget);
		return w;
	}
}

