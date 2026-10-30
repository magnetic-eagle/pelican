#include <easyqt/utils.hxx>

#include "collection.hxx"
#include "media.hxx"

namespace pelican::mediadb {
	void Collection::addMedia(MediaPtr media) {
		if (_medias.insert(media).second) {
			_totalDiskSpaceOutdated = true;
		}
	}

	void Collection::addMedias(MediaPtrVector medias) {
		for (auto media: medias) {
			addMedia(media);
		}
	}

	void Collection::removeMedia(MediaPtr media) {
		if (_medias.erase(media)) {
			_totalDiskSpaceOutdated = true;
		}
	}

	void Collection::removeMedias(MediaPtrVector medias) {
		for (auto media: medias) {
			removeMedia(media);
		}
	}

	void Collection::clear() {
		_medias.clear();
		_totalDiskSpace = 0;
	}

	MediaPtr Collection::findMediaByPath(std::filesystem::path path) const {
		auto it = std::binary_find(
			_medias.begin(), _medias.end(), path, 
			[](MediaPtr media) -> std::filesystem::path { return media->path(); }, 
			std::less{}
		);
		if (it == _medias.end()) {
			return nullptr;
		} else {
			return *it;
		}
	}

	MediaPtr Collection::atIndex(size_t index) {
		if (index >= mediaCount()) {
			return nullptr;
		}
		auto it = _medias.begin();
		std::advance(it, index);
		if (it != _medias.end()) {
			return *it;
		} else {
			return nullptr;
		}
	}

	std::uintmax_t Collection::totalDiskSpace() {
		if (_totalDiskSpaceOutdated) {
			recalculateTotalDiskSpace();
		}
		return _totalDiskSpace;
	}

	void Collection::recalculateTotalDiskSpace() {
		_totalDiskSpace = 0;
		for (auto media: _medias) {
			_totalDiskSpace += media->totalDiskSpace();
		}
	}
}