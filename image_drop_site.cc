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

  filename = event->mimeData()->text().toLower().trimmed();

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

  filename = event->mimeData()->text().toLower().trimmed();

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

  QUrl url(event->mimeData()->text());
  QString filename = url.toLocalFile();

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

  if(scene()->items().size() > 0)
    scene()->removeItem(scene()->items().at(0));

  scene()->clearSelection();
}
