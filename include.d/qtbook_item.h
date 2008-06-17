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
#include <QMainWindow>
#include <QPrintDialog>
#include <QTextDocument>

class qtbook_item
{
 public:
  qtbook_item(void);
  qtbook_item(const int);
  ~qtbook_item();
  int getRow(void);
  int getOldQ(void);
  void setOldQ(const int);
  void updateRow(const int);
  void updateFont(const QFont &, QWidget *);
  QString getID(void);

 private:
  QMap<QString, QString> widgetValues;

 protected:
  int row;
  int oldq;
  bool isQueryEnabled;
  QString oid;
  QString html;
  QMainWindow *parentWid;
  bool isDataDifferent(QMainWindow *);
  void print(QWidget *);
  void storeData(QMainWindow *);
};

#endif
