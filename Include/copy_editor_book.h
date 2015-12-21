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
  copy_editor_book(QWidget *parent, qtbook_item *bitemArg,
		   const bool showForLendingArg,
		   const int quantityArg, const QString &ioidArg,
		   QSpinBox *spinboxArg,
		   const QFont &font,
		   const QString &uniqueIdArg);
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
      m_condition = condition_arg;
      m_copyid = copyid_arg;
      m_itemoid = itemoid_arg;
      m_originality = originality_arg;
    };

  public:
    QString m_condition;
    QString m_copyid;
    QString m_itemoid;
    QString m_originality;
  };

  QList<copy_class *> m_copies;
  QSpinBox *m_spinbox;
  QString m_ioid;
  QString m_itemType;
  QVector<QString> m_columnHeaderIndexes;
  QWidget *m_parent;
  Ui_bookcopybrowser m_cb;
  bool showForLending;
  int quantity;
  qtbook_item *bitem;
  QString saveCopies(void);
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);

 private slots:
  void slotDeleteCopy(void);
  void slotSaveCopies(void);
};

#endif
