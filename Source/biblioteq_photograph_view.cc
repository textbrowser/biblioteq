#include "biblioteq_photograph_view.h"

biblioteq_photograph_view::biblioteq_photograph_view(QWidget *parent):
  QGraphicsView(parent)
{
  m_image = QImage();
}
