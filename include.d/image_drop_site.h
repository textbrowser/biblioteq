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

#include <QUrl>
#include <QKeyEvent>
#include <QDropEvent>
#include <QApplication>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QDragEnterEvent>

class image_drop_site: public QGraphicsView
{
  Q_OBJECT

 public:
  QImage image;
  QString imageFormat;
  void clear(void);
  void loadFromData(const QByteArray &);
  image_drop_site(QWidget *);

 private:
  bool doubleclicked;
  void dropEvent(QDropEvent *);
  void dragMoveEvent(QDragMoveEvent *);
  void keyPressEvent(QKeyEvent *);
  void dragEnterEvent(QDragEnterEvent *);
  void mouseDoubleClickEvent(QMouseEvent *);
  QString determineFormat(const QString &) const;
  QString determineFormat(const QByteArray &) const;
};

#endif
