#ifndef _BIBLIOTEQ_BORROWERS_EDITOR_H_
#define _BIBLIOTEQ_BORROWERS_EDITOR_H_

/*
** -- Qt Includes --
*/

#include <QDialog>
#include <QString>
#include <QWidget>

/*
** -- Local Includes --
*/

#include "biblioteq.h"
#include "biblioteq_item.h"
#include "biblioteq_misc_functions.h"
#include "ui_borrowers.h"

class biblioteq_borrowers_editor: public QDialog
{
  Q_OBJECT

 public:
  biblioteq_borrowers_editor(QWidget *parent,
			     biblioteq_item *bitemArg,
			     const int quantityArg,
			     const QString &ioidArg,
			     const QString &uniqueidArg,
			     const QFont &font,
			     const QString &itemTypeArg,
			     const int stateArg);
  ~biblioteq_borrowers_editor();
  void showUsers(void);

 private:
  QString m_ioid;
  QString m_itemType;
  QVector<QString> m_columnHeaderIndexes;
  Ui_checkedOutDialog m_bd;
  int m_quantity;
  int m_state;
  biblioteq_item *m_bitem;
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
