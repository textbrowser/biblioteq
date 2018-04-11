#ifndef _BIBLIOTEQ_GREY_LITERATURE_H_
#define _BIBLIOTEQ_GREY_LITERATURE_H_

/*
** -- Qt Includes --
*/

#include <QMainWindow>

/*
** -- Local Includes --
*/

#include "biblioteq_item.h"
#include "biblioteq_misc_functions.h"
#include "ui_biblioteq_greyliteratureinfo.h"

class biblioteq_grey_literature: public QMainWindow, public biblioteq_item
{
  Q_OBJECT

 public:
  biblioteq_grey_literature(QMainWindow *parentArg,
			    const QString &oidArg,
			    const int rowArg);
  ~biblioteq_grey_literature();
  void duplicate(const QString &p_oid, const int state);
  void insert(void);
  void modify(const int state);
  void search(const QString &field = "", const QString &value = "");

  void setPublicationDateFormat(const QString &dateFormat)
  {
    Q_UNUSED(dateFormat);
  }

  void updateWindow(const int state);

 private:
  QPalette m_te_orig_pal;
  QString m_dt_orig_ss;
  QString m_engWindowTitle;
  Ui_biblioteq_greyliteratureinfo m_ui;
  bool m_duplicate;
  bool validateWidgets(void);
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void createFile(const QByteArray &bytes,
		  const QByteArray &digest,
		  const QString &fileName) const;
  void highlightRequiredWidgets(void);
  void insertDatabase(void);
  void populateFiles(void);
  void updateDatabase(void);

 private slots:
  void slotAttachFiles(void);
  void slotCancel(void);
  void slotDeleteFiles(void);
  void slotEditFileDescription(QTableWidgetItem *item);
  void slotExportFiles(void);
  void slotGo(void);
  void slotPrint(void);
  void slotPublicationDateEnabled(bool state);
  void slotQuery(void);
  void slotReset(void);
  void slotSelectImage(void);
};

#endif
