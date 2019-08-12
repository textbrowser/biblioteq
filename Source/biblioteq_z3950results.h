#ifndef _BIBLIOTEQ_Z3950RESULTS_H_
#define _BIBLIOTEQ_Z3950RESULTS_H_

#include "ui_biblioteq_z3950results.h"

class biblioteq_magazine;

class biblioteq_z3950results: public QDialog
{
  Q_OBJECT

 public:
  biblioteq_z3950results(QWidget *parent,
			 QStringList &list,
			 biblioteq_magazine *magazine_arg,
			 const QFont &font,
			 const QString &recordSyntax);
  ~biblioteq_z3950results();

 private:
  QString m_recordSyntax;
  QStringList m_records;
  Ui_z3950ResultsDialog m_ui;
  biblioteq_magazine *m_magazine;
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void setGlobalFonts(const QFont &font);

 private slots:
  void slotClose(void);
  void slotSelectRecord(void);
  void slotUpdateQueryText(void);
};

#endif
