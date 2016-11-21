#ifndef _BIBLIOTEQ_OPTIONS_H_
#define _BIBLIOTEQ_OPTIONS_H_

/*
** -- Qt Includes --
*/

#include <QMainWindow>

/*
** -- Local Includes --
*/

#include "ui_biblioteq_options.h"

class biblioteq_options: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_options(void);
  ~biblioteq_options();

 private:
  Ui_options m_ui;
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void setGlobalFonts(const QFont &font);

 private slots:
  void slotClose(void);
};

#endif
