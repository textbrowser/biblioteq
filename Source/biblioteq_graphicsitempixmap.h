/*
** Copyright (c) 2006 - present, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from BiblioteQ without specific prior written permission.
**
** BIBLIOTEQ IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** BIBLIOTEQ, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _BIBLIOTEQ_GRAPHICSITEMPIXMAP_H_
#define _BIBLIOTEQ_GRAPHICSITEMPIXMAP_H_

#include <QGraphicsDropShadowEffect>
#include <QGraphicsPixmapItem>

static void qt_graphicsItem_highlightSelected
(QGraphicsItem *item,
 QPainter *painter,
 const QStyleOptionGraphicsItem *option)
{
  Q_UNUSED(option);

  if(!item || !painter)
    return;

  QPainterPath path;
  auto rect(item->boundingRect());
  const qreal offset = 5.0;

  rect.setHeight(offset + rect.height());
  rect.setWidth(offset + rect.width());
  rect.setX(-offset + rect.x());
  rect.setY(-offset + rect.y());
  path.addRoundedRect(rect, 5.0, 5.0);
  painter->fillPath(path, QColor(222, 141, 174, 100)); // Sassy Pink
}

class biblioteq_graphicsitempixmap: public QGraphicsPixmapItem
{
 public:
  biblioteq_graphicsitempixmap(const QPixmap &pixmap, QGraphicsItem *parent):
    QGraphicsPixmapItem(pixmap, parent)
  {
    auto effect = new QGraphicsDropShadowEffect();

    effect->setBlurRadius(10.0);
    effect->setColor(QColor(Qt::gray));
    effect->setOffset(2.5, 2.5);
    setGraphicsEffect(effect);
  }

  ~biblioteq_graphicsitempixmap()
  {
  }

  void paint(QPainter *painter,
	     const QStyleOptionGraphicsItem *option,
	     QWidget *widget)
  {
    if(!option || !painter)
      {
	QGraphicsPixmapItem::paint(painter, option, widget);
	return;
      }

    painter->setRenderHints(QPainter::Antialiasing |
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
			    QPainter::LosslessImageRendering |
#endif
			    QPainter::SmoothPixmapTransform |
			    QPainter::TextAntialiasing,
			    true);

    QPen pen;

    pen.setColor(Qt::transparent);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setWidthF(0.0);
    painter->setBrush(QBrush(pixmap()));
    painter->setPen(pen);
    painter->drawRoundedRect(boundingRect(), 5.0, 5.0); // Order.

    if(option->state & (QStyle::State_HasFocus | QStyle::State_Selected))
      qt_graphicsItem_highlightSelected(this, painter, option);
  }
};

#endif
