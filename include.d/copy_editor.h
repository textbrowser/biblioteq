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

#include "qtbook_item.h"
#include "misc_functions.h"
#include "ui_copybrowser.h"

class copy_editor: public QDialog
{
  Q_OBJECT

 public:
  copy_editor(QWidget *, qtbook_item *, const bool, const int,
	      const QString &, QSpinBox *,
	      const QFont &, const QString &);
  ~copy_editor();
  void populateCopiesEditor(void);

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
  QWidget *parent;
  QSpinBox *spinbox;
  qtbook_item *bitem;
  Ui_copybrowser cb;
  QHash<QString, int> m_columnHeaderIndexes;
  QList<copy_class *> copies;
  void closeEvent(QCloseEvent *);
  void changeEvent(QEvent *);
  void keyPressEvent(QKeyEvent *);
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
