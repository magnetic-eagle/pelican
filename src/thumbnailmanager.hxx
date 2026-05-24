#ifndef PELICAN_THUMBNAILMANAGER_HXX
#define PELICAN_THUMBNAILMANAGER_HXX

#include <filesystem>

#include <QPixmap>

#include <easyqt/singleton.hxx>

#include "media.hxx"

namespace pelican {
	class ThumbnailManager: public easyqt::NamedSingleton<ThumbnailManager> {
		public:
			ThumbnailManager();
		
			QPixmap thumbnail(Media* media, int width, int height);
			QPixmap thumbnail(MediaPtr media, int width, int height) { return thumbnail(media.get(), width, height); };
		
		private:
			void generateThumbnail(Media* media, std::filesystem::path path, int width, int height);
			
			std::filesystem::path _thumbnailDirectory;
	};
}

#endif

