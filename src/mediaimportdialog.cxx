#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QTreeView>
#include <QPushButton>
#include <QVBoxLayout>

#include <fmt/chrono.h>

#include <easyqt/logging.hxx>
#include <easyqt/objectregistry.hxx>
#include <easyqt/pathentry.hxx>
#include <easyqt/settingsdialog.hxx>
#include <easyqt/spinner.hxx>

#include "mediadb/importer.hxx"
#include "mediaimportdialog.hxx"
#include "mediaitemmodel.hxx"

namespace pelican {
	void MediaImportDialog::initImpl() {
		_importer.setStatusCallback([this](mediadb::Importer::ProgressStatus status) { this->importerCallback(status); });
		setWindowTitle("Import media");

		auto layout = new QVBoxLayout;
		setLayout(layout);

		auto label = new QLabel(
			"<b>Import media</b><br/><br/>"
			"1. Select directory to import media from"
		);
		layout->addWidget(label);
		
		_importPathEntry = new easyqt::PathEntry;
		connect(_importPathEntry, &easyqt::PathEntry::pathChanged, this, &MediaImportDialog::onImportPathChange);
		_importPathEntry->setPathType(easyqt::PathEntry::PathType::Directory);
		_importPathEntry->setPath(easyqt::ObjectRegistry::get<easyqt::SettingsDialog>()->settingValue<std::filesystem::path>("last-media-import-directory"));
		layout->addWidget(_importPathEntry);

		label = new QLabel("2. Scan");
		layout->addWidget(label);

		QHBoxLayout* scanLayout = new QHBoxLayout;
		layout->addLayout(scanLayout);

		_spinner = new easyqt::Spinner;
		scanLayout->addWidget(_spinner, 0, Qt::AlignLeft);

		_scanProgressLabel = new QLabel("Scanning done, found 9999999999 files");
		_scanProgressLabel->hide();
		scanLayout->addWidget(_scanProgressLabel, 0, Qt::AlignRight);

		_scanStartAbortButton = new QPushButton("Start");
		_scanStartAbortButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		connect(_scanStartAbortButton, &QPushButton::clicked, this, &MediaImportDialog::onScanButtonClicked);
		scanLayout->addWidget(_scanStartAbortButton, 0, Qt::AlignRight);

		label = new QLabel("3. Review media");
		layout->addWidget(label);

		_scannedMediaModel = new MediaItemModel;

		_scannedMediaView = new QTreeView;
		_scannedMediaView->setModel(_scannedMediaModel);
		layout->addWidget(_scannedMediaView);

		label = new QLabel("4. Import");
		layout->addWidget(label);

		_importProgressBar = new QProgressBar;
		_importProgressBar->setRange(0, 1);
		layout->addWidget(_importProgressBar);

		QHBoxLayout* importLayout = new QHBoxLayout;
		layout->addLayout(importLayout);

		_importProgressLabel = new QLabel("Importing done, 999 TB of 999 TB, 00:00:00 elapsed, 00:00:00 left");
		importLayout->addWidget(_importProgressLabel, 0, Qt::AlignLeft);

		_importStartAbortButton = new QPushButton("Start");
		importLayout->addWidget(_importStartAbortButton, 0, Qt::AlignRight);

		_buttonBox = new QDialogButtonBox(
			QDialogButtonBox::Close
		);
		connect(_buttonBox, &QDialogButtonBox::clicked, this, &MediaImportDialog::onButtonClicked);
		layout->addWidget(_buttonBox);
	}

	void MediaImportDialog::importerCallback(mediadb::Importer::ProgressStatus status) {
		switch (status) {
			case mediadb::Importer::ProgressStatus::FileScanned:
				QMetaObject::invokeMethod(this, [this]{
					_scanProgressLabel->setText(std::format(
						"Scanning ... found {} files, {} medias",
						_importer.scannedFilesCount(),
						_importer.scannedMediasCount()
					).c_str());
				}, Qt::QueuedConnection);
				break;
			case mediadb::Importer::ProgressStatus::ScanningFinished:
				QMetaObject::invokeMethod(this, [this]{
					_scanProgressLabel->setText(std::format(
						"Scanning done, found {} files, {} medias",
						_importer.scannedFilesCount(),
						_importer.scannedMediasCount()
					).c_str());
					_scanStartAbortButton->setText("Restart");
					_spinner->stop();
					_scannedMediaModel->setMediaCollection(_importer.scannedMedias());
					//_importProgressBar->setMaximum(_importer.scannedMediaCollection()->totalDiskSpace());
				}, Qt::QueuedConnection);
				break;
			case mediadb::Importer::ProgressStatus::MediaImported:
				QMetaObject::invokeMethod(this, [this]{
					_scanProgressLabel->setText(fmt::format(
						"Importing ... {} of {}, {} elapsed, {} left",
						_importer.importedMediasCount(),
						_importer.scannedMediasCount(),
						_importer.importingTimeElapsed(),
						_importer.importingTimeLeft(),
					).c_str());
				}, Qt::QueuedConnection);
				break;
			case mediadb::Importer::ProgressStatus::ImportingFinished:
				QMetaObject::invokeMethod(this, [this]{
					_scanProgressLabel->setText(std::format(
						"Importing done, {} medias processed",
						_importer.importedMediasCount()
					).c_str());
				}, Qt::QueuedConnection);
				break;
		}
	}

	void MediaImportDialog::onImportPathChange(std::filesystem::path path) {
		_importer.setPath(path);
	}

	void MediaImportDialog::onScanButtonClicked() {
		switch (_importer.status()) {
			case mediadb::Importer::Status::Inactive:
				_spinner->start();
				_scanProgressLabel->show();
				_scanStartAbortButton->setText("Abort");
				_importer.scan();
				easyqt::ObjectRegistry::get<easyqt::SettingsDialog>()->setSettingValue("last-media-import-directory", _importPathEntry->path());
				break;
			case mediadb::Importer::Status::Scanning:
				_spinner->stop();
				_scanProgressLabel->show();
				_scanStartAbortButton->setText("Scan");
				_importer.stop();
				break;
			default:
				break;
		}
	}

	void MediaImportDialog::onButtonClicked(QAbstractButton* button) {
		QDialogButtonBox::StandardButton role = _buttonBox->standardButton(button);
		if (role == QDialogButtonBox::Close) {
			close();
		}
	}
}