#ifndef _Z3950RESULTS_H_
#define _Z3950RESULTS_H_

/*
** -- Qt Includes --
*/

#include <QDialog>
#include <QWidget>
#include <QStringList>

/*
** -- Local Includes --
*/

#include "ui_z3950results.h"

class qtbook_magazine;

class z3950results: public QDialog
{
  Q_OBJECT

 public:
  z3950results(QWidget *, QStringList &, qtbook_magazine *, const QFont &);
  ~z3950results();

 private:
  QStringList records;
  qtbook_magazine *magazine;
  Ui_z3950ResultsDialog ui;
  void keyPressEvent(QKeyEvent *);
  void setGlobalFonts(const QFont &);
  void closeEvent(QCloseEvent *);
  void changeEvent(QEvent *);

 private slots:
  void slotClose(void);
  void slotSelectRecord(void);
  void slotUpdateQueryText(void);
};

#endif
