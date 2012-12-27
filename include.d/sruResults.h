#ifndef _SRURESULTS_H_
#define _SRURESULTS_H_

/*
** -- Qt Includes --
*/

#include <QDialog>
#include <QWidget>
#include <QStringList>

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
  qtbook_magazine *magazine;
  QList<QByteArray> m_records;
  Ui_sruResultsDialog ui;
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
