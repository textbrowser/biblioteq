#ifndef _BIBLIOTEQ_PDFREADER_H_
#define _BIBLIOTEQ_PDFREADER_H_

/*
** -- Qt Includes --
*/

#include <QMainWindow>

/*
** -- Local Includes --
*/

#include "ui_biblioteq_pdfreader.h"

class biblioteq_pdfreader: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_pdfreader(void);
  ~biblioteq_pdfreader();

 private:
  Ui_pdfreader m_ui;
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void setGlobalFonts(const QFont &font);

 private slots:
  void slotClose(void);

 public slots:
  void showNormal(void);
};

#endif
