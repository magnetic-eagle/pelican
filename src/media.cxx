#include <easyqt/logging.hxx>
#include <easyqt/utils.hxx>

#include <exiv2/exiv2.hpp>
#include <memory>

#include "media.hxx"
#include "thumbnailmanager.hxx"

namespace pelican {
	void Media::addSuffix(std::filesystem::path suffix) {
		_suffixes.insert(suffix.string());
	}
	
	void Media::addSuffix(std::string suffix) {
		_suffixes.insert(suffix);
	}
	
	QPixmap Media::thumbnail(int width, int height) {
		return ThumbnailManager::instance()->thumbnail(this, width, height);
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
		} catch (Exiv2::BasicError<char>& e) {
			LOG(ERROR, "Error opening file for getting pixel size: " << e.what());
			return QSize();
		}
		return {image->pixelWidth(), image->pixelHeight()};
	}
}

