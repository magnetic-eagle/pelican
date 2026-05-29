#ifndef PELICAN_MEDIAINFOPANE_HXX
#define PELICAN_MEDIAINFOPANE_HXX

#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include <easyqt/object.hxx>

#include "media.hxx"

namespace pelican {
	class MediaInfoPane: public easyqt::Object<QWidget> {
		Q_OBJECT
		public:
			void setMedia(MediaPtr media);
			void showMediaInfo();
		
		protected:
			virtual void initImpl() override;
		
		private:
			QGridLayout _layout;
			QLabel _nameLabel, _focalLengthLabel, _shutterSpeedLabel, _apertureLabel, _isoLabel;
			MediaPtr _media;
	};
}

#endif

