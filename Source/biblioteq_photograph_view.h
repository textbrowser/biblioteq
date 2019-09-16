#ifndef _BIBLIOTEQ_PHOTOGRAPH_VIEW_H_
#define _BIBLIOTEQ_PHOTOGRAPH_VIEW_H_

#include <QGraphicsView>

class biblioteq_photograph_view: public QGraphicsView
{
  Q_OBJECT

 public:
  biblioteq_photograph_view(QWidget *parent);

 private:
  QImage m_image;
};

#endif
