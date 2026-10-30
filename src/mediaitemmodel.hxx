#pragma once

#include <QAbstractItemModel>

#include "mediadb/collection.hxx"

namespace pelican {
	class MediaItemModel : public QAbstractItemModel {
		Q_OBJECT
		public:
			explicit MediaItemModel(QObject* parent = nullptr);

			// Updates the model with a new list of media
			void setMediaCollection(mediadb::CollectionPtr mediaList);

			// QAbstractListModel overrides
			QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
			QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
			int rowCount(const QModelIndex& parent = QModelIndex()) const override;
			int columnCount(const QModelIndex& parent = QModelIndex()) const override;
			QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
			QModelIndex parent(const QModelIndex& index) const override;

		private:
			mediadb::CollectionPtr _data;
	};
}
