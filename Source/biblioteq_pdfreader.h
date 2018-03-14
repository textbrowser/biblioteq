#ifndef _BIBLIOTEQ_PDFREADER_H_
#define _BIBLIOTEQ_PDFREADER_H_

/*
** -- Qt Includes --
*/

#include <QMainWindow>

/*
** -- Poppler Includes --
*/

#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
#if QT_VERSION < 0x050000
#include <poppler-qt4.h>
#else
#include <poppler-qt5.h>
#endif
#endif

/*
** -- Local Includes --
*/

#include "ui_biblioteq_pdfreader.h"

class biblioteq_pdfreader: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_pdfreader(QWidget *parent);
  ~biblioteq_pdfreader();
  void load(const QByteArray &data, const QString &fileName);
  void load(const QString &fileName);

 private:
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  Poppler::Document *m_document;
#endif
  QString m_fileName;
  Ui_pdfreader m_ui;
  bool event(QEvent *event);
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void prepareContents(void);
  void resizeEvent(QResizeEvent *event);
  void setGlobalFonts(const QFont &font);

 private slots:
  void slotChangePageViewSize(int value);
  void slotClose(void);
  void slotContentsDoubleClicked(QListWidgetItem *item);
  void slotPrint(void);
  void slotSaveAs(void);
  void slotSearchNext(void);
  void slotShowContents(bool state);
  void slotShowPage(int value);
  void slotSliderTriggerAction(int action);

 public slots:
  void showNormal(void);
};

#endif
