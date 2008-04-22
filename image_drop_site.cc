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

#ifdef Q_OS_WIN
  for(int i = 0; i < event->mimeData()->formats().size(); i++)
    if(event->mimeData()->formats()[i].toLower().contains("filename"))
      {
	filename = QString
	  (event->mimeData()->data(event->mimeData()->formats()[i])).toLower();
	break;
      }
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

#ifdef Q_OS_WIN
  for(int i = 0; i < event->mimeData()->formats().size(); i++)
    if(event->mimeData()->formats()[i].toLower().contains("filename"))
      {
	filename = QString
	  (event->mimeData()->data(event->mimeData()->formats()[i])).toLower();
	break;
      }
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
  if(event == NULL)
    return;

#ifdef Q_OS_WIN
  QString filename = "";

  for(int i = 0; i < event->mimeData()->formats().size(); i++)
    if(event->mimeData()->formats()[i].toLower().contains("filename"))
      {
	filename = QString
	  (event->mimeData()->data(event->mimeData()->formats()[i]));
	break;
      }
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
      scene()->addPixmap(QPixmap().fromImage(image));
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
