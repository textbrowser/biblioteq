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
  image = QImage();
  imageFormat = "";
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
	  (event->mimeData()->data(event->mimeData()->formats()[i])).trimmed();
	break;
      }
#elif defined(Q_OS_MAC)
  filename = event->mimeData()->urls()[0].toLocalFile().trimmed();
#else
  filename = event->mimeData()->text().trimmed();
#endif

  QString imgf = determineFormat(filename).toLower();

  if(imgf == "bmp" ||
     imgf == "jpg" || imgf == "jpeg" ||
     imgf == "png")
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
	  (event->mimeData()->data(event->mimeData()->formats()[i])).trimmed();
	break;
      }
#elif defined(Q_OS_MAC)
  filename = event->mimeData()->urls()[0].toLocalFile().trimmed();
#else
  filename = event->mimeData()->text().trimmed();
#endif

  QString imgf = determineFormat(filename).toLower();

  if(imgf == "bmp" ||
     imgf == "jpg" || imgf == "jpeg" ||
     imgf == "png")
    event->acceptProposedAction();
}

/*
** -- dropEvent() --
*/

void image_drop_site::dropEvent(QDropEvent *event)
{
  QPixmap pixmap;
  QString imgf("");

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

  imgf = determineFormat(filename).toLower();
  image = QImage(filename, imgf.toAscii().data());

  if(!image.isNull())
    {
      event->acceptProposedAction();
      imageFormat = imgf;
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
  doubleclicked = false;

  while(!scene()->items().isEmpty())
    scene()->removeItem(scene()->items().first());

  scene()->clearSelection();
}

/*
** -- determineFormat() --
*/

QString image_drop_site::determineFormat(const QByteArray &bytes)
{
  QString imgf("");

  if(bytes.size() >= 4 && bytes[1] == 'P' && bytes[2] == 'N' &&
     bytes[3] == 'G')
    imgf = "PNG";
  else if(bytes.size() >= 10 && bytes[6] == 'J' && bytes[7] == 'F' &&
	  bytes[8] == 'I' && bytes[9] == 'F')
    imgf = "JPG";
  else if(bytes.size() >= 2 && bytes[0] == 'B' && bytes[1] == 'M')
    imgf = "BMP";
  else // Guess!
    imgf = "JPG";

  return imgf;
}

/*
** -- determineFormat() --
*/

QString image_drop_site::determineFormat(const QString &filename)
{
  char bytes[10];
  QFile file(filename);
  qint64 bytesRead = 0;
  QString imgf("");

  (void) memset(bytes, 0, sizeof(bytes));

  if(file.open(QIODevice::ReadOnly) && (bytesRead =
					file.read(bytes, sizeof(bytes))) > 0)
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
      QString ext(filename.mid(filename.lastIndexOf(".") + 1));

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

void image_drop_site::loadFromData(const QByteArray &bytes)
{
  QPixmap pixmap;

  doubleclicked = false;
  imageFormat = determineFormat(bytes);
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
