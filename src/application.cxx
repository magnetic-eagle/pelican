#include <filesystem>

#include <QDir>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>

#include <argparse/argparse.hpp>

#include <easyqt/logging.hxx>
#include <easyqt/objectregistry.hxx>
#include <easyqt/settingsdialog.hxx>

#include "application.hxx"
#include "commands.hxx"
#include "config.hxx"
#include "mainwindow.hxx"
#include "mediaview.hxx"
#include "thumbnailmanager.hxx"

namespace pelican {
	Application::Application(int& argc, char **argv):
		easyqt::Application(argc, argv), arguments("pelican")
	{
		setDesktopFileName("pelican");

		QStringList paths = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
		std::filesystem::path sourceDataPath = std::filesystem::path(SOURCES_PATH) / "data";
		paths.append(sourceDataPath.c_str());
		QDir::setSearchPaths("res", paths);
		
		arguments.add_argument("--log-level")
			.help("Minimum log level (messages with a level lower than this will not be displayed)")
			.default_value("warning");
		
		arguments.parse_args(argc, argv);
		
		easyqt::ObjectRegistry::get<easyqt::Logger>()->setLogLevel(
			arguments.get<std::string>("--log-level")
		);
		LOG(INFO, "Log level: " << easyqt::ObjectRegistry::get<easyqt::Logger>()->logLevel());
		
		addCommands();

		easyqt::ObjectRegistry::get<easyqt::SettingsDialog>()->setFactorySettingsPath("res:/settings.xml");
		_mediaDirectoryPath = easyqt::ObjectRegistry::get<easyqt::SettingsDialog>()->settingValue<std::filesystem::path>("media-storage-directory");
	}

	Application::~Application() {
		easyqt::ObjectRegistry::get<ThumbnailManager>()->shutdown();
	}

	bool Application::initUI() {
		LOG(DEBUG, "Initializing Pelican application")

		StartupResult res = startupCheck();
		switch (res) {
			case StartupResult::SettingsAndQuit:
				easyqt::ObjectRegistry::get<easyqt::SettingsDialog>()->exec();
				return false;
			case StartupResult::Quit:
				return false;
			case StartupResult::Continue:
				break;
		}

		auto w = easyqt::ObjectRegistry::get<MainWindow>();
		w->showMaximized();
		
		_mediaDirectoryWatcher.addPath(_mediaDirectoryPath.c_str());
		QObject::connect(&_mediaDirectoryWatcher, &QFileSystemWatcher::directoryChanged, this, &Application::mediaDirectoryChanged);
		rescanMedias();

		return true;
	}
	
	void Application::mediaDirectoryChanged(const QDir& path) {
		rescanMedias();
	}

	Application::StartupResult Application::startupCheck() {
		if (_mediaDirectoryPath.empty() or !std::filesystem::is_directory(_mediaDirectoryPath)) {
			LOG(
				ERROR, "Media storage directory is empty, not defined or does not exist. "
				"Please select an existing directory in the settings and restart the application"
			);

			QMessageBox msgBox;
			msgBox.setWindowFlags(msgBox.windowFlags() & ~Qt::WindowCloseButtonHint);
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.setWindowTitle("Unusable media storage directory");
			msgBox.setText(
				"Media storage directory is empty, not defined or does not exist.\n"
				"Please select an existing directory in the settings and restart the application"
			);
			QPushButton* quitButton = msgBox.addButton("Quit", QMessageBox::RejectRole);
			QPushButton* settingsButton = msgBox.addButton("Open Settings ...", QMessageBox::ActionRole);
			msgBox.exec();

			if (msgBox.clickedButton() == settingsButton) {
				return StartupResult::SettingsAndQuit;
			}
			return StartupResult::Quit;
		}

		return StartupResult::Continue;
	}
	
	void Application::rescanMedias() {
		_medias.clear();

		for (const auto& entry: std::filesystem::directory_iterator(_mediaDirectoryPath)) {
			if (!(entry.is_regular_file() || entry.is_symlink())) {
				continue;
			}
			std::filesystem::path stem = entry.path().stem();
			std::filesystem::path ext = entry.path().extension();
			if (_medias.contains(stem)) {
				_medias[stem]->addSuffix(ext);
			} else {
				_medias[stem] = std::make_shared<Media>(_mediaDirectoryPath, stem.string());
				_medias[stem]->addSuffix(ext);
			}
		}
		
		easyqt::ObjectRegistry::get<MediaView>()->rebuild();
	}
	
	MediaPtrVector Application::medias() {
		MediaPtrVector medias;
		for (const auto& [_, media]: _medias) {
			medias.push_back(media);
		}
		return medias;
	}
}

