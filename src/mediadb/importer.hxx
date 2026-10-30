#pragma once

#include <filesystem>
#include <functional>
#include <thread>

#include "collection.hxx"

namespace pelican::mediadb {
	class Importer {
		public:
			enum class ProgressStatus {
				FileScanned,
				ScanningFinished,
				MediaImported,
				ImportingFinished,
			};

			enum class Status {
				Inactive,
				Scanning,
				Importing,
			};

			Importer();

			void setPath(std::filesystem::path path) { _path = path; }
			void setStatusCallback(std::function<void(ProgressStatus)> f) { _progressCallback = f; }
			void scan();
			void import();
			void stop();

			std::size_t scannedMediasCount() const { return _scannedMedias->mediaCount(); }
			std::uintmax_t scannedFilesCount() const { return _scannedFilesCount; }
			std::size_t importedMediasCount() const { return _importedMedias->mediaCount(); }

			Status status() const { return _status; }

			CollectionPtr scannedMedias() const {return _scannedMedias; };
		
		protected:
			void scanInternal();
			void importInternal();
		
		private:
			std::filesystem::path _path;
			mediadb::CollectionPtr _scannedMedias;
			std::uintmax_t _scannedFilesCount;
			mediadb::CollectionPtr _importedMedias;
			std::uintmax_t _bytesImported = 0;

			std::function<void(ProgressStatus)> _progressCallback = nullptr;
			std::thread _workerThread;
			std::atomic<bool> _workerThreadBusy = false;
			std::atomic<Status> _status = Status::Inactive;
			std::atomic<bool> _stopFlag;
	};
}