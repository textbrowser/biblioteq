#ifndef _BIBLIOTEQ_IMAGE_DROP_SITE_H_
#define _BIBLIOTEQ_IMAGE_DROP_SITE_H_

/*
** -- C++ Includes --
*/

#include <iostream>

using namespace std;

/*
** -- Qt Includes --
*/

#include <QApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QUrl>

class biblioteq_image_drop_site: public QGraphicsView
{
  Q_OBJECT

 public:
  QImage m_image;
  QString m_imageFormat;
  biblioteq_image_drop_site(QWidget *parent);
  void clear(void);
  void enableDoubleClickResize(const bool state);
  void loadFromData(const QByteArray &bytes);
  void setImage(const QImage &image);
  void setReadOnly(const bool readOnly);

 private:
  bool m_doubleClickResizeEnabled;
  bool m_doubleclicked;
  QString determineFormat(const QByteArray &bytes) const;
  QString determineFormat(const QString &filename) const;
  void dragEnterEvent(QDragEnterEvent *event);
  void dragLeaveEvent(QDragLeaveEvent *event);
  void dragMoveEvent(QDragMoveEvent *event);
  void dropEvent(QDropEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif
