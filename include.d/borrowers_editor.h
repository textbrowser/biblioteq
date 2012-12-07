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

#include "qtbook.h"
#include "qtbook_item.h"
#include "ui_borrowers.h"
#include "misc_functions.h"

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
  int state;
  int quantity;
  QString ioid;
  QString itemType;
  qtbook_item *bitem;
  QVector<QString> m_columnHeaderIndexes;
  Ui_checkedOutDialog bd;
  void closeEvent(QCloseEvent *);
  void changeEvent(QEvent *);
  void keyPressEvent(QKeyEvent *);
  void setGlobalFonts(const QFont &);

 private slots:
  void slotSave(void);
  void slotEraseBorrower(void);
  void slotCloseCurrentBorrowers(void);
};

#endif
