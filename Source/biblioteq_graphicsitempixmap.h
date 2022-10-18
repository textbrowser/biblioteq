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

  const auto &murect = painter->transform().mapRect(QRectF(0, 0, 1, 1));

  if(qFuzzyIsNull(qMax(murect.width(), murect.height())))
    return;

  const auto &mbrect = painter->transform().mapRect(item->boundingRect());

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
	     QWidget *widget = nullptr)
  {
    Q_UNUSED(widget);

    if(!option || !painter)
      return;

    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    auto exposed_rect(option->exposedRect.adjusted(-1, -1, 1, 1));

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
