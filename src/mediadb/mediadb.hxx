#pragma once

#include <easyqt/object.hxx>

#include "media.hxx"

namespace pelican {
	class MediaDB: public easyqt::Object<> {
		protected:
			virtual void initImpl() override;

		private:
			MediaPtrVector _medias;
	};
}