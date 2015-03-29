#ifndef _BGRAPHICSSCENE_H_
#define _BGRAPHICSSCENE_H_

/*
** -- Qt Includes --
*/

#include <QGraphicsScene>

class QGraphicsSceneMouseEvent;
class QKeyEvent;

class bgraphicsscene: public QGraphicsScene
{
  Q_OBJECT

 public:
  bgraphicsscene(QObject *parent);
  ~bgraphicsscene();

 private:
  void keyPressEvent(QKeyEvent *);
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);

 signals:
  void deleteKeyPressed(void);
  void itemDoubleClicked(void);
};

#endif
