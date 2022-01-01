#ifndef _BIBLIOTEQ_OTHEROPTIONS_H_
#define _BIBLIOTEQ_OTHEROPTIONS_H_

#include <QSettings>

#include "ui_biblioteq_otheroptions.h"

class biblioteq;

class biblioteq_otheroptions: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_otheroptions(biblioteq *parent);
  ~biblioteq_otheroptions();
  QColor availabilityColor(const QString &it) const;

  QString isbn10DisplayFormat(const QString &str) const
  {
    QSettings settings;
    auto index = qBound
      (0,
       settings.value("otheroptions/isbn10_display_format_index").toInt(),
       m_ui.isbn10_display_format->count() - 1);

    return isbnDisplayFormat(m_ui.isbn10_display_format->itemText(index), str);
  }

  QString isbn13DisplayFormat(const QString &str) const
  {
    QSettings settings;
    auto index = qBound
      (0,
       settings.value("otheroptions/isbn13_display_format_index").toInt(),
       m_ui.isbn13_display_format->count() - 1);

    return isbnDisplayFormat(m_ui.isbn13_display_format->itemText(index), str);
  }

  QString publicationDateFormat(const QString &it) const;
  bool showMainTableImages(void) const;
  bool showMainTableProgressDialogs(void) const;
  int booksAccessionNumberIndex(void) const;
  void prepareSettings(void);

 private:
  enum ItemsColumns
    {
     AVAILABILITY_COLOR = 1,
     ITEM_TYPE = 0,
     PUBLICATION_DATE_FORMAT = 1
    };

  Ui_otheroptions m_ui;
  biblioteq *qmain;

  QString isbnDisplayFormat(const QString &format, const QString &str) const
  {
    QString text("");
    auto list(format.split('-'));

    for(int i = 0, j = 0; i < list.size(); i++)
      {
	text.append(str.mid(j, list.at(i).length()));

	if(i != list.size() - 1)
	  text.append("-");

	j += list.at(i).length();
      }

    return text;
  }

  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void prepareAvailability(void);

 private slots:
  void setGlobalFonts(const QFont &font);
  void slotClose(void);
  void slotPreviewCanvasBackgroundColor(const QColor &color);
  void slotSave(void);
  void slotSelectAvailabilityColor(void);
  void slotSelectMainwindowCanvasBackgroundColor(void);

 public slots:
  void showNormal(void);

 signals:
  void mainWindowCanvasBackgroundColorChanged(const QColor &color);
  void mainWindowCanvasBackgroundColorPreview(const QColor &color);
  void saved(void);
};

#endif
