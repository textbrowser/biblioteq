#ifndef _BIBLIOTEQ_DBENUMERATIONS_H_
#define _BIBLIOTEQ_DBENUMERATIONS_H_

#include "ui_biblioteq_dbenumerations.h"

class biblioteq;

class biblioteq_dbenumerations: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_dbenumerations(biblioteq *parent);
  void clear(void);
  void closeEvent(QCloseEvent *event);
  void show(QMainWindow *parent, const bool populate);

 private:
  QHash<QWidget *, QStringList> m_listData;
  QHash<QWidget *, QMap<QString, QString> > m_tableData;
  Ui_dbEnumerations m_ui;
  biblioteq *qmain;
  void changeEvent(QEvent *event);
  void populateWidgets(void);
  void saveData(QHash<QWidget *,
		QStringList> &listData,
		QHash<QWidget *, QMap<QString, QString> > &tableData);

 private slots:
  void slotAdd(void);
  void slotClose(void);
  void slotReload(void);
  void slotRemove(void);
  void slotSave(void);
};

#endif
