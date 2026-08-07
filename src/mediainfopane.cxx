#include <QThreadPool>

#include <exiv2/tags.hpp>
#include <fmt/core.h>

#include <exiv2/exiv2.hpp>

#include <easyqt/logging.hxx>
#include <qlabel.h>

#include "exifutils.hxx"
#include "mediainfopane.hxx"

namespace pelican {
	void MediaInfoPane::initImpl() {
		_layout.addWidget(new QLabel("Name"), _layout.rowCount(), 0);
		_layout.addWidget(&_nameLabel, _layout.rowCount() - 1, 1);
		_layout.addWidget(new QLabel("Focal length"), _layout.rowCount(), 0);
		_layout.addWidget(&_focalLengthLabel, _layout.rowCount() - 1, 1);
		_layout.addWidget(new QLabel("Shutter speed"), _layout.rowCount(), 0);
		_layout.addWidget(&_shutterSpeedLabel, _layout.rowCount() - 1, 1);
		_layout.addWidget(new QLabel("Aperture"), _layout.rowCount(), 0);
		_layout.addWidget(&_apertureLabel, _layout.rowCount() - 1, 1);
		_layout.addWidget(new QLabel("ISO"), _layout.rowCount(), 0);
		_layout.addWidget(&_isoLabel, _layout.rowCount() - 1, 1);
		
		_layout.addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding), _layout.rowCount(), 0);
		_layout.addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding), _layout.rowCount(), 1);
		_layout.setSpacing(20);
		setLayout(&_layout);
		setFixedWidth(250);
	}
	
	void MediaInfoPane::setMedia(MediaPtr media) {
		_media = media;
		//QThreadPool::globalInstance()->start(std::bind(&MediaInfoPane::showMediaInfo, this));
		showMediaInfo();
	}
	
	void MediaInfoPane::showMediaInfo() {
		if (!_media) {
			_nameLabel.setText("");
			_focalLengthLabel.setText("");
			_shutterSpeedLabel.setText("");
			_apertureLabel.setText("");
			_isoLabel.setText("");
			return;
		}
		std::unique_ptr<Exiv2::Image> image;
		try {
			image = Exiv2::ImageFactory::open(_media->path().concat(_media->suffix(".jpg")));
			image->readMetadata();
		} catch (Exiv2::BasicError<char>& e) {
			LOG(ERROR, "Error opening file for reading metadata: " << e.what());
			return;
		}
		Exiv2::ExifData& exifData = image->exifData();
		
		_nameLabel.setText(_media->filename().c_str());
		const std::map<std::string, std::pair<QLabel&, std::function<std::string(const std::string&)> > > tagMap = {
			{"Exif.Photo.FocalLength", {_focalLengthLabel, &exif::formatFocalLength}},
			{"Exif.Photo.ExposureTime", {_shutterSpeedLabel, &exif::formatShutterSpeed}},
			{"Exif.Photo.FNumber", {_apertureLabel, &exif::formatAperture}},
			{"Exif.Photo.ISOSpeedRatings", {_isoLabel, &exif::formatISO}},
		};
		for (const auto& tagMapEntry: tagMap) {
			const std::string& tag = tagMapEntry.first;
			QLabel& label = tagMapEntry.second.first;
			auto parseFunc = tagMapEntry.second.second;
			auto exifKeyIt = exifData.findKey(Exiv2::ExifKey(tag));
			if (exifKeyIt != exifData.end()) {
				label.setText(
					parseFunc(exifKeyIt->value().toString()).c_str()
				);
			} else {
				label.setText(
					parseFunc("").c_str()
				);
			}
		}
	}
}

