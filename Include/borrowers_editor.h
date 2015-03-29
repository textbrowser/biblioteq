#ifndef _BORROWERS_EDITOR_H_
#define _BORROWERS_EDITOR_H_

/*
** -- Qt Includes --
*/

#include <QDialog>
#include <QString>
#include <QWidget>

/*
** -- Local Includes --
*/

#include "misc_functions.h"
#include "qtbook.h"
#include "qtbook_item.h"
#include "ui_borrowers.h"

class borrowers_editor: public QDialog
{
  Q_OBJECT

 public:
  borrowers_editor(QWidget *, qtbook_item *, const int, const QString &,
		   const QString &, const QFont &, const QString &,
		   const int);
  ~borrowers_editor();
  void showUsers(void);

 private:
  QString ioid;
  QString itemType;
  QVector<QString> m_columnHeaderIndexes;
  Ui_checkedOutDialog bd;
  int quantity;
  int state;
  qtbook_item *bitem;
  void changeEvent(QEvent *);
  void closeEvent(QCloseEvent *);
  void keyPressEvent(QKeyEvent *);
  void setGlobalFonts(const QFont &);

 private slots:
  void slotCloseCurrentBorrowers(void);
  void slotEraseBorrower(void);
  void slotSave(void);
};

#endif
