#include <QModelIndex>
#include <QVariant>

#include <easyqt/utils.hxx>

#include "mediaitemmodel.hxx"

namespace pelican {
	MediaItemModel::MediaItemModel(QObject* parent):
		QAbstractItemModel(parent)
	{
	}

	void MediaItemModel::setMediaCollection(mediadb::CollectionPtr collection) {
		beginResetModel();
		_data = collection;
		endResetModel();
	}

	QVariant MediaItemModel::data(const QModelIndex& index, int role) const {
		if (!index.isValid() || index.row() >= rowCount() || index.column() > 3 || !_data) {
			return QVariant();
		}

		const auto& media = _data->atIndex(index.row());

		if (role == Qt::DisplayRole) {
			switch (index.column()) {
				case 0:
					return QString::fromStdString(media->filename());
				case 1:
					return QString::fromStdString(media->directory());
				case 2:
					return QString::fromStdString(media->typeString());
				case 3:
					return QString::fromStdString(easyqt::formatFileSize(media->totalDiskSpace()));
			}
		}

		return QVariant();
	}

	QVariant MediaItemModel::headerData(int section, Qt::Orientation orientation, int role) const {
		if (role == Qt::DisplayRole) {
			switch (section) {
				case 0:
					return "Name";
				case 1:
					return "Path";
				case 2:
					return "Type";
				case 3:
					return "Size";
			}
		}
		return QVariant();
	}

	int MediaItemModel::rowCount(const QModelIndex& parent) const {
		if (parent.isValid() || !_data) {
			return 0;
		}
		return static_cast<int>(_data->mediaCount());
	}

	int MediaItemModel::columnCount(const QModelIndex& parent) const {
		return 4;
	}

	QModelIndex MediaItemModel::index(int row, int column, const QModelIndex& parent) const {
		if (row < 0 || row >= rowCount() || column < 0 || column >= 4) {
			return QModelIndex();
		}
		return createIndex(row, column);
	}

	QModelIndex MediaItemModel::parent(const QModelIndex& index) const {
		return QModelIndex();
	}
}