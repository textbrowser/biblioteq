/*
** -- Qt Includes --
*/

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

/*
** -- Local Includes --
*/

#include "biblioteq_bgraphicsscene.h"

/*
** -- biblioteq_bgraphicsscene() --
*/

biblioteq_bgraphicsscene::biblioteq_bgraphicsscene(QObject *parent):
  QGraphicsScene(parent)
{
}

/*
** -- ~biblioteq_bgraphicsscene() --
*/

biblioteq_bgraphicsscene::~biblioteq_bgraphicsscene()
{
}

/*
** -- mouseDoubleClickEvent() --
*/

void biblioteq_bgraphicsscene::mouseDoubleClickEvent
(QGraphicsSceneMouseEvent *event)
{
  QGraphicsScene::mouseDoubleClickEvent(event);

#if QT_VERSION < 0x050000
  if(event && itemAt(event->scenePos()))
#else
  if(event && itemAt(event->scenePos(), QTransform()))
#endif
    emit itemDoubleClicked();
}

/*
** -- keyPressEvent() --
*/

void biblioteq_bgraphicsscene::keyPressEvent(QKeyEvent *event)
{
  QGraphicsScene::keyPressEvent(event);

  if(event)
    if(event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
      emit deleteKeyPressed();
}
