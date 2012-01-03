#ifndef _BGRAPHICSSCENE_H_
#define _BGRAPHICSSCENE_H_

/*
** -- Qt Includes --
*/

#include <QGraphicsScene>

class QGraphicsSceneMouseEvent;

class bgraphicsscene: public QGraphicsScene
{
  Q_OBJECT

 public:
  bgraphicsscene(void);

 private:
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

 signals:
  void itemDoubleClicked(void);
};

#endif
