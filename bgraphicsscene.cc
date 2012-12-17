/*
** -- Qt Includes --
*/

#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>

/*
** -- Local Includes --
*/

#include "bgraphicsscene.h"

/*
** -- bgraphicsscene() --
*/

bgraphicsscene::bgraphicsscene(QObject *parent):QGraphicsScene(parent)
{
}

/*
** -- ~bgraphicsscene() --
*/

bgraphicsscene::~bgraphicsscene()
{
}

/*
** -- mouseDoubleClickEvent() --
*/

void bgraphicsscene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsScene::mouseDoubleClickEvent(event);

  if(event && itemAt(event->scenePos()))
    emit itemDoubleClicked();
}

/*
** -- keyPressEvent() --
*/

void bgraphicsscene::keyPressEvent(QKeyEvent *event)
{
  QGraphicsScene::keyPressEvent(event);

  if(event && event->key() == Qt::Key_Delete)
    emit deleteKeyPressed();
}
