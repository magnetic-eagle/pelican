#ifndef PELICAN_MEDIALIST_HXX
#define PELICAN_MEDIALIST_HXX

#include <memory>

#include <QEnterEvent>
#include <QFileSystemWatcher>
#include <QGraphicsEffect>
#include <QGridLayout>
#include <QLabel>
#include <QPoint>
#include <QRubberBand>
#include <QScrollArea>

#include <easyqt/object.hxx>
#include <easyqt/scrollarea.hxx>
#include <easyqt/viewedflowlayout.hxx>

namespace pelican {
	class MediaView;
	class MediaViewEntry;
	class MediaViewEntryWidget;
	typedef std::shared_ptr<MediaViewEntry> MediaViewEntryPtr;
	
	class MediaView: public easyqt::Object<easyqt::ScrollArea> {
		Q_OBJECT
		
		public:
			friend MediaViewEntryWidget;
			
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
			bool eventFilter(QObject* watched, QEvent* event) override;

		protected:
			void initImpl() override { rebuild(); };
			void selectEntry(MediaViewEntryPtr entry, SelectionOperation op);
			MediaViewEntryPtr getMediaEntry(unsigned int index) const { return _mediaEntries.at(index); }
			void updateVisibleWidgets();
		
		private:
			easyqt::ViewedFlowLayout _layout;
			std::vector<MediaViewEntryPtr> _mediaEntries;
			int _selectionStartEntry = -1;
			QRubberBand* _rubberBand;
			QPoint _rubberBandOrigin;
	};
}

#endif

