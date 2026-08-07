#ifndef PELICAN_MEDIA_HXX
#define PELICAN_MEDIA_HXX

#include <filesystem>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <QPixmap>

namespace pelican {
	class Media {
		public:
			Media(std::filesystem::path directory, std::string filename, std::set<std::string> suffixes = {}):
					_directory(directory), _filename(filename), _suffixes(suffixes)
			{};
		
			// @description Add suffix (file type) for this media 
			//	(photos often come in JPG and RAW formats, videos might have a thumbnail)
			// @param suffix Suffix to add
			void addSuffix(std::filesystem::path suffix);
			void addSuffix(std::string suffix);
			
			// @description Get suffix for a given extension
			//	(needed to get the correctly-cased suffix when the file extension is in a different case than assumed)
			// @param ext Wanted extension
			// @return Suffix matching file extension @param ext, or empty string if no suffix corresponding to @param ext is found
			std::string suffixForExtension(std::string ext);
			
			// @description Generate a thumbnail of this media
			// @param size Desired size of the thumbnail in its larger dimension
			// @return The thumbnail, or std::nullopt if there is an error reading the media or generating the thumbnail
			std::optional<QImage> thumbnail(unsigned int size);
			
			// @return Path to this media without file extension
			std::filesystem::path path();
			// @return Path to the directory that contains this media
			std::filesystem::path directory() { return _directory; };
			// @return Name of this media (without extension)
			std::string filename() { return _filename; };
			// @return Suffixes (file extensions) of all files asociated with this media. There is always at least one suffix
			std::set<std::string> suffixes() { return _suffixes; };
			// @return Paths to all files associated with this media.
			std::vector<std::filesystem::path> paths();
			
			// @description Get an available suffix (file extension) for this media
			// @param preferred Which file extension is preferred - if available, the suffix for that extension will be returned.
			// 	If @param preferred is empty or there is no suffix for that extension, the first available suffix will be returned
			std::string suffix(std::string preferred = "");
			
			QSize size(std::string suffix);
		
		private:
			std::filesystem::path _directory;
			std::string _filename;
			std::set<std::string> _suffixes;
	};
	
	using MediaPtr = std::shared_ptr<Media>;
	using MediaPtrVector = std::vector<MediaPtr>;
}

#endif

