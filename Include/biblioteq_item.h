#ifndef _BIBLIOTEQ_ITEM_H_
#define _BIBLIOTEQ_ITEM_H_

/*
** -- Local Includes --
*/

#include "biblioteq_image_drop_site.h"

/*
** -- Qt Includes --
*/

#include <QFont>
#include <QKeyEvent>
#include <QMainWindow>
#include <QPrintDialog>
#include <QPrinter>
#include <QProgressDialog>
#include <QString>
#include <QTextDocument>
#include <QWidget>

class biblioteq_item_working_dialog: public QProgressDialog
{
  Q_OBJECT

 public:
  biblioteq_item_working_dialog(QMainWindow *parent):QProgressDialog(parent)
  {
  }

  ~biblioteq_item_working_dialog()
  {
  }

 protected:
  void closeEvent(QCloseEvent *e)
  {
    if(e)
      e->ignore();
  }

  void keyPressEvent(QKeyEvent *e)
  {
    if(e && e->key() == Qt::Key_Escape)
      e->ignore();
    else
      QProgressDialog::keyPressEvent(e);
  }
};

class biblioteq_item
{
 public:
  biblioteq_item(const int);
  biblioteq_item(void);
  virtual ~biblioteq_item();
  QString getID(void) const;
  int getOldQ(void) const;
  int getRow(void) const;
  void setOldQ(const int);
  void updateFont(const QFont &, QWidget *);
  void updateRow(const int);

 protected:
  QMainWindow *parentWid;
  QMap<QString, QImage> imageValues;
  QMap<QString, QString> widgetValues;
  QString html;
  QString oid;
  bool isQueryEnabled;
  int oldq;
  int row;
  bool hasDataChanged(QMainWindow *) const;
  void print(QWidget *);
  void storeData(QMainWindow *);
};

#endif
