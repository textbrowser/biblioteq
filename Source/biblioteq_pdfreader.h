#ifndef _BIBLIOTEQ_PDFREADER_H_
#define _BIBLIOTEQ_PDFREADER_H_

#include "ui_biblioteq_pdfreader.h"

#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
#ifdef Q_OS_MACOS
#include <poppler-qt6.h>
#else
#include <poppler-qt5.h>
#endif
#endif

class QPrinter;

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
  QRectF m_searchLocation;
  QString m_fileName;
  Ui_pdfreader m_ui;
  bool event(QEvent *event);
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void prepareContents(void);
  void resizeEvent(QResizeEvent *event);

 private slots:
  void setGlobalFonts(const QFont &font);
  void slotChangePageViewSize(int value);
  void slotClose(void);
  void slotContentsDoubleClicked(QListWidgetItem *item);
  void slotPrint(void);
  void slotPrintPreview(QPrinter *printer);
  void slotPrintPreview(void);
  void slotSaveAs(void);
  void slotSearchNext(void);
  void slotSearchPrevious(void);
  void slotShowContents(bool state);
  void slotShowPage(int value, const QRectF &location = QRectF());
  void slotSliderTriggerAction(int action);

 public slots:
  void showNormal(void);
};

#endif
