#ifndef _BIBLIOTEQ_SRURESULTS_H_
#define _BIBLIOTEQ_SRURESULTS_H_

/*
** -- Qt Includes --
*/

#include <QDialog>
#include <QStringList>
#include <QWidget>

/*
** -- Local Includes --
*/

#include "ui_sruResults.h"

class biblioteq_magazine;

class biblioteq_sruresults: public QDialog
{
  Q_OBJECT

 public:
  biblioteq_sruresults(QWidget *, const QList<QByteArray> &,
		       biblioteq_magazine *, const QFont &);
  ~biblioteq_sruresults();

 private:
  QList<QByteArray> m_records;
  Ui_sruResultsDialog ui;
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
