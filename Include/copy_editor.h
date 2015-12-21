#ifndef _COPY_EDITOR_H_
#define _COPY_EDITOR_H_

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

#include "misc_functions.h"
#include "qtbook_item.h"
#include "ui_copybrowser.h"

class copy_editor: public QDialog
{
  Q_OBJECT

 public:
  copy_editor(QWidget *);
  copy_editor(QWidget *, qtbook_item *, const bool, const int,
	      const QString &, QSpinBox *,
	      const QFont &, const QString &, const QString &);
  virtual ~copy_editor();
  void populateCopiesEditor(void);

 private:
  class copy_class
  {
  public:
    copy_class(const QString &copyid_arg, const QString &itemoid_arg)
      {
	m_copyid = copyid_arg;
	m_itemoid = itemoid_arg;
      };

  public:
    QString m_copyid;
    QString m_itemoid;
  };

  QList<copy_class *> m_copies;
  QSpinBox *m_spinbox;
  QString m_ioid;
  QString m_itemType;
  QVector<QString> m_columnHeaderIndexes;
  QWidget *m_parent;
  Ui_copybrowser m_cb;
  bool m_showForLending;
  int m_quantity;
  qtbook_item *m_bitem;
  QString saveCopies(void);
  void changeEvent(QEvent *);
  void closeEvent(QCloseEvent *);
  void keyPressEvent(QKeyEvent *);

 protected:
  void clearCopiesList(void);
  void setGlobalFonts(const QFont &);

 protected slots:
  void slotCloseCopyEditor(void);

 private slots:
  void slotCheckoutCopy(void);
  void slotDeleteCopy(void);
  void slotSaveCopies(void);
};

#endif
