#ifndef _BIBLIOTEQ_GRAPHICSITEMPIXMAP_H_
#define _BIBLIOTEQ_GRAPHICSITEMPIXMAP_H_

static void qt_graphicsItem_highlightSelected
(QGraphicsItem *item,
 QPainter *painter,
 const QStyleOptionGraphicsItem *option)
{
  if(!item || !option || !painter)
    return;

  const QRectF murect = painter->transform().mapRect(QRectF(0, 0, 1, 1));

  if(qFuzzyIsNull(qMax(murect.width(), murect.height())))
    return;

  const QRectF mbrect = painter->transform().mapRect(item->boundingRect());

  if(qMin(mbrect.width(), mbrect.height()) < qreal(1.0))
    return;

  const QColor bgcolor(70, 130, 180);
  const qreal pad = 0.0;
  const qreal penWidth = 2.5;

  painter->setBrush(Qt::NoBrush);
  painter->setPen(QPen(bgcolor, penWidth, Qt::SolidLine));
  painter->drawRect(item->boundingRect().adjusted(pad, pad, -pad, -pad));
}

class biblioteq_graphicsitempixmap: public QGraphicsPixmapItem
{
 public:
  biblioteq_graphicsitempixmap(const QPixmap &pixmap, QGraphicsItem *parent):
    QGraphicsPixmapItem(pixmap, parent)
  {
  }

  ~biblioteq_graphicsitempixmap()
  {
  }

  void paint(QPainter *painter,
	     const QStyleOptionGraphicsItem *option,
	     QWidget *widget = 0)
  {
    Q_UNUSED(widget);

    if(!option || !painter)
      return;

    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    QRectF exposed_rect(option->exposedRect.adjusted(-1, -1, 1, 1));

    exposed_rect &= QRectF(offset().x(),
			   offset().y(),
			   pixmap().width(),
			   pixmap().height());
    painter->drawPixmap
      (exposed_rect, pixmap(), exposed_rect.translated(-offset()));

    if(option->state & (QStyle::State_Selected | QStyle::State_HasFocus))
      qt_graphicsItem_highlightSelected(this, painter, option);
  }
};

#endif
