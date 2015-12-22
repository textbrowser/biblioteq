#ifndef _BIBLIOTEQ_Z3950RESULTS_H_
#define _BIBLIOTEQ_Z3950RESULTS_H_

/*
** -- Qt Includes --
*/

#include <QDialog>
#include <QStringList>
#include <QWidget>

/*
** -- Local Includes --
*/

#include "ui_z3950results.h"

class biblioteq_magazine;

class biblioteq_z3950results: public QDialog
{
  Q_OBJECT

 public:
  biblioteq_z3950results(QWidget *,
			 QStringList &,
			 biblioteq_magazine *,
			 const QFont &,
			 const QString &);
  ~biblioteq_z3950results();

 private:
  QString m_recordSyntax;
  QStringList records;
  Ui_z3950ResultsDialog ui;
  biblioteq_magazine *magazine;
  void changeEvent(QEvent *);
  void closeEvent(QCloseEvent *);
  void keyPressEvent(QKeyEvent *);
  void setGlobalFonts(const QFont &);

 private slots:
  void slotClose(void);
  void slotSelectRecord(void);
  void slotUpdateQueryText(void);
};

#endif
