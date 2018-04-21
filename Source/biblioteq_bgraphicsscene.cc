/*
** -- Qt Includes --
*/

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

/*
** -- Local Includes --
*/

#include "biblioteq_bgraphicsscene.h"

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
    if(event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete)
      emit deleteKeyPressed();
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
