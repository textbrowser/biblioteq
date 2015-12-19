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
  borrowers_editor(QWidget *parent,
		   qtbook_item *bitemArg,
		   const int quantityArg,
		   const QString &ioidArg,
		   const QString &uniqueidArg,
		   const QFont &font,
		   const QString &itemTypeArg,
		   const int stateArg);
  ~borrowers_editor();
  void showUsers(void);

 private:
  QString m_ioid;
  QString m_itemType;
  QVector<QString> m_columnHeaderIndexes;
  Ui_checkedOutDialog m_bd;
  int m_quantity;
  int m_state;
  qtbook_item *m_bitem;
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void setGlobalFonts(const QFont &font);

 private slots:
  void slotCloseCurrentBorrowers(void);
  void slotEraseBorrower(void);
  void slotSave(void);
};

#endif
