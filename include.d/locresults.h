#ifndef _LOCRESULTS_H_
#define _LOCRESULTS_H_

/*
** -- Qt Includes --
*/

#include <QDialog>
#include <QWidget>
#include <QStringList>

/*
** -- Local Includes --
*/

#include "ui_locresults.h"
#include "qtbook_magazine.h"

class qtbook_magazine;

class locresults: QDialog
{
  Q_OBJECT

 public:
  locresults(QWidget *, QStringList &, qtbook_magazine *, const QFont &);
  ~locresults();

 private:
  QStringList records;
  qtbook_magazine *magazine;
  Ui_locResultsDialog ui;
  void setGlobalFonts(const QFont &);

 protected:
  void closeEvent(QCloseEvent *);

 private slots:
  void slotClose(void);
  void slotSelectRecord(void);
  void slotUpdateQueryText(void);
};

#endif
