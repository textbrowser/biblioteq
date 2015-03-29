#ifndef _SRURESULTS_H_
#define _SRURESULTS_H_

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

class qtbook_magazine;

class sruresults: public QDialog
{
  Q_OBJECT

 public:
  sruresults(QWidget *, const QList<QByteArray> &, qtbook_magazine *,
	     const QFont &);
  ~sruresults();

 private:
  QList<QByteArray> m_records;
  Ui_sruResultsDialog ui;
  qtbook_magazine *magazine;
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
