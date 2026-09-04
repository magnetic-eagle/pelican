#include <cctype>
#include <memory>

#include <easyqt/logging.hxx>
#include <easyqt/objectregistry.hxx>
#include <easyqt/utils.hxx>

#include <FreeImagePlus.h>

#include <exiv2/exiv2.hpp>

#include "media.hxx"

namespace pelican {
	void Media::addSuffix(std::filesystem::path suffix) {
		addSuffix(suffix.string());
	}
	
	void Media::addSuffix(std::string suffix) {
		_suffixes.insert(suffix);
		std::string lowerSuffix = std::tolower(suffix);
		if (_type == Type::Image and (lowerSuffix == ".mp4" or lowerSuffix == ".mpeg" or lowerSuffix == ".mts" or lowerSuffix == ".mkv")) {
			_type = Type::Video;
		}
	}
	
	std::optional<QImage> Media::thumbnail(unsigned int size) {
		std::string mediaPath = path().concat(suffix(".jpg")).string();
		fipImage image;
		bool success = image.load(mediaPath.c_str());
		if (!success) {
			LOG(ERROR, "Failed loading '" << mediaPath << "' for thumbnail generation");
			return std::nullopt;
		}
		success = image.makeThumbnail(size);
		if (!success) {
			LOG(ERROR, "Failed creating thumbnail for '" << mediaPath << "'");
			return std::nullopt;
		}

		QImage::Format format = QImage::Format_ARGB32;
		switch (image.getBitsPerPixel()) {
			case 24:
				format = QImage::Format_BGR888;
				break;
			case 32:
				format = QImage::Format_ARGB32;
				break;
			default:
				image.convertTo32Bits();
				format = QImage::Format_ARGB32;
		}
		image.flipVertical();
		return QImage(
			image.accessPixels(),
			image.getWidth(),
			image.getHeight(),
			image.getScanWidth(),
			format
		).copy();
	}

	std::string Media::suffixForExtension(std::string ext) {
		ext = std::tolower(ext);
		for (const auto& suffix: _suffixes) {
			if (std::tolower(suffix) == ext) {
				return suffix;
			}
		}
		return "";
	}
	
	std::string Media::suffix(std::string preferred) {
		std::string s;
		if (!preferred.empty()) {
			s = suffixForExtension(preferred);
		}
		if (s.empty()) {
			s = *_suffixes.begin();
		}
		return s;
	}
	
	std::filesystem::path Media::path() {
		return directory() / filename();
	}
	
	std::vector<std::filesystem::path> Media::paths() {
		std::vector<std::filesystem::path> v;
		for (const auto& suffix: _suffixes) {
			v.push_back(path().concat(suffix));
		}
		return v;
	}
	
	QSize Media::size(std::string suffix) {
		std::unique_ptr<Exiv2::Image> image;
		try {
			image = Exiv2::ImageFactory::open(path().concat(suffix));
		} catch (Exiv2::Error& e) {
			LOG(ERROR, "Error opening file for getting pixel size: " << e.what());
			return QSize();
		}
		// I think we can safely use a cast to int here, because if we are loading an image that is more than 2 billion pixels large, we got bigger problems ...
		return {(int)image->pixelWidth(), (int)(image->pixelHeight())};
	}

	void Media::deleteFiles() {
		bool success = true;
		for (const auto& path: paths()) {
			LOG(INFO, "Deleting " << std::quoted(path.string()));
			success &= std::filesystem::remove(path);
		}
		if (!success) {
			LOG(ERROR, "Some files could not be deleted");
		}
	}
}

