#ifndef _BIBLIOTEQ_GRAPHICSITEMPIXMAP_H_
#define _BIBLIOTEQ_GRAPHICSITEMPIXMAP_H_

static void qt_graphicsItem_highlightSelected
(QGraphicsItem *item, QPainter *painter,
 const QStyleOptionGraphicsItem *option)
{
  const QRectF murect = painter->transform().mapRect(QRectF(0, 0, 1, 1));

  if(qFuzzyIsNull(qMax(murect.width(), murect.height())))
    return;

  const QRectF mbrect = painter->transform().mapRect(item->boundingRect());

  if(qMin(mbrect.width(), mbrect.height()) < qreal(1.0))
    return;

  const QColor bgcolor(70, 130, 180);
  const qreal pad = 0.0;
  const qreal penWidth = 10.0;

  painter->setPen(QPen(bgcolor, penWidth, Qt::SolidLine));
  painter->setBrush(Qt::NoBrush);
  painter->drawRect(item->boundingRect().adjusted(pad, pad, -pad, -pad));
  painter->setPen(QPen(option->palette.windowText(), 0, Qt::SolidLine));
  painter->setBrush(Qt::NoBrush);
  painter->drawRect(item->boundingRect().adjusted(pad, pad, -pad, -pad));
}

class biblioteq_graphicsitempixmap: public QGraphicsPixmapItem
{
public:
  biblioteq_graphicsitempixmap(const QPixmap &pixmap,
			       QGraphicsItem *parent):
    QGraphicsPixmapItem(pixmap, parent)
  {
  }

  ~biblioteq_graphicsitempixmap()
  {
  }

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	     QWidget *widget = 0)
  {
    QGraphicsPixmapItem::paint(painter, option, widget);

    if(option)
      if(option->state & (QStyle::State_Selected | QStyle::State_HasFocus))
	qt_graphicsItem_highlightSelected(this, painter, option);
  }
};

#endif
