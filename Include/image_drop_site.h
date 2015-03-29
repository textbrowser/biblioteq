#ifndef _IMAGE_DROP_SITE_H_
#define _IMAGE_DROP_SITE_H_

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

class image_drop_site: public QGraphicsView
{
  Q_OBJECT

 public:
  QImage image;
  QString imageFormat;
  image_drop_site(QWidget *);
  void clear(void);
  void loadFromData(const QByteArray &);
  void setImage(const QImage &);
  void setReadOnly(const bool);

 private:
  bool doubleclicked;
  QString determineFormat(const QByteArray &) const;
  QString determineFormat(const QString &) const;
  void dragEnterEvent(QDragEnterEvent *);
  void dragLeaveEvent(QDragLeaveEvent *);
  void dragMoveEvent(QDragMoveEvent *);
  void dropEvent(QDropEvent *);
  void keyPressEvent(QKeyEvent *);
  void mouseDoubleClickEvent(QMouseEvent *);
};

#endif
