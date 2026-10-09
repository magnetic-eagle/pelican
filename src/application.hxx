#ifndef PELICAN_APPLICATION_HXX
#define PELICAN_APPLICATION_HXX

#include <filesystem>
#include <map>

#include <QFileSystemWatcher>
#include <QDir>

#include <easyqt/application.hxx>

#include <argparse/argparse.hpp>

#include "media.hxx"

namespace pelican {
	class Application: public easyqt::Application {
		Q_OBJECT
		public:
			enum class StartupResult {
				Continue,
				SettingsAndQuit,
				Quit,
			};

			Application(int& argc, char **argv);
			~Application();
			bool initUI();
			static Application* instance() {
				return qobject_cast<Application*>(easyqt::Application::instance());
			}
			
			StartupResult startupCheck();
			void rescanMedias();
			MediaPtrVector medias();
			
			argparse::ArgumentParser arguments;
			
		public slots:
			void mediaDirectoryChanged(const QDir& path);
		
		private:
			QFileSystemWatcher _mediaDirectoryWatcher;
			std::filesystem::path _mediaDirectoryPath;
			std::map<std::string, MediaPtr> _medias;
	};
}

#endif

