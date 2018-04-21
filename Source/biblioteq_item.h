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
  void closeEvent(QCloseEvent *event)
  {
    if(event)
      event->ignore();
  }

  void keyPressEvent(QKeyEvent *event)
  {
    if(event && event->key() == Qt::Key_Escape)
      event->ignore();
    else
      QProgressDialog::keyPressEvent(event);
  }
};

class biblioteq_item
{
 public:
  biblioteq_item(const int rowArg);
  biblioteq_item(void);
  virtual ~biblioteq_item();
  QString getID(void) const;
  int getOldQ(void) const;
  int getRow(void) const;
  void setOldQ(const int q);
  void updateFont(const QFont &font, QWidget *window);
  void updateRow(const int rowArg);

 protected:
  QMainWindow *m_parentWid;
  QMap<QString, QImage> m_imageValues;
  QMap<QString, QString> m_widgetValues;
  QString m_html;
  QString m_oid;
  bool m_isQueryEnabled;
  int m_oldq;
  int m_row;
  bool hasDataChanged(QMainWindow *window) const;
  void print(QWidget *parent);
  void setReadOnlyFields(QMainWindow *window, const bool state);
  void storeData(QMainWindow *window);
};

#endif
