#ifndef _BIBLIOTEQ_COPY_EDITOR_BOOK_H_
#define _BIBLIOTEQ_COPY_EDITOR_BOOK_H_

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

#include "biblioteq_copy_editor.h"
#include "biblioteq_item.h"
#include "biblioteq_misc_functions.h"
#include "ui_bookcopybrowser.h"

class biblioteq_copy_editor_book: public biblioteq_copy_editor
{
  Q_OBJECT

 public:
  biblioteq_copy_editor_book(QWidget *parent, biblioteq_item *bitemArg,
			     const bool showForLendingArg,
			     const int quantityArg, const QString &ioidArg,
			     QSpinBox *spinboxArg,
			     const QFont &font,
			     const QString &uniqueIdArg);
  ~biblioteq_copy_editor_book();
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
  biblioteq_item *m_bitem;
  bool m_showForLending;
  int m_quantity;
  QString saveCopies(void);
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);

 private slots:
  void slotDeleteCopy(void);
  void slotSaveCopies(void);
};

#endif
