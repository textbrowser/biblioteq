#ifndef _DBENUMERATIONS_H_
#define _DBENUMERATIONS_H_

/*
** -- Qt Includes --
*/

#include <QMainWindow>

/*
** -- Local Includes --
*/

#include "ui_dbenumerations.h"

class dbenumerations: public QMainWindow
{
  Q_OBJECT

 public:
  dbenumerations(QWidget *parent);
  void show(QMainWindow *parent, const bool populate);
  void clear(void);

 private:
  Ui_dbEnumerations ui;
  void changeEvent(QEvent *event);
  void populateWidgets(void);

 private slots:
  void slotAdd(void);
  void slotSave(void);
  void slotClose(void);
  void slotReload(void);
  void slotRemove(void);
};

#endif
