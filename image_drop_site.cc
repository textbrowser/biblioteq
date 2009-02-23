/*
** -- Local Includes --
*/

#include "qtbook_book.h"
#include "image_drop_site.h"

/*
** -- image_drop_site() --
*/

image_drop_site::image_drop_site(QWidget *parent)
{
  (void) parent;
  doubleclicked = false;
  setAcceptDrops(true);
}

/*
** -- dragEnterEvent() --
*/

void image_drop_site::dragEnterEvent(QDragEnterEvent *event)
{
  QString filename = "";

  if(event == 0)
    return;

#if defined(Q_OS_WIN)
  for(int i = 0; i < event->mimeData()->formats().size(); i++)
    if(event->mimeData()->formats()[i].toLower().contains("filename"))
      {
	filename = QString
	  (event->mimeData()->data(event->mimeData()->formats()[i])).
	  toLower().trimmed();
	break;
      }
#elif defined(Q_OS_MAC)
  filename = event->mimeData()->urls()[0].toLocalFile().toLower().trimmed();
#else
  filename = event->mimeData()->text().toLower().trimmed();
#endif

  if(filename.endsWith(".bmp") || filename.endsWith(".jpg") ||
     filename.endsWith(".jpeg") || filename.endsWith(".png"))
    event->acceptProposedAction();
}

/*
** -- dragMoveEvent() --
*/

void image_drop_site::dragMoveEvent(QDragMoveEvent *event)
{
  QString filename = "";

  if(event == 0)
    return;

#if defined(Q_OS_WIN)
  for(int i = 0; i < event->mimeData()->formats().size(); i++)
    if(event->mimeData()->formats()[i].toLower().contains("filename"))
      {
	filename = QString
	  (event->mimeData()->data(event->mimeData()->formats()[i])).
	  toLower().trimmed();
	break;
      }
#elif defined(Q_OS_MAC)
  filename = event->mimeData()->urls()[0].toLocalFile().toLower().trimmed();
#else
  filename = event->mimeData()->text().toLower().trimmed();
#endif

  if(filename.endsWith(".bmp") || filename.endsWith(".jpg") ||
     filename.endsWith(".jpeg") || filename.endsWith(".png"))
    event->acceptProposedAction();
}

/*
** -- dropEvent() --
*/

void image_drop_site::dropEvent(QDropEvent *event)
{
  QPixmap pixmap;

  if(event == 0)
    return;

#if defined(Q_OS_WIN)
  QString filename = "";

  for(int i = 0; i < event->mimeData()->formats().size(); i++)
    if(event->mimeData()->formats()[i].toLower().contains("filename"))
      {
	filename = QString
	  (event->mimeData()->data(event->mimeData()->formats()[i])).trimmed();
	break;
      }
#elif defined(Q_OS_MAC)
  QString filename = event->mimeData()->urls()[0].toLocalFile().trimmed();
#else
  QUrl url(event->mimeData()->text());
  QString filename = url.toLocalFile().trimmed();
#endif

  if(filename.toLower().endsWith(".bmp"))
    imageFormat = "BMP";
  else if(filename.toLower().endsWith(".jpg") ||
	  filename.toLower().endsWith(".jpeg")) 
    imageFormat = "JPG";
  else if(filename.toLower().endsWith(".png"))
    imageFormat = "PNG";
  else // Guess!
    imageFormat = "JPG";

  image = QImage(filename, imageFormat.toAscii().data());

  if(!image.isNull())
    {
      event->acceptProposedAction();
      doubleclicked = false;

      if(scene()->items().size() > 0)
	scene()->removeItem(scene()->items().at(0));

      if(image.width() > width() ||
	 image.height() > height())
	pixmap = QPixmap().fromImage(image).scaled
	  (size() - 0.05 * size(), Qt::KeepAspectRatio,
	   Qt::SmoothTransformation);
      else
	pixmap = QPixmap().fromImage(image);

      scene()->addPixmap(pixmap);
      scene()->items().at(0)->setFlags(QGraphicsItem::ItemIsSelectable);
    }
}

/*
** -- keyPressEvent() --
*/

void image_drop_site::keyPressEvent(QKeyEvent *event)
{
  if(event == 0)
    return;

  if((event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) &&
     !scene()->selectedItems().isEmpty())
    clear();
}

/*
** -- clear() --
*/

void image_drop_site::clear(void)
{
  image = QImage();
  imageFormat = "";
  doubleclicked = false;

  while(!scene()->items().isEmpty())
    scene()->removeItem(scene()->items().first());

  scene()->clearSelection();
}

/*
** -- determineFormat() --
*/

void image_drop_site::determineFormat(const QByteArray &bytes)
{
  if(bytes.size() >= 4 && bytes[1] == 'P' && bytes[2] == 'N' &&
     bytes[3] == 'G')
    imageFormat = "PNG";
  else if(bytes.size() >= 10 && bytes[6] == 'J' && bytes[7] == 'F' &&
	  bytes[8] == 'I' && bytes[9] == 'F')
    imageFormat = "JPG";
  else if(bytes.size() >= 2 && bytes[0] == 'B' && bytes[1] == 'M')
    imageFormat = "BMP";
  else // Guess!
    imageFormat = "JPG";
}

/*
** -- loadFromData() --
*/

void image_drop_site::loadFromData(const QByteArray &bytes)
{
  QPixmap pixmap;

  doubleclicked = false;
  determineFormat(bytes);
  image.loadFromData(bytes, imageFormat.toAscii().data());

  if(image.width() > width() ||
     image.height() > height())
    pixmap = QPixmap().fromImage(image).scaled
      (size() - 0.05 * size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
  else
    pixmap = QPixmap().fromImage(image);

  scene()->addPixmap(pixmap);
  scene()->items().at(0)->setFlags(QGraphicsItem::ItemIsSelectable);
}

/*
** -- mouseDoubleClickEvent() --
*/

void image_drop_site::mouseDoubleClickEvent(QMouseEvent *e)
{
  QPixmap pixmap;

  (void) e;

  if(image.width() < width() && image.height() < height())
    return;

  while(!scene()->items().isEmpty())
    scene()->removeItem(scene()->items().first());

  if(!doubleclicked)
    scene()->addPixmap(QPixmap().fromImage(image));
  else
    {
      pixmap = QPixmap().fromImage(image).scaled
	(size() - 0.05 * size(), Qt::KeepAspectRatio,
	 Qt::SmoothTransformation);
      scene()->addPixmap(pixmap);
    }

  doubleclicked = !doubleclicked;
  scene()->items().at(0)->setFlags(QGraphicsItem::ItemIsSelectable);
}
