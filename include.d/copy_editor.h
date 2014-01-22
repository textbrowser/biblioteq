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
  copy_editor(QWidget *);
  copy_editor(QWidget *, qtbook_item *, const bool, const int,
	      const QString &, QSpinBox *,
	      const QFont &, const QString &, const QString &);
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
  QWidget *m_parent;
  QSpinBox *spinbox;
  qtbook_item *bitem;
  Ui_copybrowser cb;
  QVector<QString> m_columnHeaderIndexes;
  QList<copy_class *> copies;
  QString saveCopies(void);
  void closeEvent(QCloseEvent *);
  void changeEvent(QEvent *);
  void keyPressEvent(QKeyEvent *);

 protected:
  void clearCopiesList(void);
  void setGlobalFonts(const QFont &);

 protected slots:
  void slotCloseCopyEditor(void);

 private slots:
  void slotDeleteCopy(void);
  void slotSaveCopies(void);
  void slotCheckoutCopy(void);
};

#endif
