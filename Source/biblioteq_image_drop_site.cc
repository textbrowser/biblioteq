/*
** -- Qt Includes --
*/

#include <QMimeData>
#include <QtDebug>

/*
** -- Local Includes --
*/

#include "biblioteq_book.h"
#include "biblioteq_image_drop_site.h"

/*
** -- biblioteq_image_drop_site() --
*/

biblioteq_image_drop_site::biblioteq_image_drop_site(QWidget *parent):
  QGraphicsView(parent)
{
  m_doubleClickResizeEnabled = true;
  m_doubleclicked = false;
  m_image = QImage();
  m_imageFormat = "";
  setAcceptDrops(true);
}

/*
** -- dragEnterEvent() --
*/

void biblioteq_image_drop_site::dragEnterEvent(QDragEnterEvent *event)
{
  QString filename = "";

#if defined(Q_OS_WIN32)
  if(event)
    for(int i = 0; i < event->mimeData()->formats().size(); i++)
      if(event->mimeData()->formats()[i].toLower().contains("filename"))
	{
	  filename = QString
	    (event->mimeData()->data(event->mimeData()->formats()[i])).
	    trimmed();
	  break;
	}
#elif defined(Q_OS_MAC)
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

/*
** -- dragMoveEvent() --
*/

void biblioteq_image_drop_site::dragMoveEvent(QDragMoveEvent *event)
{
  QString filename = "";

  QGraphicsView::dragMoveEvent(event);

#if defined(Q_OS_WIN32)
  if(event)
    for(int i = 0; i < event->mimeData()->formats().size(); i++)
      if(event->mimeData()->formats()[i].toLower().contains("filename"))
	{
	  filename = QString
	    (event->mimeData()->data(event->mimeData()->formats()[i])).
	    trimmed();
	  break;
	}
#elif defined(Q_OS_MAC)
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

/*
** -- dropEvent() --
*/

void biblioteq_image_drop_site::dropEvent(QDropEvent *event)
{
  QString imgf("");
  QString filename("");

#if defined(Q_OS_WIN32)
  if(event)
    for(int i = 0; i < event->mimeData()->formats().size(); i++)
      if(event->mimeData()->formats()[i].toLower().contains("filename"))
	{
	  filename = QString
	    (event->mimeData()->data(event->mimeData()->formats()[i])).
	    trimmed();
	  break;
	}
#elif defined(Q_OS_MAC)
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

      if(scene()->items().size() > 0)
	scene()->removeItem(scene()->items().at(0));

      QPixmap pixmap;

      if(m_image.width() > width() ||
	 m_image.height() > height())
	{
	  pixmap = QPixmap().fromImage(m_image);

	  if(!pixmap.isNull())
	    pixmap = pixmap.scaled
	      (0.50 * size(), Qt::KeepAspectRatio,
	       Qt::SmoothTransformation);
	}
      else
	pixmap = QPixmap().fromImage(m_image);

      while(!scene()->items().isEmpty())
	scene()->removeItem(scene()->items().first());

      scene()->addPixmap(pixmap);

      if(acceptDrops())
	if(scene()->items().size() > 0)
	  scene()->items().at(0)->setFlags(QGraphicsItem::ItemIsSelectable);
    }
}

/*
** -- keyPressEvent() --
*/

void biblioteq_image_drop_site::keyPressEvent(QKeyEvent *event)
{
  if(acceptDrops())
    if(event)
      if(event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
	if(!scene()->selectedItems().isEmpty())
	  clear();
}

/*
** -- clear() --
*/

void biblioteq_image_drop_site::clear(void)
{
  m_doubleclicked = false;
  m_image = QImage();
  m_imageFormat.clear();

  while(!scene()->items().isEmpty())
    scene()->removeItem(scene()->items().first());

  scene()->clearSelection();
}

/*
** -- determineFormat() --
*/

QString biblioteq_image_drop_site::determineFormat
(const QByteArray &bytes) const
{
  QString imgf("");

  if(bytes.size() >= 4 &&
     tolower(bytes[1]) == 'p' &&
     tolower(bytes[2]) == 'n' &&
     tolower(bytes[3]) == 'g')
    imgf = "PNG";
  else if(bytes.size() >= 10 &&
	  tolower(bytes[6]) == 'j' && tolower(bytes[7]) == 'f' &&
	  tolower(bytes[8]) == 'i' && tolower(bytes[9]) == 'f')
    imgf = "JPG";
  else if(bytes.size() >= 2 &&
	  tolower(bytes[0]) == 'b' &&
	  tolower(bytes[1]) == 'm')
    imgf = "BMP";
  else // Guess!
    imgf = "JPG";

  return imgf;
}

/*
** -- determineFormat() --
*/

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
    {
      if(bytesRead >= 4 && bytes[1] == 'P' && bytes[2] == 'N' &&
	 bytes[3] == 'G')
	imgf = "PNG";
      else if(bytesRead >= 10 && bytes[6] == 'J' && bytes[7] == 'F' &&
	      bytes[8] == 'I' && bytes[9] == 'F')
	imgf = "JPG";
      else if(bytesRead >= 2 && bytes[0] == 'B' && bytes[1] == 'M')
	imgf = "BMP";
    }

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

/*
** -- loadFromData() --
*/

void biblioteq_image_drop_site::loadFromData(const QByteArray &bytes)
{
  QPixmap pixmap;

  m_doubleclicked = false;
  m_imageFormat = determineFormat(bytes);
  m_image.loadFromData(bytes, m_imageFormat.toLatin1().data());

  if(m_image.width() > width() ||
     m_image.height() > height())
    {
      pixmap = QPixmap().fromImage(m_image);

      if(!pixmap.isNull())
	pixmap = pixmap.scaled
	  (0.50 * size(), Qt::KeepAspectRatio,
	   Qt::SmoothTransformation);
    }
  else
    pixmap = QPixmap().fromImage(m_image);

  while(!scene()->items().isEmpty())
    scene()->removeItem(scene()->items().first());

  scene()->addPixmap(pixmap);

  if(acceptDrops())
    if(scene()->items().size() > 0)
      scene()->items().at(0)->setFlags(QGraphicsItem::ItemIsSelectable);
}

/*
** -- mouseDoubleClickEvent() --
*/

void biblioteq_image_drop_site::mouseDoubleClickEvent(QMouseEvent *event)
{
  QGraphicsView::mouseDoubleClickEvent(event);

  if(!m_doubleClickResizeEnabled)
    return;

  while(!scene()->items().isEmpty())
    scene()->removeItem(scene()->items().first());

  if(!m_doubleclicked)
    scene()->addPixmap(QPixmap().fromImage(m_image));
  else
    {
      QPixmap pixmap(QPixmap().fromImage(m_image));

      if(!pixmap.isNull())
	pixmap = pixmap.scaled
	  (size(), Qt::KeepAspectRatio,
	   Qt::SmoothTransformation);

      scene()->addPixmap(pixmap);
    }

  m_doubleclicked = !m_doubleclicked;

  if(acceptDrops())
    if(scene()->items().size() > 0)
      scene()->items().at(0)->setFlags(QGraphicsItem::ItemIsSelectable);
}

/*
** -- setReadOnly() --
*/

void biblioteq_image_drop_site::setReadOnly(const bool readOnly)
{
  setAcceptDrops(!readOnly);
}

/*
** -- setImage() --
*/

void biblioteq_image_drop_site::setImage(const QImage &image)
{
  QPixmap pixmap;

  m_doubleclicked = false;
  this->m_image = image;

  if(this->m_image.width() > width() ||
     this->m_image.height() > height())
    {
      pixmap = QPixmap().fromImage(this->m_image);

      if(!pixmap.isNull())
	pixmap = pixmap.scaled
	  (0.50 * size(), Qt::KeepAspectRatio,
	   Qt::SmoothTransformation);
    }
  else
    pixmap = QPixmap().fromImage(this->m_image);

  while(!scene()->items().isEmpty())
    scene()->removeItem(scene()->items().first());

  scene()->addPixmap(pixmap);

  if(acceptDrops())
    if(scene()->items().size() > 0)
      scene()->items().at(0)->setFlags(QGraphicsItem::ItemIsSelectable);
}

/*
** -- dragLeaveEvent() --
*/

void biblioteq_image_drop_site::dragLeaveEvent(QDragLeaveEvent *event)
{
  QGraphicsView::dragLeaveEvent(event);
}

/*
** -- enableDoubleClickResize() --
*/

void biblioteq_image_drop_site::enableDoubleClickResize(const bool state)
{
  m_doubleClickResizeEnabled = state;
}
