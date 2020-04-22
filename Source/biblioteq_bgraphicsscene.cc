#include "biblioteq_bgraphicsscene.h"

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

biblioteq_bgraphicsscene::biblioteq_bgraphicsscene(QObject *parent):
  QGraphicsScene(parent)
{
}

biblioteq_bgraphicsscene::~biblioteq_bgraphicsscene()
{
}

void biblioteq_bgraphicsscene::keyPressEvent(QKeyEvent *event)
{
  QGraphicsScene::keyPressEvent(event);

  if(event)
    switch(event->key())
      {
      case Qt::Key_Backspace:
      case Qt::Key_Delete:
	{
	  emit deleteKeyPressed();
	  break;
	}
      case Qt::Key_Enter:
      case Qt::Key_Return:
	{
	  emit enterKeyPressed();
	  break;
	}
      default:
	{
	  break;
	}
      }
}

void biblioteq_bgraphicsscene::mouseDoubleClickEvent
(QGraphicsSceneMouseEvent *event)
{
  /*
  ** Do not issue QGraphicsScene::mouseDoubleClickEvent() as the
  ** scene may temporarily lose a future click.
  */

#if QT_VERSION < 0x050000
  if(event && itemAt(event->scenePos()))
#else
  if(event && itemAt(event->scenePos(), QTransform()))
#endif
    emit itemDoubleClicked();
}
