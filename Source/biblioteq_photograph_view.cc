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
}

void biblioteq_photograph_view::resizeEvent(QResizeEvent *event)
{
  QGraphicsView::resizeEvent(event);

  if(event && m_bestFit)
    {
      QGraphicsPixmapItem *item = 0;

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

  QGraphicsPixmapItem *item = 0;

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

void biblioteq_photograph_view::setImage(const QImage &image)
{
  m_degrees = 0.0;
  m_image = image;
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
