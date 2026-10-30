#pragma once

#include <cstdint>
#include <memory>

#include "media.hxx"

namespace pelican::mediadb {
	class Collection {
		public:
			void addMedia(MediaPtr media);
			void addMedias(MediaPtrVector medias);
			void removeMedia(MediaPtr media);
			void removeMedias(MediaPtrVector);
			void clear();

			MediaPtr findMediaByPath(std::filesystem::path path) const;
			MediaPtr atIndex(size_t index);
			MediaPtrSet medias() const { return _medias; };
			std::size_t mediaCount() const { return _medias.size(); };
			std::uintmax_t totalDiskSpace();
		
		protected:
			void recalculateTotalDiskSpace();
		
		private:
			std::uintmax_t _totalDiskSpace = 0;
			bool _totalDiskSpaceOutdated = false;

			MediaPtrSet _medias;
	};

	using CollectionPtr = std::shared_ptr<Collection>;
}
