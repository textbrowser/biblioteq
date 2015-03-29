#ifndef _QTBOOK_PHOTOGRAPHCOLLECTION_H_
#define _QTBOOK_PHOTOGRAPHCOLLECTION_H_

/*
** -- Qt Includes --
*/

#include <QDialog>
#include <QMainWindow>
#include <QMenu>
#include <QStringList>

/*
** -- Local Includes --
*/

#include "misc_functions.h"
#include "qtbook_item.h"
#include "ui_photograph.h"
#include "ui_photographinfo.h"

class bgraphicsscene;
class borrowers_editor;
class copy_editor;

class qtbook_photographcollection: public QMainWindow, public qtbook_item
{
  Q_OBJECT

 public:
  static const int PHOTOGRAPHS_PER_PAGE = 25;
  qtbook_photographcollection(QMainWindow *, const QString &,
			      const int);
  ~qtbook_photographcollection();
  void duplicate(const QString &, const int);
  void insert(void);
  void modify(const int, const QString & = "");
  void search(const QString & = "", const QString & = "");
  void updateWindow(const int);

 private:
  QDialog *photo_diag;
  QString engWindowTitle;
  QString m_itemOid;
  Ui_pcDialog pc;
  Ui_photographDialog photo;
  bgraphicsscene *scene;
  bool verifyItemFields(void);
  void changeEvent(QEvent *);
  void closeEvent(QCloseEvent *);
  void showPhotographs(const int);
  void storeData(void);

 private slots:
  void slotAddItem(void);
  void slotCancel(void);
  void slotClosePhoto(void);
  void slotDeleteItem(void);
  void slotExportPhotographs(void);
  void slotGo(void);
  void slotInsertItem(void);
  void slotModifyItem(void);
  void slotPageChanged(const QString &);
  void slotPrint(void);
  void slotQuery(void);
  void slotReset(void);
  void slotSceneSelectionChanged(void);
  void slotSelectImage(void);
  void slotUpdateItem(void);
  void slotViewContextMenu(const QPoint &);
  void slotViewPhotograph(void);
};

#endif
