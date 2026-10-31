#include <easyqt/logging.hxx>

#include "importer.hxx"

namespace pelican::mediadb {
	Importer::Importer() {
		_scannedMedias = std::make_shared<Collection>();
		_importedMedias = std::make_shared<Collection>();
	}

	void Importer::scanInternal() {
		_scannedMedias->clear();
		_scannedFilesCount = 0;

		for (const auto& entry : std::filesystem::recursive_directory_iterator(_path)) {
			if (_stopFlag) {
				break;
			}

			if (!(entry.is_regular_file() || entry.is_symlink())) {
				continue;
			}

			const auto stem = entry.path().stem();
			const auto ext = entry.path().extension();
			const auto dir = entry.path().parent_path();
			const auto path = dir / stem;
			MediaPtr media = _scannedMedias->findMediaByPath(path);
			if (!media) {
				media = std::make_shared<Media>(path);
				_scannedMedias->addMedia(media);
			}
			media->addSuffix(ext);
			_scannedFilesCount++;

			if (_progressCallback) {
				_progressCallback(ProgressStatus::FileScanned);
			}
		}

		if (_progressCallback) {
			_progressCallback(ProgressStatus::ScanningFinished);
		}
		_workerThreadBusy = false;
		_stopFlag = false;
		_status = Status::Inactive;
	}

	void Importer::scan() {
		if (_workerThreadBusy) {
			return;
		}

		if (_workerThread.joinable()) {
			_workerThread.join();
		}
		_workerThreadBusy = true;
		_status = Status::Scanning;
		_workerThread = std::thread(&Importer::scanInternal, this);
	}

	void Importer::importInternal() {
		_importedMedias->clear();
		_bytesImported = 0;

		for (auto media: _scannedMedias->medias()) {
			if (_stopFlag) {
				break;
			}
			_bytesImported += media->totalDiskSpace();
		}

		if (_progressCallback) {
			_progressCallback(ProgressStatus::ImportingFinished);
		}
		_workerThreadBusy = false;
		_stopFlag = false;
		_status = Status::Inactive;
	}

	void Importer::import() {
		if (_workerThreadBusy) {
			return;
		}
		if (_workerThread.joinable()) {
			_workerThread.join();
		}
		_workerThreadBusy = true;
		_status = Status::Importing;
		_workerThread = std::thread(&Importer::importInternal, this);
	}

	void Importer::stop() {
		if (_workerThreadBusy) {
			_stopFlag = true;
		}
	}
}
