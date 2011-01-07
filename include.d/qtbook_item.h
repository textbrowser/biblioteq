#ifndef _QTBOOK_ITEM_H_
#define _QTBOOK_ITEM_H_

/*
** -- Local Includes --
*/

#include "image_drop_site.h"

/*
** -- Qt Includes --
*/

#include <QFont>
#include <QString>
#include <QWidget>
#include <QPrinter>
#include <QKeyEvent>
#include <QMainWindow>
#include <QPrintDialog>
#include <QTextDocument>
#include <QProgressDialog>

class qtbook_item_working_dialog: public QProgressDialog
{
 public:
  qtbook_item_working_dialog(QMainWindow *parent):QProgressDialog(parent)
  {
  }

  ~qtbook_item_working_dialog()
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

class qtbook_item
{
 public:
  qtbook_item(void);
  qtbook_item(const int);
  ~qtbook_item();
  int getRow(void) const;
  int getOldQ(void) const;
  void setOldQ(const int);
  void updateRow(const int);
  void updateFont(const QFont &, QWidget *);
  QString getID(void) const;

 private:
  QMap<QString, QImage> imageValues;
  QMap<QString, QString> widgetValues;

 protected:
  int row;
  int oldq;
  bool isQueryEnabled;
  QString oid;
  QString html;
  QMainWindow *parentWid;
  bool hasDataChanged(QMainWindow *) const;
  void print(QWidget *);
  void storeData(QMainWindow *);
};

#endif
