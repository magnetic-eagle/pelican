#ifndef PELICAN_THUMBNAILMANAGER_HXX
#define PELICAN_THUMBNAILMANAGER_HXX

#include <atomic>
#include <filesystem>
#include <memory>

#include <QThreadPool>

#include <easyqt/object.hxx>

#include "media.hxx"

namespace pelican {
	constexpr int THUMBNAIL_PRIORITY_BUFFER  = 0;
	constexpr int THUMBNAIL_PRIORITY_VISIBLE = 100;

	class ThumbnailManager: public easyqt::Object<> {
		public:
			enum class ThumbnailPriority: int {
				BUFFER=0,
				VISIBLE=100,
			};

			ThumbnailManager();
			~ThumbnailManager();
			void shutdown();
			std::shared_ptr<std::atomic_bool> requestThumbnail(MediaPtr media, unsigned int size, std::function<void(QImage)> callback, ThumbnailPriority priority = ThumbnailPriority::BUFFER);
		
		private:
			class Job: public QRunnable {
				public:
					Job(MediaPtr media, std::filesystem::path thumbPath, unsigned int size, std::function<void(QImage)> callback, std::shared_ptr<std::atomic_bool> cancelled):
						_media(media), _thumbPath(thumbPath), _size(size), _callback(callback), _cancelled(cancelled)
					{}
				private:
					MediaPtr _media;
					std::filesystem::path _thumbPath;
					unsigned int _size;
					std::function<void(QImage)> _callback;
					std::shared_ptr<std::atomic_bool> _cancelled;

				void run() override;
			};
			
			std::filesystem::path _thumbnailDirectory;

			QThreadPool _threadPool;
	};
}

#endif

