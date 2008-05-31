#ifndef _COPY_EDITOR_H_
#define _COPY_EDITOR_H_

/*
** -- Qt Includes --
*/

#include <QDialog>
#include <QString>
#include <QSpinBox>
#include <QMainWindow>
#include <QMessageBox>

/*
** -- Local Includes --
*/

#include "qtbook.h"
#include "qtbook_item.h"
#include "misc_functions.h"
#include "ui_copybrowser.h"

class copy_editor: QDialog
{
  Q_OBJECT

 public:
  copy_editor(QWidget *, qtbook_item *, const bool, const int,
	      const QString &, const QString &, QSpinBox *,
	      const QFont &, const QString &);
  ~copy_editor();
  void populateCopiesEditor(void);
  virtual void closeEvent(QCloseEvent *);

 private:
  class copy_class
  {
  public:
    copy_class(const QString &copyid_arg, const QString &itemoid_arg)
      {
	copyid = copyid_arg;
	itemoid = itemoid_arg;
      };
    
  public:
    QString copyid;
    QString itemoid;
  };

  int quantity;
  bool showForLending;
  QString ioid;
  QString itemType;
  QString uniqueid;
  QWidget *parent;
  QSpinBox *spinbox;
  qtbook_item *bitem;
  Ui_copybrowser cb;
  QList<copy_class *> copies;
  void setGlobalFonts(const QFont &);
  void clearCopiesList(void);
  QString saveCopies(void);

 private slots:
  void slotDeleteCopy(void);
  void slotSaveCopies(void);
  void slotCheckoutCopy(void);
  void slotCloseCopyEditor(void);
};

#endif
