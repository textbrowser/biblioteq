#ifndef _BIBLIOTEQ_BORROWERS_EDITOR_H_
#define _BIBLIOTEQ_BORROWERS_EDITOR_H_

#include "ui_biblioteq_borrowers.h"

class biblioteq;
class biblioteq_item;

class biblioteq_borrowers_editor: public QDialog
{
  Q_OBJECT

 public:
  biblioteq_borrowers_editor(QWidget *parent,
			     biblioteq *biblioteq,
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
  biblioteq *qmain;
  biblioteq_item *m_bitem;
  int m_quantity;
  int m_state;
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
