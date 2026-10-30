#pragma once

#include <filesystem>

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTreeView>

#include <easyqt/object.hxx>
#include <easyqt/pathentry.hxx>
#include <easyqt/spinner.hxx>

#include "mediadb/importer.hxx"
#include "mediaitemmodel.hxx"

namespace pelican {
	class MediaImportDialog: public easyqt::Object<QDialog> {
		Q_OBJECT
		
		public slots:
			void onButtonClicked(QAbstractButton* button);
			void onScanButtonClicked();
			void onImportPathChange(std::filesystem::path path);

			void importerCallback(mediadb::Importer::ProgressStatus status);
		
		protected:
			virtual void initImpl() override;
		
		private:
			easyqt::PathEntry* _importPathEntry;
			QPushButton* _startAbortButton;
			easyqt::Spinner* _spinner;
			QLabel* _scanProgressLabel;
			QTreeView* _scannedMediaView;
			MediaItemModel* _scannedMediaModel;
			QProgressBar* _importProgressBar;
			QDialogButtonBox* _buttonBox;

			mediadb::Importer _importer;
	};
}