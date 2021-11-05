#ifndef _BIBLIOTEQ_DOCUMENTATIONWINDOW_H_
#define _BIBLIOTEQ_DOCUMENTATIONWINDOW_H_

#include <QMainWindow>

#include "ui_biblioteq_documentationwindow.h"

class biblioteq_documentationwindow: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_documentationwindow(QWidget *parent);
  biblioteq_documentationwindow(const QUrl &url, QWidget *parent);
  ~biblioteq_documentationwindow();
  void setAllowOpeningOfExternalLinks(const bool state);
  void setHtml(const QString &html);

 public slots:
  void show(void);

 private:
  QPalette m_originalFindPalette;
  Ui_biblioteq_documentationwindow m_ui;
  bool m_openExternalLinks;
  void connectSignals(void);

 private slots:
  void slotAnchorClicked(const QUrl &url);
  void slotFind(void);
  void slotFindText(void);
  void slotPrint(void);
};

#endif
