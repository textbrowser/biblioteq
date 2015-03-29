#ifndef _COPY_EDITOR_BOOK_H_
#define _COPY_EDITOR_BOOK_H_

/*
** -- Qt Includes --
*/

#include <QDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QSpinBox>
#include <QString>

/*
** -- Local Includes --
*/

#include "copy_editor.h"
#include "misc_functions.h"
#include "qtbook_item.h"
#include "ui_bookcopybrowser.h"

class copy_editor_book: public copy_editor
{
  Q_OBJECT

 public:
  copy_editor_book(QWidget *, qtbook_item *, const bool, const int,
		   const QString &, QSpinBox *,
		   const QFont &, const QString &);
  ~copy_editor_book();
  void populateCopiesEditor(void);

 private:
  class copy_class
  {
  public:
    copy_class(const QString &condition_arg,
	       const QString &copyid_arg,
	       const QString &itemoid_arg,
	       const QString &originality_arg)
      {
	condition = condition_arg;
	copyid = copyid_arg;
	itemoid = itemoid_arg;
	originality = originality_arg;
      };

  public:
    QString condition;
    QString copyid;
    QString itemoid;
    QString originality;
  };

  QList<copy_class *> copies;
  QSpinBox *spinbox;
  QString ioid;
  QString itemType;
  QVector<QString> m_columnHeaderIndexes;
  QWidget *parent;
  Ui_bookcopybrowser cb;
  bool showForLending;
  int quantity;
  qtbook_item *bitem;
  QString saveCopies(void);
  void changeEvent(QEvent *);
  void closeEvent(QCloseEvent *);
  void keyPressEvent(QKeyEvent *);

 private slots:
  void slotDeleteCopy(void);
  void slotSaveCopies(void);
};

#endif
