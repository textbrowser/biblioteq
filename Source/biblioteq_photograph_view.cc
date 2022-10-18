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

#include "biblioteq_photograph_view.h"

#include <QApplication>
#include <QGraphicsItem>
#include <QResizeEvent>
#include <QScrollBar>

biblioteq_photograph_view::biblioteq_photograph_view(QWidget *parent):
  QGraphicsView(parent)
{
  m_bestFit = true; // Agrees with default UI setting.
  m_degrees = 0.0;
  m_image = QImage();
  m_oid = -1;
}

void biblioteq_photograph_view::resizeEvent(QResizeEvent *event)
{
  QGraphicsView::resizeEvent(event);

  if(event && m_bestFit)
    {
      QGraphicsPixmapItem *item = nullptr;

      if(scene() && !scene()->items().isEmpty())
	item = qgraphicsitem_cast<QGraphicsPixmapItem *>
	  (scene()->items().at(0));

      if(item)
	{
	  auto image(m_image);

	  if(!image.isNull())
	    image = image.scaled
	      (event->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	  item->setPixmap(QPixmap::fromImage(image));
	  scene()->setSceneRect(scene()->itemsBoundingRect());
	  horizontalScrollBar()->setValue(0);
	  verticalScrollBar()->setValue(0);
	}
    }
}

void biblioteq_photograph_view::rotateImage(const qreal degrees)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  QGraphicsPixmapItem *item = nullptr;

  if(scene() && !scene()->items().isEmpty())
    item = qgraphicsitem_cast<QGraphicsPixmapItem *> (scene()->items().at(0));

  if(item)
    {
      m_degrees += degrees;

      if(m_degrees >= 360.0 || m_degrees <= -360.0)
	m_degrees = 0.0;

      item->setRotation(m_degrees);
      scene()->setSceneRect(scene()->itemsBoundingRect());
      horizontalScrollBar()->setValue(0);
      verticalScrollBar()->setValue(0);
    }

  QApplication::restoreOverrideCursor();
}

void biblioteq_photograph_view::setBestFit(const bool bestFit)
{
  m_bestFit = bestFit;
}

void biblioteq_photograph_view::setImage(const QImage &image,
					 const QString &format,
					 const qint64 oid)
{
  m_degrees = 0.0;
  m_format = format;
  m_image = image;
  m_oid = oid;
  rotateImage(m_degrees);
}

void biblioteq_photograph_view::slotRotateLeft(void)
{
  rotateImage(-90.0);
}

void biblioteq_photograph_view::slotRotateRight(void)
{
  rotateImage(90.0);
}

void biblioteq_photograph_view::slotSave(void)
{
  QGraphicsPixmapItem *item = nullptr;

  if(scene() && !scene()->items().isEmpty())
    item = qgraphicsitem_cast<QGraphicsPixmapItem *> (scene()->items().at(0));

  if(item)
    emit save
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
      (item->pixmap().toImage().transformed(QTransform().rotate(m_degrees),
					    Qt::SmoothTransformation),
#else
      (item->pixmap().toImage().transformed(QMatrix().rotate(m_degrees),
					    Qt::SmoothTransformation),
#endif
       m_format,
       m_oid);
}
