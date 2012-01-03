/*
** -- Qt Includes --
*/

#include <QGraphicsSceneMouseEvent>

/*
** -- Local Includes --
*/

#include "bgraphicsscene.h"

/*
** -- bgraphicsscene() --
*/

bgraphicsscene::bgraphicsscene(void):QGraphicsScene(0)
{
}

/*
** -- mouseDoubleClickEvent() --
*/

void bgraphicsscene::mouseDoubleClickEvent
(QGraphicsSceneMouseEvent *mouseEvent)
{
  QGraphicsScene::mouseDoubleClickEvent(mouseEvent);

  if(itemAt(mouseEvent->scenePos()))
    emit itemDoubleClicked();
}
