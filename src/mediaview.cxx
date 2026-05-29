#include <QImage>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QRect>
#include <QRubberBand>
#include <QSize>
#include <QScrollBar>
#include <QThreadPool>

#include <easyqt/logging.hxx>
#include <easyqt/objectregistry.hxx>

#include "application.hxx"
#include "mediainfopane.hxx"
#include "mediashowarea.hxx"
#include "mediaview.hxx"

#define THUMBNAIL_SIZE 128
#define HOVER_COLOR "#bcf"
#define SELECTED_COLOR "#8af"

namespace pelican {
	MediaViewEntry::MediaViewEntry(MediaPtr media):
			_media(media)
	{
		setLayout(&_layout);
		setFixedWidth(THUMBNAIL_SIZE + 20);
		setFocusPolicy(Qt::NoFocus);
		
		_thumbnailLabel.setFixedSize(THUMBNAIL_SIZE, THUMBNAIL_SIZE);
		_thumbnailLabel.setPixmap(QIcon::fromTheme("image-jpg").pixmap(THUMBNAIL_SIZE, THUMBNAIL_SIZE));
		_layout.addWidget(&_thumbnailLabel, 0, 0);
		_layout.setAlignment(&_thumbnailLabel, Qt::AlignCenter);
		_nameLabel.setText(media->filename().c_str());
		_layout.addWidget(&_nameLabel, 1, 0);
		_layout.setAlignment(&_nameLabel, Qt::AlignCenter);
		
		QThreadPool::globalInstance()->start(std::bind(&MediaViewEntry::showThumbnail, this));
	}
	
	void MediaViewEntry::showThumbnail() {
		_thumbnailLabel.setPixmap(_media->thumbnail(THUMBNAIL_SIZE, THUMBNAIL_SIZE));
	}
	
	void MediaViewEntry::deleteFiles() {
		bool success = true;
		for (const auto& path: _media->paths()) {
			LOG(INFO, "Deleting " << std::quoted(path.string()));
			success &= std::filesystem::remove(path);
		}
		if (!success) {
			LOG(ERROR, "Some files could not be deleted");
		}
	}
	
	void MediaViewEntry::paintEvent(QPaintEvent *event) {
		QPainter painter;
		painter.begin(this);
		
		if (_selected) {
			painter.fillRect(rect(), QColor(SELECTED_COLOR));
		} else if (underMouse()) {
			painter.fillRect(rect(), QColor(HOVER_COLOR));
		}
		painter.end(); 
		
		QWidget::paintEvent(event);
	}
	
	void MediaViewEntry::mousePressEvent(QMouseEvent* event) {
		if (event->button() == Qt::LeftButton) {
			MediaView::SelectionOperation op = MediaView::ReplaceSelection;
			if (event->modifiers() & Qt::ShiftModifier) {
				op = MediaView::AddRangeToSelection;
			} else if (event->modifiers() & Qt::ControlModifier) {
				if (_selected) {
					op = MediaView::RemoveFromSelection;
				} else {
					op = MediaView::AddToSelection;
				}
			}
			easyqt::ObjectRegistry::get<MediaView>()->selectEntry(this, op);
		}
	}
	
	void MediaViewEntry::mouseReleaseEvent(QMouseEvent* event) {}
	
	void MediaViewEntry::mouseDoubleClickEvent(QMouseEvent* event) {
		if (event->modifiers() == Qt::NoModifier) {
			easyqt::ObjectRegistry::get<MediaShowArea>()->show();
			easyqt::ObjectRegistry::get<MediaShowArea>()->setMedia(_media);
		}
	}
	
	void MediaViewEntry::enterEvent(QEvent* event) {
		update();
	}
	
	void MediaViewEntry::leaveEvent(QEvent* event) {
		update();
	}
	
	void MediaViewEntry::setSelected(bool selected) {
		_selected = selected;
		update();
	}
	
	MediaView::MediaView() {
		QWidget* widget = new QWidget(this);
		widget->setLayout(&_layout);
		setWidget(widget);
		_rubberBand = new QRubberBand(QRubberBand::Rectangle, widget);
		
		setFocusPolicy(Qt::ClickFocus);
	}
	
	void MediaView::rebuild() {
		auto it = std::ranges::find(_mediaEntries, _selectionStartEntry);
		int i = -1;
		if (it != _mediaEntries.end()) {
			i = std::distance(_mediaEntries.begin(), it);
		}

		while (QLayoutItem* item = _layout.takeAt(0)) {
			if (QWidget* widget = item->widget()) {
				widget->deleteLater();
			}
			delete item;
		}
		_mediaEntries.clear();

		for (const auto& media: Application::instance()->medias()) {
			MediaViewEntry* mediaEntry = new MediaViewEntry(media);
			_mediaEntries.push_back(mediaEntry); 
			_layout.addWidget(mediaEntry);
			mediaEntry->show();
		}
		
		if (i >= 0 and i < _mediaEntries.size()) {
			selectEntry(_mediaEntries[i], ReplaceSelection);
			ensureWidgetVisible(_mediaEntries[i], 0, 0);
		} else {
			clearSelection();
		}
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
	
	void MediaView::selectEntry(MediaViewEntry* entry, SelectionOperation op) {
		if (op == ReplaceSelection) {
			clearSelection();
			entry->setSelected(true);
			_selectionStartEntry = entry;
			ensureWidgetVisible(entry, 0, 0);
			if (easyqt::ObjectRegistry::get<MediaShowArea>()->isVisible()) {
				easyqt::ObjectRegistry::get<MediaShowArea>()->setMedia(entry->media());
			}
		} else if (op == AddToSelection) {
			entry->setSelected(true);
			_selectionStartEntry = entry;
			ensureWidgetVisible(entry, 0, 0);
		} else if (op == RemoveFromSelection) {
			entry->setSelected(false);
			_selectionStartEntry = nullptr;
		} else if (op == AddRangeToSelection) {
			if (!_selectionStartEntry) {
				selectEntry(entry, ReplaceSelection);
				return;
			}
			
			auto entryIt = std::find(_mediaEntries.begin(), _mediaEntries.end(), entry);
			auto selectionStartEntryIt = std::find(_mediaEntries.begin(), _mediaEntries.end(), _selectionStartEntry);
			clearSelection();
			if (selectionStartEntryIt != entryIt) {
				if (selectionStartEntryIt > entryIt) {
					std::swap(selectionStartEntryIt, entryIt);
				}
  				for (; selectionStartEntryIt != entryIt; selectionStartEntryIt++) {
					(*selectionStartEntryIt)->setSelected(true);
					ensureWidgetVisible(*selectionStartEntryIt, 0, 0);
				}
			}
			_selectionStartEntry->setSelected(true);
			ensureWidgetVisible(_selectionStartEntry, 0, 0);
			entry->setSelected(true);
			ensureWidgetVisible(entry, 0, 0);
		}
		if (_selectionStartEntry) {
			easyqt::ObjectRegistry::get<MediaInfoPane>()->setMedia(_selectionStartEntry->media());
		} else {
			easyqt::ObjectRegistry::get<MediaInfoPane>()->setMedia(nullptr);
		}
	}
	
	void MediaView::selectAll() {
		for (const auto& mediaEntry: _mediaEntries) {
			mediaEntry->setSelected(true);
		}
		_selectionStartEntry = nullptr;
		easyqt::ObjectRegistry::get<MediaInfoPane>()->setMedia(nullptr);
	}
	
	void MediaView::invertSelection() {
		std::vector<MediaViewEntry*> selected;
		for (const auto& mediaEntry: _mediaEntries) {
			mediaEntry->setSelected(!mediaEntry->selected());
			if (mediaEntry->selected()) {
				selected.push_back(mediaEntry);
			}
		}
		if (selected.size() == 1) {
			easyqt::ObjectRegistry::get<MediaInfoPane>()->setMedia(selected[0]->media());
			_selectionStartEntry = selected[0];
			ensureWidgetVisible(_selectionStartEntry, 0, 0);
		} else {
			easyqt::ObjectRegistry::get<MediaInfoPane>()->setMedia(nullptr);
			_selectionStartEntry = nullptr;
		}
	}
	
	void MediaView::clearSelection() {
		for (const auto& mediaEntry: _mediaEntries) {
			mediaEntry->setSelected(false);
		}
		easyqt::ObjectRegistry::get<MediaInfoPane>()->setMedia(nullptr);
	}
	
	void MediaView::resizeEvent(QResizeEvent* event) {
		easyqt::ScrollArea::resizeEvent(event);
		
		if (_selectionStartEntry) {
			ensureWidgetVisible(_selectionStartEntry, 0, 0);
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
			for (auto it = _mediaEntries.begin(); it != _mediaEntries.end(); it++) {
				selected = (*it)->geometry().intersects(_rubberBand->geometry());
				(*it)->setSelected(selected);
				if (selected) {
					ensureWidgetVisible(*it, 0, 0);
				}
			}
		}
		
		QScrollArea::mouseMoveEvent(event);
	}

	void MediaView::mouseReleaseEvent(QMouseEvent* event) {
		_rubberBand->hide();
		QScrollArea::mouseReleaseEvent(event);
	}
	
	void MediaView::keyPressEvent(QKeyEvent* event) {
		if (event->key() == Qt::Key_Left) {
			auto it = std::ranges::find(_mediaEntries, _selectionStartEntry);
			if (it != _mediaEntries.end() and it != _mediaEntries.begin()) {
				if (event->modifiers() == Qt::NoModifier) {
					selectEntry(*(--it), ReplaceSelection);
				} else if (event->modifiers() & Qt::ShiftModifier) {
					selectEntry(*(--it), AddToSelection);
				}
			}
			return;
		} else if (event->key() == Qt::Key_Right) {
			auto it = std::ranges::find(_mediaEntries, _selectionStartEntry);
			if (it < --_mediaEntries.end()) {
				if (event->modifiers() == Qt::NoModifier) {
					selectEntry(*(++it), ReplaceSelection);
				} else if (event->modifiers() & Qt::ShiftModifier) {
					selectEntry(*(++it), AddToSelection);
				}
			}
			return;
		} else if (event->key() == Qt::Key_Up) {
			auto it = std::ranges::find(_mediaEntries, _selectionStartEntry);
			if (std::distance(_mediaEntries.begin(), it) >= _layout.columns()) {
				std::advance(it, -_layout.columns());
				if (event->modifiers() == Qt::NoModifier) {
					selectEntry(*it, ReplaceSelection);
				} else if (event->modifiers() & Qt::ShiftModifier) {
					selectEntry(*it, AddRangeToSelection);
				}
			}
			return;
		} else if (event->key() == Qt::Key_Down) {
			auto it = std::ranges::find(_mediaEntries, _selectionStartEntry);
			if (std::distance(it, _mediaEntries.end()) > _layout.columns()) {
				std::advance(it, _layout.columns());
				if (event->modifiers() == Qt::NoModifier) {
					selectEntry(*it, ReplaceSelection);
				} else if (event->modifiers() & Qt::ShiftModifier) {
					selectEntry(*it, AddRangeToSelection);
				}
			}
			return;
		} else if (event->modifiers() & Qt::ControlModifier) {
			if (event->key() == Qt::Key_A) {
				selectAll();
			} else if (event->key() == Qt::Key_I) {
				invertSelection();
			}
		} else if (event->key() == Qt::Key_Delete) {
			std::vector<MediaViewEntry*> selected;
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
				question << std::quoted(selected[0]->media()->filename()) << " ?";
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
					entry->deleteFiles();
				}
				
				easyqt::ObjectRegistry::get<MediaShowArea>()->setMedia(nullptr);
			}
		}

		QScrollArea::keyPressEvent(event);
	}
}

