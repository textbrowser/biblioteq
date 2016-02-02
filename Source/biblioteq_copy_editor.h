#ifndef _BIBLIOTEQ_COPY_EDITOR_H_
#define _BIBLIOTEQ_COPY_EDITOR_H_

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

#include "biblioteq_item.h"
#include "biblioteq_misc_functions.h"
#include "ui_copybrowser.h"

class biblioteq_copy_editor: public QDialog
{
  Q_OBJECT

 public:
  biblioteq_copy_editor(QWidget *parent);
  biblioteq_copy_editor(QWidget *parent, biblioteq_item *bitemArg,
			const bool showForLendingArg,
			const int quantityArg, const QString &ioidArg,
			QSpinBox *spinboxArg,
			const QFont &font,
			const QString &itemTypeArg,
			const QString &uniqueIdArg);
  virtual ~biblioteq_copy_editor();
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
  biblioteq_item *m_bitem;
  bool m_showForLending;
  int m_quantity;
  QString saveCopies(void);
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);

 protected:
  void clearCopiesList(void);
  void setGlobalFonts(const QFont &font);

 protected slots:
  void slotCloseCopyEditor(void);

 private slots:
  void slotCheckoutCopy(void);
  void slotDeleteCopy(void);
  void slotSaveCopies(void);
};

#endif
