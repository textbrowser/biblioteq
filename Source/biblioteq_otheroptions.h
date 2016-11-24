#ifndef _BIBLIOTEQ_OTHEROPTIONS_H_
#define _BIBLIOTEQ_OTHEROPTIONS_H_

/*
** -- Qt Includes --
*/

#include <QMainWindow>

/*
** -- Local Includes --
*/

#include "ui_biblioteq_otheroptions.h"

class biblioteq_otheroptions: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_otheroptions(void);
  ~biblioteq_otheroptions();
  QString dateFormat(const QString &itemType) const;
  void prepareSettings(void);

 private:
  Ui_otheroptions m_ui;
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
