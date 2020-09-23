#include "biblioteq_photograph_view.h"

#include <QApplication>
#include <QGraphicsItem>
#include <QResizeEvent>
#include <QScrollBar>

biblioteq_photograph_view::biblioteq_photograph_view(QWidget *parent):
  QGraphicsView(parent)
{
  m_bestFit = true; // Agrees with default UI setting.
  m_degrees = 0.0;
  m_image = QImage();
  m_oid = -1;
}

void biblioteq_photograph_view::resizeEvent(QResizeEvent *event)
{
  QGraphicsView::resizeEvent(event);

  if(event && m_bestFit)
    {
      QGraphicsPixmapItem *item = nullptr;

      if(scene() && !scene()->items().isEmpty())
	item = qgraphicsitem_cast<QGraphicsPixmapItem *>
	  (scene()->items().at(0));

      if(item)
	{
	  QImage image(m_image);

	  if(!image.isNull())
	    image = image.scaled
	      (event->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	  item->setPixmap(QPixmap::fromImage(image));
	  scene()->setSceneRect(scene()->itemsBoundingRect());
	  horizontalScrollBar()->setValue(0);
	  verticalScrollBar()->setValue(0);
	}
    }
}

void biblioteq_photograph_view::rotateImage(const qreal degrees)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  QGraphicsPixmapItem *item = nullptr;

  if(scene() && !scene()->items().isEmpty())
    item = qgraphicsitem_cast<QGraphicsPixmapItem *> (scene()->items().at(0));

  if(item)
    {
      m_degrees += degrees;

      if(m_degrees >= 360.0 || m_degrees <= -360.0)
	m_degrees = 0.0;

      item->setRotation(m_degrees);
      scene()->setSceneRect(scene()->itemsBoundingRect());
      horizontalScrollBar()->setValue(0);
      verticalScrollBar()->setValue(0);
    }

  QApplication::restoreOverrideCursor();
}

void biblioteq_photograph_view::setBestFit(const bool bestFit)
{
  m_bestFit = bestFit;
}

void biblioteq_photograph_view::setImage(const QImage &image,
					 const QString &format,
					 const qint64 oid)
{
  m_degrees = 0.0;
  m_format = format;
  m_image = image;
  m_oid = oid;
  rotateImage(m_degrees);
}

void biblioteq_photograph_view::slotRotateLeft(void)
{
  rotateImage(-90.0);
}

void biblioteq_photograph_view::slotRotateRight(void)
{
  rotateImage(90.0);
}

void biblioteq_photograph_view::slotSave(void)
{
  QGraphicsPixmapItem *item = nullptr;

  if(scene() && !scene()->items().isEmpty())
    item = qgraphicsitem_cast<QGraphicsPixmapItem *> (scene()->items().at(0));

  if(item)
    emit save
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
      (item->pixmap().toImage().transformed(QTransform().rotate(m_degrees),
					    Qt::SmoothTransformation),
#else
      (item->pixmap().toImage().transformed(QMatrix().rotate(m_degrees),
					    Qt::SmoothTransformation),
#endif
       m_format,
       m_oid);
}
