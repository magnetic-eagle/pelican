#ifndef PELICAN_THUMBNAILMANAGER_HXX
#define PELICAN_THUMBNAILMANAGER_HXX

#include <filesystem>

#include <QThreadPool>

#include <easyqt/object.hxx>

#include "media.hxx"

namespace pelican {
	class ThumbnailManager: public easyqt::Object<> {
		public:
			ThumbnailManager();
			~ThumbnailManager();
			void shutdown();
			void requestThumbnail(MediaPtr media, unsigned int size, std::function<void(QImage)> callback);
		
		private:
			class Job: public QRunnable {
				public:
					Job(MediaPtr media, std::filesystem::path thumbPath, unsigned int size, std::function<void(QImage)> callback):
						_media(media), _thumbPath(thumbPath), _size(size), _callback(callback)
					{}
				private:
					MediaPtr _media;
					std::filesystem::path _thumbPath;
					unsigned int _size;
					std::function<void(QImage)> _callback;

				void run() override;
			};
			
			std::filesystem::path _thumbnailDirectory;

			QThreadPool _threadPool;
	};
}

#endif

