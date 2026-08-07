#ifndef PELICAN_MEDIALIST_HXX
#define PELICAN_MEDIALIST_HXX

#include <QEnterEvent>
#include <QFileSystemWatcher>
#include <QGridLayout>
#include <QLabel>
#include <QPoint>
#include <QRubberBand>
#include <QScrollArea>

#include <easyqt/flowlayout.hxx>
#include <easyqt/scrollarea.hxx>
#include <easyqt/object.hxx>

#include "media.hxx"

namespace pelican {
	class MediaView;
	
	class MediaViewEntry: public QWidget {
		Q_OBJECT
		
		public:
			friend MediaView;
			
			MediaViewEntry(MediaPtr media);
			void showThumbnail();
			void deleteFiles();
			bool selected() { return _selected; };
			MediaPtr media() { return _media; };
		
			virtual void paintEvent(QPaintEvent* event) override;
			virtual void mousePressEvent(QMouseEvent* event) override;
			virtual void mouseReleaseEvent(QMouseEvent* event) override;
			virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
			virtual void enterEvent(QEnterEvent* event) override;
			virtual void leaveEvent(QEvent* event) override;
		
		protected:
			void setSelected(bool selected);
		
		private:
			QGridLayout _layout;
			QLabel _thumbnailLabel;
			QLabel _nameLabel;
			
			MediaPtr _media;
			bool _selected = {false};
	};
	
	class MediaView: public easyqt::Object<easyqt::ScrollArea> {
		Q_OBJECT
		
		public:
			friend MediaViewEntry;
			
			enum SelectionOperation {
				ReplaceSelection,
				AddToSelection,
				RemoveFromSelection,
				AddRangeToSelection,
			};
			
			MediaView();
			void rebuild();
			void invertSelection();
			void selectAll();
			void clearSelection();
			
			virtual void resizeEvent(QResizeEvent* event) override;
			virtual void mousePressEvent(QMouseEvent* event) override;
			virtual void mouseMoveEvent(QMouseEvent* event) override;
			virtual void mouseReleaseEvent(QMouseEvent* event) override;
			virtual void keyPressEvent(QKeyEvent* event) override;
			virtual QSize sizeHint() const override;
			virtual QSize minimumSizeHint() const override;
			
		protected:
			void initImpl() override { rebuild(); };
			void selectEntry(MediaViewEntry* entry, SelectionOperation op);
		
		private:
			easyqt::FlowLayout _layout;
			std::vector<MediaViewEntry*> _mediaEntries;
			MediaViewEntry* _selectionStartEntry = nullptr;
			QRubberBand* _rubberBand;
			QPoint _rubberBandOrigin;
	};
} 

#endif

