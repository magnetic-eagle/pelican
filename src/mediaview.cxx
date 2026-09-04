#include <atomic>
#include <memory>

#include <QGraphicsEffect>
#include <QImage>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPointer>
#include <QRect>
#include <QRubberBand>
#include <QSize>
#include <QScrollBar>
#include <QThreadPool>

#include <easyqt/logging.hxx>
#include <easyqt/objectregistry.hxx>
#include <easyqt/viewedflowlayout.hxx>

#include "application.hxx"
#include "media.hxx"
#include "mediainfopane.hxx"
#include "mediashowarea.hxx"
#include "thumbnailmanager.hxx"
#include "mediaview.hxx"

#define THUMBNAIL_SIZE 128
#define HOVER_COLOR "#bcf"
#define SELECTED_COLOR "#46f"

namespace pelican {
	class MediaViewEntryWidget: public easyqt::ViewedFlowLayoutWidget {
		
		public:
			friend MediaView;
			
			MediaViewEntryWidget();

			virtual void itemIndexChanged() override;
			virtual void itemVisibilityChanged() override;

			MediaViewEntryPtr mediaEntry() { return _mediaEntry; };
			
			virtual void paintEvent(QPaintEvent* event) override;
			virtual void mousePressEvent(QMouseEvent* event) override;
			virtual void mouseReleaseEvent(QMouseEvent* event) override;
			virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
			virtual void enterEvent(QEnterEvent* event) override;
			virtual void leaveEvent(QEvent* event) override;
			
		private:
			void showThumbnail();
			void resetThumbnail();
			void requestThumbnail();

			QGridLayout* _layout;
			QLabel* _thumbnailLabel;
			unsigned int _thumbnailGeneration = 0;
			QLabel* _nameLabel;
			QGraphicsColorizeEffect* _thumbnailTint;
			std::shared_ptr<std::atomic_bool> _thumbnailCancelled;
			
			MediaViewEntryPtr _mediaEntry = nullptr;
	};

	class MediaViewEntry {
		public:
			friend MediaView;

			MediaViewEntry(MediaPtr media): media(media) {}

			MediaPtr media = {};


			bool selected() const { return _selected; }
			void setSelected(bool selected) {
				_selected = selected;
				/*if (widget) {
					widget->update();
				}*/
			}
			int index() const { return _index; }
		
		protected:
			void setIndex(int index) { _index = index; }
		
		private:
			bool _selected = false;
			int _index = -1;
	};

	MediaViewEntryWidget::MediaViewEntryWidget() {
		_layout = new QGridLayout();
		_layout->setContentsMargins(10, 10, 10, 10);
		_layout->setHorizontalSpacing(0);
		_layout->setVerticalSpacing(10);
		setLayout(_layout);
		setFocusPolicy(Qt::NoFocus);
		
		_thumbnailLabel = new QLabel;
		_thumbnailLabel->setFixedSize(THUMBNAIL_SIZE, THUMBNAIL_SIZE);
		_layout->addWidget(_thumbnailLabel, 0, 0);
		_layout->setAlignment(_thumbnailLabel, Qt::AlignCenter);
		resetThumbnail();

		_nameLabel = new QLabel("<No name>");
		_layout->addWidget(_nameLabel, 1, 0);
		_layout->setAlignment(_nameLabel, Qt::AlignCenter);

		_thumbnailTint = new QGraphicsColorizeEffect(_thumbnailLabel);
		_thumbnailTint->setColor(HOVER_COLOR);
		_thumbnailTint->setStrength(0.25);
		_thumbnailTint->setEnabled(false);
		_thumbnailLabel->setGraphicsEffect(_thumbnailTint);
	}

	void MediaViewEntryWidget::resetThumbnail() {
		Media::Type type = Media::Type::Image;
		if (_mediaEntry and _mediaEntry->media) {
			type = _mediaEntry->media->type();
		}
		switch (type) {
			case Media::Type::Image:
				_thumbnailLabel->setPixmap(QIcon::fromTheme("image-jpeg").pixmap(THUMBNAIL_SIZE, THUMBNAIL_SIZE));
				break;
			case Media::Type::Video:
				_thumbnailLabel->setPixmap(QIcon::fromTheme("video-x-generic").pixmap(THUMBNAIL_SIZE, THUMBNAIL_SIZE));
				break;
		}
	}

	void MediaViewEntryWidget::requestThumbnail() {
		QPointer<MediaViewEntryWidget> self = this;
		if (!_mediaEntry) {
			return;
		}
		_thumbnailCancelled = easyqt::ObjectRegistry::get<ThumbnailManager>()->requestThumbnail(
			_mediaEntry->media,
			THUMBNAIL_SIZE,
			[self](QImage img) {
				if (!self or (self->_thumbnailCancelled and self->_thumbnailCancelled->load())) {
					return;
				}
				self->_thumbnailLabel->setPixmap(QPixmap::fromImage(img));
			},
			_itemVisible ? ThumbnailManager::ThumbnailPriority::VISIBLE : ThumbnailManager::ThumbnailPriority::BUFFER
		);
	}

	void MediaViewEntryWidget::itemIndexChanged() {
		if (_thumbnailCancelled) {
			_thumbnailCancelled->store(true);
		}
		
		_mediaEntry = easyqt::ObjectRegistry::get<MediaView>()->getMediaEntry(itemIndex());
		_nameLabel->setText(_mediaEntry->media->filename().c_str());
		resetThumbnail();
		requestThumbnail();
		
		update();
	}

	void MediaViewEntryWidget::itemVisibilityChanged() {
		if (_thumbnailCancelled) {
			_thumbnailCancelled->store(true);
		}
		requestThumbnail();
	}
	
	void MediaViewEntryWidget::paintEvent(QPaintEvent *event) {
		QPainter painter;
		painter.begin(this);
		
		if (_mediaEntry->selected()) {
			painter.fillRect(rect(), QColor(SELECTED_COLOR));
		}
		painter.end(); 
		QWidget::paintEvent(event);
		
	}
	
	void MediaViewEntryWidget::mousePressEvent(QMouseEvent* event) {
		if (!_mediaEntry) {
			return;
		}
		if (event->button() == Qt::LeftButton) {
			MediaView::SelectionOperation op = MediaView::ReplaceSelection;
			if (event->modifiers() & Qt::ShiftModifier) {
				op = MediaView::AddRangeToSelection;
			} else if (event->modifiers() & Qt::ControlModifier) {
				if (_mediaEntry->selected()) {
					op = MediaView::RemoveFromSelection;
				} else {
					op = MediaView::AddToSelection;
				}
			}
			easyqt::ObjectRegistry::get<MediaView>()->selectEntry(_mediaEntry, op);
		}
	}
	
	void MediaViewEntryWidget::mouseReleaseEvent(QMouseEvent* event) {}
	
	void MediaViewEntryWidget::mouseDoubleClickEvent(QMouseEvent* event) {
		if (!_mediaEntry) {
			return;
		}
		if (event->modifiers() == Qt::NoModifier) {
			easyqt::ObjectRegistry::get<MediaShowArea>()->show();
			easyqt::ObjectRegistry::get<MediaShowArea>()->setMedia(_mediaEntry->media);
		}
	}
	
	void MediaViewEntryWidget::enterEvent(QEnterEvent* event) {
		_thumbnailTint->setEnabled(true);
		update();
	}
	
	void MediaViewEntryWidget::leaveEvent(QEvent* event) {
		_thumbnailTint->setEnabled(false);
		update();
	}
	
	MediaView::MediaView() {
		QWidget* widget = new QWidget(this);
		widget->setLayout(&_layout);
		setWidget(widget);
		_rubberBand = new QRubberBand(QRubberBand::Rectangle, widget);

		_layout.setSpacing(0);
		_layout.setMargin(0);
		
		setFocusPolicy(Qt::ClickFocus);

		viewport()->installEventFilter(this);

		connect(verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int) {
			updateVisibleWidgets();
		});

	}
	
	void MediaView::rebuild() {
		clearSelection();
		_mediaEntries.clear();

		int i = 0;
		for (const auto& media: Application::instance()->medias()) {
			MediaViewEntryPtr mediaEntry = std::make_shared<MediaViewEntry>(media);
			mediaEntry->setIndex(i);
			_mediaEntries.push_back(mediaEntry);
			i++;
		}

		_layout.setItemCount(_mediaEntries.size());
		_layout.setWidgetFactory([]() { return std::make_unique<MediaViewEntryWidget>(); });
	}
	
	QSize MediaView::sizeHint() const {
		if (easyqt::ObjectRegistry::get<MediaShowArea>()->isVisible()) {
			return minimumSizeHint();
		}
		QSize hint = _layout.sizeHint();
		hint.setWidth(
			hint.width() +
			2 * frameWidth() +
			verticalScrollBar()->size().width()
		);
		return hint;
	}
	
	QSize MediaView::minimumSizeHint() const {
		QSize hint = _layout.minimumSize();
		hint.setWidth(
			hint.width() +
			2 * frameWidth() +
			verticalScrollBar()->size().width()
		);
		return hint;
	}
	
	void MediaView::selectEntry(MediaViewEntryPtr entry, SelectionOperation op) {
		if (op == ReplaceSelection) {
			if (_selectionStartEntry != entry->index() || !entry->selected()) {
				clearSelection();
				entry->setSelected(true);
				_selectionStartEntry = entry->index();
				ensureRectVisible(_layout.itemGeometry(entry->index()), 0, 0);
				if (easyqt::ObjectRegistry::get<MediaShowArea>()->isVisible()) {
					easyqt::ObjectRegistry::get<MediaShowArea>()->setMedia(entry->media);
				}
			}
		} else if (op == AddToSelection) {
			entry->setSelected(true);
			_selectionStartEntry = entry->index();
			ensureRectVisible(_layout.itemGeometry(entry->index()), 0, 0);
		} else if (op == RemoveFromSelection) {
			entry->setSelected(false);
			if (_selectionStartEntry == entry->index()) {
				_selectionStartEntry = -1;
			}
		} else if (op == AddRangeToSelection) {
			if (!_selectionStartEntry) {
				selectEntry(entry, ReplaceSelection);
				return;
			}
			
			clearSelection();
			int selectionStartIndex = _selectionStartEntry;
			int selectionEndIndex = entry->index();
			if (selectionStartIndex != selectionEndIndex) {
				if (selectionStartIndex > selectionEndIndex) {
					std::swap(selectionStartIndex, selectionEndIndex);
				}
  				for (; selectionStartIndex != selectionEndIndex; selectionStartIndex++) {
					_mediaEntries[selectionStartIndex]->setSelected(true);
					ensureRectVisible(_layout.itemGeometry(selectionStartIndex), 0, 0);
				}
			}
			ensureRectVisible(_layout.itemGeometry(selectionEndIndex), 0, 0);
		}
		if (_selectionStartEntry) {
			easyqt::ObjectRegistry::get<MediaInfoPane>()->setMedia(_mediaEntries[_selectionStartEntry]->media);
		} else {
			easyqt::ObjectRegistry::get<MediaInfoPane>()->setMedia(nullptr);
		}

		_layout.updateWidgets();
	}
	
	void MediaView::selectAll() {
		for (const auto& mediaEntry: _mediaEntries) {
			mediaEntry->setSelected(true);
		}
		_selectionStartEntry = -1;
		easyqt::ObjectRegistry::get<MediaInfoPane>()->setMedia(nullptr);
		_layout.updateWidgets();
	}
	
	void MediaView::invertSelection() {
		int selected = -1;
		bool multipleSelected = false;
		for (const auto& mediaEntry: _mediaEntries) {
			mediaEntry->setSelected(!mediaEntry->selected());
			if (mediaEntry->selected()) {
				if (selected == -1) {
					selected = mediaEntry->index();
				} else {
					multipleSelected = true;
				}
			}
		}
		if (!multipleSelected) {
			easyqt::ObjectRegistry::get<MediaInfoPane>()->setMedia(_mediaEntries[selected]->media);
			_selectionStartEntry = selected;
			ensureRectVisible(_layout.itemGeometry(selected), 0, 0);
		} else {
			easyqt::ObjectRegistry::get<MediaInfoPane>()->setMedia(nullptr);
			_selectionStartEntry = -1;
		}
		_layout.updateWidgets();
	}
	
	void MediaView::clearSelection() {
		for (const auto& mediaEntry: _mediaEntries) {
			mediaEntry->setSelected(false);
		}
		easyqt::ObjectRegistry::get<MediaInfoPane>()->setMedia(nullptr);
		_layout.updateWidgets();
	}
	
	void MediaView::resizeEvent(QResizeEvent* event) {
		easyqt::ScrollArea::resizeEvent(event);
		
		if (_selectionStartEntry >= 0) {
			ensureRectVisible(_layout.itemGeometry(_selectionStartEntry), 0, 0);
		}
	}
	
	void MediaView::mousePressEvent(QMouseEvent* event) {
		clearSelection();
		
		_rubberBandOrigin = event->pos();
		_rubberBandOrigin += scrollPos();
		_rubberBand->setGeometry(QRect(_rubberBandOrigin, QSize()));
		_rubberBand->show();
		QScrollArea::mousePressEvent(event);
	}

	void MediaView::mouseMoveEvent(QMouseEvent* event) {
		if (_rubberBand->isVisible()) {
			_rubberBand->setGeometry(QRect(_rubberBandOrigin, event->pos() + scrollPos()).normalized());
			
			bool selected = false;
			for (int i = _layout.firstBufferItem(); i < _layout.lastBufferItem(); i++) {
				QRect itemGeometry = _layout.itemGeometry(i);
				selected = itemGeometry.intersects(_rubberBand->geometry());
				_mediaEntries[i]->setSelected(selected);
				if (selected) {
					ensureRectVisible(itemGeometry, 0, 0);
				}
			}
		}
		_layout.updateWidgets();
		
		QScrollArea::mouseMoveEvent(event);
	}

	void MediaView::mouseReleaseEvent(QMouseEvent* event) {
		_rubberBand->hide();
		QScrollArea::mouseReleaseEvent(event);
	}
	
	void MediaView::keyPressEvent(QKeyEvent* event) {
		if (event->key() == Qt::Key_Left) {
			if (_selectionStartEntry > 0 and _selectionStartEntry < _mediaEntries.size()) {
				SelectionOperation op = ReplaceSelection;
				if (event->modifiers() & Qt::ShiftModifier) {
					op = AddToSelection;
				}
				selectEntry(_mediaEntries[_selectionStartEntry - 1], op);
			}
			return;
		} else if (event->key() == Qt::Key_Right) {
			if (_selectionStartEntry >= 0 and _selectionStartEntry < _mediaEntries.size() - 1) {
				SelectionOperation op = ReplaceSelection;
				if (event->modifiers() & Qt::ShiftModifier) {
					op = AddToSelection;
				}
				selectEntry(_mediaEntries[_selectionStartEntry + 1], op);
			}
			return;
		} else if (event->key() == Qt::Key_Up) {
			if (_selectionStartEntry >= _layout.columns() and _selectionStartEntry < _mediaEntries.size()) {
				SelectionOperation op = ReplaceSelection;
				if (event->modifiers() & Qt::ShiftModifier) {
					op = AddRangeToSelection;
				}
				selectEntry(_mediaEntries[_selectionStartEntry - _layout.columns()], op);
			}
			return;
		} else if (event->key() == Qt::Key_Down) {
			if (_selectionStartEntry >= 0 and _selectionStartEntry < _mediaEntries.size() - _layout.columns()) {
				SelectionOperation op = ReplaceSelection;
				if (event->modifiers() & Qt::ShiftModifier) {
					op = AddRangeToSelection;
				}
				selectEntry(_mediaEntries[_selectionStartEntry + _layout.columns()], op);
			}
			return;
		} else if (event->modifiers() & Qt::ControlModifier) {
			if (event->key() == Qt::Key_A) {
				selectAll();
			} else if (event->key() == Qt::Key_I) {
				invertSelection();
			}
		} else if (event->key() == Qt::Key_Delete) {
			std::vector<MediaViewEntryPtr> selected;
			for (const auto& mediaEntry: _mediaEntries) {
				if (mediaEntry->selected()) {
					selected.push_back(mediaEntry);
				}
			}
			if (selected.size() == 0) {
				return;
			}
			
			std::ostringstream question;
			question << "Are you sure you want to delete ";
			if (selected.size() == 1) {
				question << std::quoted(selected[0]->media->filename()) << " ?";
			} else {
				question << selected.size() << " medias ?";
			}
			QMessageBox::StandardButton reply = QMessageBox::question(
				this,
				"Deleted files cannot be recovered",
				question.str().c_str(),
				QMessageBox::Yes | QMessageBox::Cancel
			);
			
			if (reply == QMessageBox::Yes) {
				for (const auto& entry: selected) {
					entry->media->deleteFiles();
				}
				
				easyqt::ObjectRegistry::get<MediaShowArea>()->setMedia(nullptr);
			}
		}

		QScrollArea::keyPressEvent(event);
	}

	bool MediaView::eventFilter(QObject* watched, QEvent* event) {
		if (watched == viewport() && event->type() == QEvent::Resize) {
			updateVisibleWidgets();
		}

		return QScrollArea::eventFilter(watched, event);
	}

	void MediaView::updateVisibleWidgets() {
		_layout.setVisibleArea(QRect(
			0,
			verticalScrollBar()->value(),
			viewport()->width(),
			viewport()->height()
		));

		_layout.invalidate();
		_layout.activate();
	}
}

