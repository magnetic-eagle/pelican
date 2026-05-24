#include <algorithm>
#include <sstream>

#include <QStandardPaths>

#include <FreeImagePlus.h>

#include <easyqt/logging.hxx>

#include "thumbnailmanager.hxx"

namespace pelican {
	ThumbnailManager::ThumbnailManager() {
		_thumbnailDirectory = std::filesystem::path(QStandardPaths::writableLocation(QStandardPaths::CacheLocation).toStdString()) / "thumbnails";
		std::filesystem::create_directories(_thumbnailDirectory);
	}
	
	QPixmap ThumbnailManager::thumbnail(Media* media, int width, int height) {
		// Get media path
		std::filesystem::path path = media->path().concat(media->suffix(".jpg"));
		
		
		// Hashing function taken from https://stackoverflow.com/a/13325223/14909980
		unsigned long long int hash = 7 + width + height;
		for (const auto& c: path.filename().string()) {
			hash = hash * 31 + c;
		}
		
		std::ostringstream hashstr;
		hashstr << std::setfill('0') << std::setw(sizeof(unsigned long long int) * 2) << std::hex << hash;
		std::filesystem::path thumbnailPath = (_thumbnailDirectory / hashstr.str()).concat(".png");
		
		if (!std::filesystem::is_regular_file(thumbnailPath)) {
			generateThumbnail(media, thumbnailPath, width, height);
		}
		LOG(DEBUG, "Loading thumbnail for " << path << " from " << thumbnailPath);
		return QPixmap(thumbnailPath.c_str());
	}
	
	void ThumbnailManager::generateThumbnail(Media* media, std::filesystem::path thumbnailPath, int width, int height) {
		std::string mediaPath = media->path().concat(media->suffix(".jpg")).string();
		LOG(DEBUG, "Generating thumbnail for '" << mediaPath << "' at '" << thumbnailPath << "'");
		fipImage originalImage;
		bool success = originalImage.load(mediaPath.c_str());
		if (!success) {
			LOG(ERROR, "Failed loading '" << mediaPath << "' for thumbnail generation");
			return;
		}
		success = originalImage.makeThumbnail(std::max(width, height));
		if (!success) {
			LOG(ERROR, "Failed creating thumbnail for '" << mediaPath << "'");
			return;
		}
		success = originalImage.save(thumbnailPath.c_str());
		if (!success) {
			LOG(ERROR, "Failed saving thumbnail for '" << mediaPath << "' to '" << thumbnailPath << "'");
		}
	}
}

