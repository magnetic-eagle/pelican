#include <atomic>
#include <memory>

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
	
	std::shared_ptr<std::atomic_bool> ThumbnailManager::requestThumbnail(MediaPtr media, unsigned int size, std::function<void(QImage)> callback, ThumbnailPriority priority) {
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
		
		auto cancelled = std::make_shared<std::atomic_bool>(false);
		auto *job = new Job{
			media,
			thumbnailPath,
			size,
			std::move(callback),
			cancelled
		};

		_threadPool.start(job, (int)priority);

		return cancelled;
	}
	
	void ThumbnailManager::Job::run() {
		if (_cancelled->load()) {
			return;
		}
		if (!std::filesystem::is_regular_file(_thumbPath)) {
			LOG(DEBUG, "Generating thumbnail for '" << _media->path() << "' at '" << _thumbPath << "'");
			std::optional<QImage> thumb = _media->thumbnail(_size);

			if (_cancelled->load()) {
				return;
			}
			if (thumb == std::nullopt) {
				return;
			}

			bool success = thumb->save(_thumbPath.c_str());
			if (_cancelled->load()) {
				return;
			}

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
		if (_cancelled->load()) {
			return;
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

