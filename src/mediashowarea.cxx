#include <cmath>

#include <QIcon>
#include <QPixmap>
#include <QtConcurrent>

#include <easyqt/objectregistry.hxx>
#include <fmt/format.h>

#include <easyqt/commands.hxx>
#include <easyqt/logging.hxx>

#include "mediashowarea.hxx"
#include "mediaview.hxx"

namespace pelican {
	MediaShowArea::GraphicsView::GraphicsView(MediaShowArea* parent): _parent(parent) {
		setScene(&_scene);
		QObject::connect(&_mediaLoadWatcher, &QFutureWatcher<void>::finished, this, &MediaShowArea::GraphicsView::showMedia);
		_imageItem = _scene.addPixmap(QPixmap());
		setFocusPolicy(Qt::ClickFocus);
		setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	}
	
	void MediaShowArea::GraphicsView::setMedia(MediaPtr media) {
		_media = media;
		_imageItem->setPixmap(QPixmap());
		if (_media) {
			QFuture<void> future = QtConcurrent::run(std::bind(&MediaShowArea::GraphicsView::loadMedia, this));
			_mediaLoadWatcher.setFuture(future);
		}
	}
	
	void MediaShowArea::GraphicsView::loadMedia() {
		_image = QPixmap(_media->path().concat(_media->suffix(".jpg")).c_str());
	}
	
	void MediaShowArea::GraphicsView::showMedia() {
		_imageItem->setPixmap(_image);
		setMediaScale(_scale);
	}
	
	void MediaShowArea::GraphicsView::scaleIncrease() {
		// Calculate actual scale when _scale is set to -1 (scale to fit), otherwise does not change the visual result
		setMediaScale(_scale);
		
		double delta;
		if (_scale >= 5) {
			delta = 1;
		} else if (_scale >= 2) {
			delta = 0.5;
		} else if (_scale > 0.25) {
			delta = 0.1;
		} else if (_scale > 0.05) {
			delta = 0.05;
		} else if (_scale >= 0.01) {
			delta = 0.01;
		}
		double newScale;
		if (_scale > 0.05) {
			newScale = (std::floor(_scale * 20) + delta * 20) / 20;
		} else {
			newScale = _scale + delta;
		}
		if (newScale > 10) {
			newScale = 10;
		}
		setMediaScale(newScale);
	}
	
	void MediaShowArea::GraphicsView::scaleDecrease() {
		// Calculate actual scale when _scale is set to -1 (scale to fit), otherwise does not change the visual result
		setMediaScale(_scale);
		
		double delta;
		if (_scale >= 5) {
			delta = 1;
		} else if (_scale >= 2) {
			delta = 0.5;
		} else if (_scale > 0.25) {
			delta = 0.1;
		} else if (_scale > 0.05) {
			delta = 0.05;
		} else if (_scale > 0.01) {
			delta = 0.01;
		}
		double newScale;
		if (_scale > 0.05) {
			newScale = (std::floor(_scale * 20) - delta * 20) / 20;
		} else {
			newScale = _scale - delta;
		}
		if (newScale < 0.01) {
			newScale = 0.01;
		}
		setMediaScale(newScale);
	}
	
	void MediaShowArea::GraphicsView::setMediaScale(double scale) {
		if (scale == -1) {
			_scale = scale;
			fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
			QSize newSize = _image.size().scaled(size(), Qt::KeepAspectRatio);
			_scale = (double)newSize.width() / _image.size().width();
		} else if (scale != _scale) {
			LOG(DEBUG, "Setting media display scale to " << scale * 100 << "%");
			_scale = scale;
			
			resetTransform();
			this->scale(_scale, _scale);
			centerOn(mapToScene(_oldCenter));
		}
		_parent->setScaleInfo(_scale);
	}
	
	void MediaShowArea::GraphicsView::keyPressEvent(QKeyEvent* event) {
		if (event->key() == Qt::Key_Escape) {
			_parent ->hide();
		} else {
			easyqt::ObjectRegistry::get<MediaShowArea>()->keyPressEvent(event);
		}
		
		QGraphicsView::keyPressEvent(event);
	}
	
	void MediaShowArea::GraphicsView::mousePressEvent(QMouseEvent* event) {
		if (event->buttons() == Qt::LeftButton) {
			_lastMousePosition = event->pos();
		} else {
			_lastMousePosition = QPoint();
		}
		
		QGraphicsView::mousePressEvent(event);
	}
	
	void MediaShowArea::GraphicsView::mouseMoveEvent(QMouseEvent* event) {
		if (event->buttons() == Qt::LeftButton) {
			QPointF oldp = mapToScene(_lastMousePosition);
			QPointF newp = mapToScene(event->pos());
			QPointF delta = newp - oldp;
			translate(delta.x(), delta.y());
			_lastMousePosition = event->pos();
		}
		
		_oldCenter = rect().center();
		
		QGraphicsView::mouseMoveEvent(event);
	}
	
	MediaShowArea::MediaShowArea():
			_view(this),
			_scaleDecreaseAction(QIcon::fromTheme("zoom-out"), "Zoom out"),
			_scaleIncreaseAction(QIcon::fromTheme("zoom-in"), "Zoom in"),
			_scaleFitAction(QIcon::fromTheme("zoom-fit-best"), "Fit in view"),
			_scaleOriginalSizeAction(QIcon::fromTheme("zoom-original"), "Original size")
	{
	
		setLayout(&_layout);
		
		_scaleResetButton.setFlat(true);
		_scaleResetButton.setText("--- %");
		QObject::connect(&_scaleResetButton, &QPushButton::clicked, [this](bool checked) {
			easyqt::getCommand("mediashowarea-scale-fit")->execute();
		});
		
		QObject::connect(&_scaleDecreaseAction, &QAction::triggered, [this](bool checked) {
			easyqt::getCommand("mediashowarea-scale-decrease")->execute();
		});
		QObject::connect(&_scaleIncreaseAction, &QAction::triggered, [this](bool checked) {
			easyqt::getCommand("mediashowarea-scale-increase")->execute();
		});
		_toolBar.addAction(&_scaleDecreaseAction);
		_toolBar.addWidget(&_scaleResetButton);
		_toolBar.addAction(&_scaleIncreaseAction);
		
		QObject::connect(&_scaleFitAction, &QAction::triggered, [this](bool checked) {
			easyqt::getCommand("mediashowarea-scale-fit")->execute();
		});
		_toolBar.addAction(&_scaleFitAction);
		
		QObject::connect(&_scaleOriginalSizeAction, &QAction::triggered, [this](bool checked) {
			easyqt::getCommand("mediashowarea-scale-original-size")->execute();
		});
		_toolBar.addAction(&_scaleOriginalSizeAction);
		
		_layout.addWidget(&_toolBar);
		_layout.addWidget(&_view);
	}
	
	void MediaShowArea::setScaleInfo(double scale) {
		_scaleResetButton.setText(fmt::format("   {: 4.0f} %   ", scale * 100).c_str());
		if (scale == 0.01) {
			_scaleDecreaseAction.setEnabled(false);
		} else if (scale == 10) {
			_scaleIncreaseAction.setEnabled(false);
		} else {
			_scaleDecreaseAction.setEnabled(true);
			_scaleIncreaseAction.setEnabled(true);
		}
	}
}

