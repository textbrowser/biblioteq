#ifndef _BIBLIOTEQ_ITEM_H_
#define _BIBLIOTEQ_ITEM_H_

#include <QFont>
#include <QKeyEvent>
#include <QMainWindow>
#include <QPointer>
#include <QPrintDialog>
#include <QPrinter>
#include <QProgressDialog>

class QPersistentModelIndex;
class biblioteq;

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
  biblioteq_item(const QModelIndex &index);
  biblioteq_item(void);
  virtual ~biblioteq_item();
  QString getID(void) const;
  int getOldQ(void) const;
  int getRow(void) const;
  void setOldQ(const int q);
  void updateQuantity(const int q);
  void updateRow(const QModelIndex &index);

 protected:
  QColor m_queryHighlightColor;
  QColor m_requiredHighlightColor;
  QMap<QString, QImage> m_imageValues;
  QMap<QString, QString> m_widgetValues;
  QPersistentModelIndex *m_index;
  QPointer<QMainWindow> m_parentWid;
  QString m_html;
  QString m_oid;
  biblioteq *qmain;
  bool m_isQueryEnabled;
  int m_oldq;
  bool hasDataChanged(QMainWindow *window) const;
  void print(QWidget *parent);
  void setQMain(QMainWindow *window);
  void setReadOnlyFields(QMainWindow *window, const bool state);
  void storeData(QMainWindow *window);
  void updateFont(const QFont &font, QWidget *window);
};

#endif
