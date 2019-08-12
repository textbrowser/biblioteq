#ifndef _BIBLIOTEQ_OTHEROPTIONS_H_
#define _BIBLIOTEQ_OTHEROPTIONS_H_

#include "ui_biblioteq_otheroptions.h"

class biblioteq;

class biblioteq_otheroptions: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_otheroptions(biblioteq *parent);
  ~biblioteq_otheroptions();
  QString publicationDateFormat(const QString &itemType) const;
  void prepareSettings(void);

 private:
  Ui_otheroptions m_ui;
  biblioteq *qmain;
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void setGlobalFonts(const QFont &font);

 private slots:
  void slotClose(void);
  void slotPreviewCanvasBackgroundColor(const QColor &color);
  void slotSave(void);
  void slotSelectMainwindowCanvasBackgroundColor(void);

 public slots:
  void showNormal(void);

 signals:
  void mainWindowCanvasBackgroundColorChanged(const QColor &color);
  void mainWindowCanvasBackgroundColorPreview(const QColor &color);
  void saved(void);
};

#endif
