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
  dbenumerations(void);
  void show(QMainWindow *parent, const bool populate);

 private:
  Ui_dbEnumerations ui;
  void populateWidgets(void);

 private slots:
  void slotAdd(void);
  void slotSave(void);
  void slotClose(void);
  void slotReload(void);
  void slotRemove(void);
};

#endif
