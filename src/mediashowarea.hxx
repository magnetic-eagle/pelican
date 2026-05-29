#ifndef PELICAN_MEDIASHOWAREА_HXX
#define PELICAN_MEDIASHOWAREА_HXX

#include <cmath>

#include <QBoxLayout>
#include <QFutureWatcher>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QLabel>
#include <QPoint>
#include <QPushButton>
#include <QResizeEvent>
#include <QToolBar>

#include <easyqt/object.hxx>

#include "media.hxx"

namespace pelican {
	class MediaShowArea: public easyqt::Object<QWidget> {
		Q_OBJECT
		public:
			class GraphicsView: public QGraphicsView {
				public:
					GraphicsView(MediaShowArea* parent);
			
					void setMedia(MediaPtr media);
					void setMediaScale(double value);
					
					void scaleIncrease();
					void scaleDecrease();
					inline void scaleFit() {
						setMediaScale(-1);
					}
					inline void scaleOriginalSize() {
						setMediaScale(1);
					}
				
					virtual void keyPressEvent(QKeyEvent* event) override;
					virtual void mousePressEvent(QMouseEvent* event) override;
					virtual void mouseMoveEvent(QMouseEvent* event) override;
				
				protected:
					void loadMedia();
					void showMedia();
				
				private:
					MediaShowArea* _parent;
					MediaPtr _media;
					QGraphicsScene _scene;
					QPixmap _image;
					QGraphicsPixmapItem* _imageItem = nullptr;
					QFutureWatcher<void> _mediaLoadWatcher;
					double _scale = {-1};
					QPoint _lastMousePosition;
					QPoint _oldCenter;
			};
			
			MediaShowArea();
			
			inline void setMedia(MediaPtr media) {
				_media = media;
				_view.setMedia(media);
			};
			inline void setMediaScale(double value) { _view.setMediaScale(value); };
			
			inline void scaleIncrease() { _view.scaleIncrease(); };
			inline void scaleDecrease() { _view.scaleDecrease(); };
			inline void scaleFit() { _view.scaleFit(); };
			inline void scaleOriginalSize() { _view.scaleOriginalSize(); };
			
			void setScaleInfo(double scale);
		
		private:
			MediaPtr _media;
			QToolBar _toolBar;
			QPushButton _scaleResetButton;
			QAction _scaleDecreaseAction, _scaleIncreaseAction, _scaleFitAction, _scaleOriginalSizeAction;
			QVBoxLayout _layout;
			GraphicsView _view;
	};
}

#endif

