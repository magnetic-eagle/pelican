#include <QCryptographicHash>
#include <QImageReader>
#include <QStandardPaths>
#include <QString>
#include <QUrl>

#include <FreeImagePlus.h>

#include <easyqt/logging.hxx>
#include <easyqt/objectregistry.hxx>

#include "thumbnailmanager.hxx"
#include "mediaview.hxx"

namespace pelican {
	ThumbnailManager::ThumbnailManager() {
		_thumbnailDirectory = std::filesystem::path(QStandardPaths::writableLocation(QStandardPaths::CacheLocation).toStdString()) / "thumbnails";
		std::filesystem::create_directories(_thumbnailDirectory);
	}

	ThumbnailManager::~ThumbnailManager() {
		shutdown();
	}
	
	void ThumbnailManager::shutdown() {
		_threadPool.clear();
		_threadPool.waitForDone();
	}		
	
	void ThumbnailManager::requestThumbnail(MediaPtr media, unsigned int size, std::function<void(QImage)> callback) {
		// Get media path
		std::filesystem::path path = media->path().concat(media->suffix(".jpg"));
		
		
		QString uri = QUrl::fromLocalFile(
			QString::fromStdString(path.string())
		).toString();

		QByteArray hash = QCryptographicHash::hash(
			uri.toUtf8(),
			QCryptographicHash::Sha256
		);

		std::string hashstr = hash.toHex().toStdString();
		std::filesystem::path thumbnailPath = (_thumbnailDirectory / hashstr).concat(".png");
		
		auto *job = new Job{
			media,
			thumbnailPath,
			size,
			std::move(callback)
		};

		_threadPool.start(job);
	}
	
	void ThumbnailManager::Job::run() {
		LOG(DEBUG, "Generating thumbnail for '" << _media->path() << "' at '" << _thumbPath << "'");
		if (!std::filesystem::is_regular_file(_thumbPath)) {
			std::optional<QImage> thumb = _media->thumbnail(_size);
			if (thumb == std::nullopt) {
				return;
			}
			bool success = thumb->save(_thumbPath.c_str());
			if (!success) {
				LOG(ERROR, "Failed saving thumbnail for '" << _media->path() << "' to '" << _thumbPath << "'");
				return;
			}
		}

		QImageReader reader(_thumbPath.c_str());
		QImage thumb = reader.read();
		if (thumb.isNull()) {
			LOG(ERROR, "Failed loading thumbnail for '" << _media->path() << "' from '" << _thumbPath << "': " << reader.errorString());
		}
		QMetaObject::invokeMethod(
			easyqt::ObjectRegistry::get<MediaView>(),
			[callback = std::move(_callback), thumb]() {
				callback(thumb);
			},
			Qt::QueuedConnection
		);
	}
}

