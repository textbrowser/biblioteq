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

static void qt_graphicsItem_highlightSelected
(QGraphicsItem *item,
 QPainter *painter,
 const QStyleOptionGraphicsItem *option)
{
  if(!item || !option || !painter)
    return;

  auto const marect = painter->transform().mapRect(QRectF(0, 0, 1, 1));

  if(qFuzzyIsNull(qMax(marect.height(), marect.width())))
    return;

  auto const mbrect = painter->transform().mapRect(item->boundingRect());

  if(qMin(mbrect.height(), mbrect.width()) < qreal(1.0))
    return;

  QColor const color(8, 255, 8); // Neon for YOU!
  QPen pen;
  const qreal pad = 0.0;

  pen.setColor(color);
  pen.setJoinStyle(Qt::RoundJoin);
  pen.setStyle(Qt::SolidLine);
  pen.setWidthF(3.5);
  painter->setBrush(Qt::NoBrush);
  painter->setPen(pen);
  painter->drawRoundedRect
    (item->boundingRect().adjusted(pad, pad, -pad, -pad), 5.0, 5.0);
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
	     QWidget *widget)
  {
    if(!option || !painter)
      {
	QGraphicsPixmapItem::paint(painter, option, widget);
	return;
      }

    QPen pen;

    pen.setColor(QColor(70, 130, 180));
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setWidthF(1.5);
    painter->setBrush(QBrush(pixmap()));
    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawRoundedRect(boundingRect(), 5.0, 5.0); // Order.

    if(option->state & (QStyle::State_HasFocus | QStyle::State_Selected))
      qt_graphicsItem_highlightSelected(this, painter, option);
  }
};

#endif
