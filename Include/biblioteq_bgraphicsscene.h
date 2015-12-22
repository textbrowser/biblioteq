#ifndef _BIBLIOTEQ_BGRAPHICSSCENE_H_
#define _BIBLIOTEQ_BGRAPHICSSCENE_H_

/*
** -- Qt Includes --
*/

#include <QGraphicsScene>

class QGraphicsSceneMouseEvent;
class QKeyEvent;

class biblioteq_bgraphicsscene: public QGraphicsScene
{
  Q_OBJECT

 public:
  biblioteq_bgraphicsscene(QObject *parent);
  ~biblioteq_bgraphicsscene();

 private:
  void keyPressEvent(QKeyEvent *event);
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

 signals:
  void deleteKeyPressed(void);
  void itemDoubleClicked(void);
};

#endif
