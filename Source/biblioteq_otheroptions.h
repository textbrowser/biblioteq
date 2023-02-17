/*
** Copyright (c) 2006 - present, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from BiblioteQ without specific prior written permission.
**
** BIBLIOTEQ IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** BIBLIOTEQ, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _BIBLIOTEQ_OTHEROPTIONS_H_
#define _BIBLIOTEQ_OTHEROPTIONS_H_

#include <QColorDialog>
#include <QSettings>
#include <QStyledItemDelegate>

#include "ui_biblioteq_otheroptions.h"

class biblioteq_otheroptions_item_delegate: public QStyledItemDelegate
{
  Q_OBJECT

 public:
  biblioteq_otheroptions_item_delegate(QObject *parent):
    QStyledItemDelegate(parent)
  {
  }

  QWidget *createEditor(QWidget *parent,
			const QStyleOptionViewItem &option,
			const QModelIndex &index) const
  {
    switch(index.column())
      {
      case 1:
	{
	  auto pushButton = new QPushButton(parent);

	  connect(pushButton,
		  SIGNAL(clicked(void)),
		  this,
		  SLOT(slotSelectColor(void))
#ifdef Q_OS_MACOS
		  , Qt::QueuedConnection
#endif
		  );
	  m_index = index;
	  pushButton->setText(index.data().toString().trimmed());
	  return pushButton;
	}
      default:
	{
	  break;
	}
      }

    return QStyledItemDelegate::createEditor(parent, option, index);
  }

 private:
  void setModelData(QWidget *editor,
		    QAbstractItemModel *model,
		    const QModelIndex &index) const
  {
    auto pushButton = qobject_cast<QPushButton *> (editor);

    if(model && pushButton)
      {
	pushButton->setText(model->data(index).toString().trimmed());
	return;
      }

    QStyledItemDelegate::setModelData(editor, model, index);
  }

  mutable QModelIndex m_index;

 private slots:
  void slotSelectColor(void)
  {
    auto pushButton = qobject_cast<QPushButton *> (sender());

    if(!pushButton)
      return;

    QColorDialog dialog(pushButton);

    dialog.setCurrentColor(QColor(pushButton->text().remove('&')));
    dialog.setOptions(QColorDialog::DontUseNativeDialog);

    if(dialog.exec() == QDialog::Accepted)
      {
	if(m_index.isValid() && m_index.model())
	  const_cast<QAbstractItemModel *> (m_index.model())->setData
	    (m_index, dialog.selectedColor().name());

	pushButton->setText(dialog.selectedColor().name());

	if(m_index.isValid())
	  emit changed(dialog.selectedColor(), m_index.row());
      }
  }

 signals:
  void changed(const QColor &color, const int row);
};

class biblioteq;

class biblioteq_otheroptions: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_otheroptions(biblioteq *parent);
  ~biblioteq_otheroptions();
  QColor availabilityColor(const QString &it) const;
  QColor itemMandatoryFieldColor(void) const;
  QColor itemQueryResultColor(void) const;
  QMap<QString, QColor> customQueryColors(void) const;

  QString isbn10DisplayFormat(const QString &str) const
  {
    if(QString(str).remove('-').length() != 10)
      return str;

    if(m_isbn10Format.isEmpty())
      {
	QSettings settings;
	auto index = qBound
	  (0,
	   settings.value("otheroptions/isbn10_display_format_index").toInt(),
	   m_ui.isbn10_display_format->count() - 1);

	m_isbn10Format = m_ui.isbn10_display_format->itemText(index);
      }

    return isbnDisplayFormat(m_isbn10Format, str);
  }

  QString isbn13DisplayFormat(const QString &str) const
  {
    if(QString(str).remove('-').length() != 13)
      return str;

    if(m_isbn13Format.isEmpty())
      {
	QSettings settings;
	auto index = qBound
	  (0,
	   settings.value("otheroptions/isbn13_display_format_index").toInt(),
	   m_ui.isbn13_display_format->count() - 1);

	m_isbn13Format = m_ui.isbn13_display_format->itemText(index);
      }

    return isbnDisplayFormat(m_isbn13Format, str);
  }

  QString publicationDateFormat(const QString &it) const;
  bool showBookReadStatus(void) const;
  bool showMainTableImages(void) const;
  bool showMainTableProgressDialogs(void) const;
  int booksAccessionNumberIndex(void) const;
  int iconsViewColumnCount(void) const;
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
  biblioteq_otheroptions_item_delegate *m_itemDelegate;
  mutable QString m_isbn10Format;
  mutable QString m_isbn13Format;

  QString isbnDisplayFormat(const QString &format, const QString &s) const
  {
    auto str(QString(s).remove('-'));

    if(!(str.length() == 10 || str.length() == 13))
      return s;

    QString text("");
    auto list(format.split('-'));

    for(int i = 0, j = 0; i < list.size(); i++)
      {
	text.append(str.mid(j, list.at(i).length()));

	if(i != list.size() - 1)
	  text.append('-');

	j += list.at(i).length();
      }

    return text;
  }

  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void prepareAvailability(void);
  void prepareSQLiteKeywords(void);

 private slots:
  void setGlobalFonts(const QFont &font);
  void slotClose(void);
  void slotCustomQueryColorSelected(const QColor &color, const int row);
  void slotPreviewCanvasBackgroundColor(const QColor &color);
  void slotSave(void);
  void slotSelectAvailabilityColor(void);
  void slotSelectColor(void);
  void slotSelectMainwindowCanvasBackgroundColor(void);

 public slots:
  void showNormal(void);

 signals:
  void mainWindowCanvasBackgroundColorChanged(const QColor &color);
  void mainWindowCanvasBackgroundColorPreview(const QColor &color);
  void saved(void);
};

#endif
