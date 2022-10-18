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

#include "biblioteq_image_drop_site.h"
#include "biblioteq_misc_functions.h"

#include <QDropEvent>
#include <QFile>
#include <QMimeData>
#include <QUrl>

biblioteq_image_drop_site::biblioteq_image_drop_site(QWidget *parent):
  QGraphicsView(parent)
{
  m_doubleClickResizeEnabled = true;
  m_doubleclicked = false;
  m_image = QImage();
  m_imageFormat = "";
  setAcceptDrops(true);
}

QString biblioteq_image_drop_site::determineFormat
(const QByteArray &bytes) const
{
  return biblioteq_misc_functions::imageFormatGuess(bytes);
}

QString biblioteq_image_drop_site::determineFormat
(const QString &filename) const
{
  QByteArray bytes(10, 0);
  QFile file(filename);
  QString imgf("");
  qint64 bytesRead = 0;

  if(file.open(QIODevice::ReadOnly) && (bytesRead =
					file.read(bytes.data(),
						  bytes.length())) > 0)
    imgf = biblioteq_misc_functions::imageFormatGuess(bytes);

  if(imgf.isEmpty())
    {
      QString ext("");

      if(filename.contains("."))
	ext = filename.mid(filename.lastIndexOf(".") + 1);

      if(ext.isEmpty())
	imgf = "JPG";
      else
	imgf = ext.toUpper();
    }

  file.close();
  return imgf;
}

void biblioteq_image_drop_site::clear(void)
{
  m_doubleclicked = false;
  m_image = QImage();
  m_imageFormat.clear();
  scene()->clear();
  scene()->clearSelection();
}

void biblioteq_image_drop_site::dragEnterEvent(QDragEnterEvent *event)
{
  QString filename = "";

#if defined(Q_OS_WIN)
  if(event)
    for(int i = 0; i < event->mimeData()->formats().size(); i++)
      if(event->mimeData()->formats()[i].toLower().contains("filename"))
	{
	  filename = QString
	    (event->mimeData()->data(event->mimeData()->formats()[i])).
	    trimmed();
	  break;
	}
#elif defined(Q_OS_MACOS)
  if(event)
    filename = event->mimeData()->urls().value(0).toLocalFile().trimmed();
#else
  if(event)
    filename = event->mimeData()->text().trimmed();
#endif

  if(event)
    {
      auto imgf = determineFormat(filename);

      if(imgf == "BMP" ||
	 imgf == "JPG" || imgf == "JPEG" ||
	 imgf == "PNG")
	event->acceptProposedAction();
    }
}

void biblioteq_image_drop_site::dragLeaveEvent(QDragLeaveEvent *event)
{
  QGraphicsView::dragLeaveEvent(event);
}

void biblioteq_image_drop_site::dragMoveEvent(QDragMoveEvent *event)
{
  QString filename = "";

  QGraphicsView::dragMoveEvent(event);

#if defined(Q_OS_WIN)
  if(event)
    for(int i = 0; i < event->mimeData()->formats().size(); i++)
      if(event->mimeData()->formats()[i].toLower().contains("filename"))
	{
	  filename = QString
	    (event->mimeData()->data(event->mimeData()->formats()[i])).
	    trimmed();
	  break;
	}
#elif defined(Q_OS_MACOS)
  if(event)
    filename = event->mimeData()->urls().value(0).toLocalFile().trimmed();
#else
  if(event)
    filename = event->mimeData()->text().trimmed();
#endif

  if(event)
    {
      QString imgf = determineFormat(filename);

      if(imgf == "BMP" ||
	 imgf == "JPG" || imgf == "JPEG" ||
	 imgf == "PNG")
	event->acceptProposedAction();
    }
}

void biblioteq_image_drop_site::dropEvent(QDropEvent *event)
{
  QString filename("");
  QString imgf("");

#if defined(Q_OS_WIN)
  if(event)
    for(int i = 0; i < event->mimeData()->formats().size(); i++)
      if(event->mimeData()->formats()[i].toLower().contains("filename"))
	{
	  filename = QString
	    (event->mimeData()->data(event->mimeData()->formats()[i])).
	    trimmed();
	  break;
	}
#elif defined(Q_OS_MACOS)
  if(event)
    filename = event->mimeData()->urls().value(0).toLocalFile().trimmed();
#else
  if(event)
    {
      QUrl url(event->mimeData()->text());

      filename = url.toLocalFile().trimmed();
    }
#endif

  imgf = determineFormat(filename);
  m_image = QImage(filename, imgf.toLatin1().data());

  if(!m_image.isNull())
    {
      if(event)
	event->acceptProposedAction();

      m_imageFormat = imgf;
      m_doubleclicked = false;
      scene()->clear();

      QPixmap pixmap;

      if(m_image.width() > width() ||
	 m_image.height() > height())
	{
	  pixmap = QPixmap::fromImage(m_image);

	  if(!pixmap.isNull())
	    pixmap = pixmap.scaled
	      (0.50 * size(), Qt::KeepAspectRatio,
	       Qt::SmoothTransformation);
	}
      else
	pixmap = QPixmap::fromImage(m_image);

      scene()->clear();

      if(pixmap.isNull())
	pixmap = QPixmap(":/no_image.png");

      scene()->addPixmap(pixmap);

      if(acceptDrops())
	if(!scene()->items().isEmpty())
	  scene()->items().at(0)->setFlags(QGraphicsItem::ItemIsSelectable);

      scene()->setSceneRect(scene()->itemsBoundingRect());
    }
}

void biblioteq_image_drop_site::enableDoubleClickResize(const bool state)
{
  m_doubleClickResizeEnabled = state;
}

void biblioteq_image_drop_site::keyPressEvent(QKeyEvent *event)
{
  if(acceptDrops())
    if(event)
      if(event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
	if(!scene()->selectedItems().isEmpty())
	  clear();
}

void biblioteq_image_drop_site::loadFromData(const QByteArray &bytes)
{
  QPixmap pixmap;

  m_doubleclicked = false;
  m_imageFormat = determineFormat(bytes);
  m_image.loadFromData(bytes, m_imageFormat.toLatin1().data());

  if(m_image.width() > width() ||
     m_image.height() > height())
    {
      pixmap = QPixmap::fromImage(m_image);

      if(!pixmap.isNull())
	pixmap = pixmap.scaled
	  (0.50 * size(), Qt::KeepAspectRatio,
	   Qt::SmoothTransformation);
    }
  else
    pixmap = QPixmap::fromImage(m_image);

  scene()->clear();

  if(pixmap.isNull())
    pixmap = QPixmap(":/no_image.png");

  scene()->addPixmap(pixmap);

  if(acceptDrops())
    if(!scene()->items().isEmpty())
      scene()->items().at(0)->setFlags(QGraphicsItem::ItemIsSelectable);

  scene()->setSceneRect(scene()->itemsBoundingRect());
}

void biblioteq_image_drop_site::mouseDoubleClickEvent(QMouseEvent *event)
{
  QGraphicsView::mouseDoubleClickEvent(event);

  if(!m_doubleClickResizeEnabled)
    return;

  scene()->clear();

  if(!m_doubleclicked)
    {
      auto pixmap(QPixmap::fromImage(m_image));

      if(pixmap.isNull())
	pixmap = QPixmap(":/no_image.png");

      scene()->addPixmap(pixmap);
    }
  else
    {
      auto pixmap(QPixmap::fromImage(m_image));

      if(!pixmap.isNull())
	pixmap = pixmap.scaled
	  (size(), Qt::KeepAspectRatio,
	   Qt::SmoothTransformation);

      if(pixmap.isNull())
	pixmap = QPixmap(":/no_image.png");

      scene()->addPixmap(pixmap);
    }

  m_doubleclicked = !m_doubleclicked;

  if(acceptDrops())
    if(!scene()->items().isEmpty())
      scene()->items().at(0)->setFlags(QGraphicsItem::ItemIsSelectable);

  scene()->setSceneRect(scene()->itemsBoundingRect());
}

void biblioteq_image_drop_site::setImage(const QImage &image)
{
  QPixmap pixmap;

  m_doubleclicked = false;
  this->m_image = image;

  if(this->m_image.width() > width() ||
     this->m_image.height() > height())
    {
      pixmap = QPixmap::fromImage(this->m_image);

      if(!pixmap.isNull())
	pixmap = pixmap.scaled
	  (0.50 * size(), Qt::KeepAspectRatio,
	   Qt::SmoothTransformation);
    }
  else
    pixmap = QPixmap::fromImage(this->m_image);

  scene()->clear();

  if(pixmap.isNull())
    pixmap = QPixmap(":/no_image.png");

  scene()->addPixmap(pixmap);

  if(acceptDrops())
    if(!scene()->items().isEmpty())
      scene()->items().at(0)->setFlags(QGraphicsItem::ItemIsSelectable);
}

void biblioteq_image_drop_site::setReadOnly(const bool readOnly)
{
  setAcceptDrops(!readOnly);
}
