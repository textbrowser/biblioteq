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
  setAcceptDrops(true);
}

/*
** -- dragEnterEvent() --
*/

void image_drop_site::dragEnterEvent(QDragEnterEvent *event)
{
  QString filename = "";

  if(event == NULL)
    return;

#if defined(Q_OS_WIN)
  for(int i = 0; i < event->mimeData()->formats().size(); i++)
    if(event->mimeData()->formats()[i].toLower().contains("filename"))
      {
	filename = QString
	  (event->mimeData()->data(event->mimeData()->formats()[i])).toLower();
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

  if(event == NULL)
    return;

#if defined(Q_OS_WIN)
  for(int i = 0; i < event->mimeData()->formats().size(); i++)
    if(event->mimeData()->formats()[i].toLower().contains("filename"))
      {
	filename = QString
	  (event->mimeData()->data(event->mimeData()->formats()[i])).toLower();
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

  if(event == NULL)
    return;

#if defined(Q_OS_WIN)
  QString filename = "";

  for(int i = 0; i < event->mimeData()->formats().size(); i++)
    if(event->mimeData()->formats()[i].toLower().contains("filename"))
      {
	filename = QString
	  (event->mimeData()->data(event->mimeData()->formats()[i]));
	break;
      }
#elif defined(Q_OS_MAC)
  QString filename = event->mimeData()->urls()[0].toLocalFile();
#else
  QUrl url(event->mimeData()->text());
  QString filename = url.toLocalFile();
#endif

  image = QImage(filename);

  if(!image.isNull())
    {
      event->acceptProposedAction();

      if(scene()->items().size() > 0)
	scene()->removeItem(scene()->items().at(0));

      imageFormat = filename.mid(filename.lastIndexOf(".") + 1).toUpper();
      pixmap = QPixmap().fromImage(image).scaled
	(size() - 0.05 * size(), Qt::KeepAspectRatio);
      scene()->addPixmap(pixmap);
      scene()->items().at(0)->setFlags(QGraphicsItem::ItemIsSelectable);
    }
}

/*
** -- keyPressEvent() --
*/

void image_drop_site::keyPressEvent(QKeyEvent *event)
{
  if(event == NULL)
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
}

/*
** -- loadFromData() --
*/

void image_drop_site::loadFromData(const QByteArray &bytes)
{
  QPixmap pixmap;

  image.loadFromData(bytes);
  pixmap = QPixmap().fromImage(image).scaled
    (size() - 0.35 * size(), Qt::KeepAspectRatio);
  scene()->addPixmap(pixmap);
  scene()->items().at(0)->setFlags(QGraphicsItem::ItemIsSelectable);
}

/*
** -- mouseDoubleClickEvent() --
*/

void image_drop_site::mouseDoubleClickEvent(QMouseEvent *e)
{
  (void) e;

  while(!scene()->items().isEmpty())
    scene()->removeItem(scene()->items().first());

  scene()->addPixmap(QPixmap().fromImage(image));
  scene()->items().at(0)->setFlags(QGraphicsItem::ItemIsSelectable);
}
