#ifndef _BIBLIOTEQ_DVD_H_
#define _BIBLIOTEQ_DVD_H_

#include "biblioteq_item.h"
#include "ui_biblioteq_dvdinfo.h"

class biblioteq_dvd: public QMainWindow, public biblioteq_item
{
  Q_OBJECT

 public:
  biblioteq_dvd(biblioteq *parentArg,
		const QString &oidArg,
		const QModelIndex &index);
  ~biblioteq_dvd();
  void duplicate(const QString &p_oid, const int state);
  void insert(void);
  void modify(const int state);
  void search(const QString &field = "", const QString &value = "");

  void setPublicationDateFormat(const QString &dateFormat)
  {
    dvd.release_date->setDisplayFormat(dateFormat);
  }

  void updateWindow(const int state);

 private:
  QString m_engWindowTitle;
  Ui_dvdDialog dvd;
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);

 private slots:
  void slotCancel(void);
  void slotGo(void);
  void slotPopulateCopiesEditor(void);
  void slotPrint(void);
  void slotPublicationDateEnabled(bool state);
  void slotQuery(void);
  void slotReset(void);
  void slotSelectImage(void);
  void slotShowUsers(void);
};

#endif
