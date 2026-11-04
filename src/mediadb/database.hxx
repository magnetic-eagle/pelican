#pragma once

#include <easyqt/object.hxx>

#include "media.hxx"

namespace pelican::mediadb {
	class Importer;

	class Database: public easyqt::Object<> {
		public:
			friend Importer;
		
		protected:
			void addMedia(MediaPtr media) {};
			virtual void initImpl() override;

		private:
			MediaPtrVector _medias;
	};
}