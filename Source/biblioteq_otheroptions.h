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
#include <QLineEdit>
#include <QPointer>
#include <QSettings>
#include <QStyledItemDelegate>
#include <QTimer>

#include "biblioteq.h"
#include "biblioteq_misc_functions.h"
#include "ui_biblioteq_otheroptions.h"

class biblioteq_otheroptions_shortcut_lineedit: public QLineEdit
{
  Q_OBJECT

 public:
  biblioteq_otheroptions_shortcut_lineedit(QWidget *parent):QLineEdit(parent)
  {
  }

 private:
  void keyPressEvent(QKeyEvent *event)
  {
    QLineEdit::keyReleaseEvent(event);

    if(event)
      {
	auto const key = static_cast<Qt::Key> (event->key());

	if(key == Qt::Key_Alt ||
	   key == Qt::Key_Control ||
	   key == Qt::Key_Meta ||
	   key == Qt::Key_Shift)
	  return;

	auto const modifiers = event->modifiers();
	auto integer = event->key();

	if(modifiers & Qt::AltModifier)
	  integer += Qt::ALT;

	if(modifiers & Qt::ControlModifier)
	  integer += Qt::CTRL;

	if(modifiers & Qt::MetaModifier)
	  integer += Qt::META;

	if(modifiers & Qt::ShiftModifier)
	  integer += Qt::SHIFT;

	setText(QKeySequence(integer).toString(QKeySequence::NativeText));
      }
  }
};

class biblioteq_otheroptions_item_delegate: public QStyledItemDelegate
{
  Q_OBJECT

 public:
  enum class ParentTypes
    {
      Keywords = 0,
      Shortcuts,
      SpecialValueColors
    };

  biblioteq_otheroptions_item_delegate
    (const ParentTypes type, QObject *parent):QStyledItemDelegate(parent)
  {
    m_parent = qobject_cast<QWidget *> (parent);
    m_type = type;
  }

  QWidget *createEditor(QWidget *parent,
			const QStyleOptionViewItem &option,
			const QModelIndex &index) const
  {
    switch(m_type)
      {
      case ParentTypes::Keywords:
      case ParentTypes::SpecialValueColors:
	{
	  switch(index.column())
	    {
	    case 1: // Keyword Color
	    case 2: // Special Value Color
	      {
		auto pushButton = new QPushButton(parent);

		connect(pushButton,
			SIGNAL(clicked(void)),
			this,
			SLOT(slotSelectColor(void)));
		m_index = index;
		pushButton->setText
		  (index.data().toString().remove('&').trimmed());
		return pushButton;
	      }
	    case 3: // Special Value Reset
	      {
		auto pushButton = new QPushButton(parent);

		connect(pushButton,
			SIGNAL(clicked(void)),
			this,
			SLOT(slotResetRow(void)));
		m_index = index;
		pushButton->setText(tr("Reset Row"));
		return pushButton;
	      }
	    default:
	      {
		break;
	      }
	    }

	  break;
	}
      case ParentTypes::Shortcuts:
	{
	  switch(index.column())
	    {
	    case 1: // Shortcut
	      {
		auto lineEdit = new biblioteq_otheroptions_shortcut_lineedit
		  (parent);

		lineEdit->setClearButtonEnabled(true);
		lineEdit->setPlaceholderText(tr("Shortcut"));
		lineEdit->setText(index.data().toString().trimmed());
		return lineEdit;
	      }
	    default:
	      {
		break;
	      }
	    }

	  break;
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
	QTimer::singleShot(1000, this, SIGNAL(changed(void))); // Constness.
	return;
      }

    QStyledItemDelegate::setModelData(editor, model, index);
    QTimer::singleShot(1000, this, SIGNAL(changed(void))); // Constness.
  }

  ParentTypes m_type;
  QPointer<QWidget> m_parent;
  mutable QModelIndex m_index;

 private slots:
  void slotResetRow(void)
  {
    if(!m_index.isValid())
      return;

    auto model = const_cast<QAbstractItemModel *> (m_index.model());

    if(model)
      for(int i = 0; i < model->columnCount(); i++)
	if(i != 3) // Reset.
	  {
	    model->setData
	      (model->sibling(m_index.row(), i, m_index), QVariant());
	    model->setData
	      (model->sibling(m_index.row(), i, m_index),
	       QVariant(),
	       Qt::DecorationRole);
	  }
  }

  void slotSelectColor(void)
  {
    QColorDialog dialog(m_parent);

    dialog.setCurrentColor
      (QColor(m_index.data().toString().remove('&').trimmed()));

    if(dialog.exec() == QDialog::Accepted &&
       m_index.isValid() &&
       m_index.model())
      {
	auto model = const_cast<QAbstractItemModel *> (m_index.model());
	auto sortingEnabled = false;
	auto table = qobject_cast<QTableWidget *>
	  (m_index.model()->parent());

	if(table)
	  {
	    sortingEnabled = table->isSortingEnabled();
	    table->setSortingEnabled(false);
	  }

	if(model)
	  {
	    model->setData(m_index, dialog.selectedColor().name());
	    model->setData
	      (m_index, dialog.selectedColor(), Qt::DecorationRole);
	  }

	if(table)
	  table->setSortingEnabled(sortingEnabled);
      }
  }

 signals:
  void changed(void);
};

class biblioteq_otheroptions: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_otheroptions(biblioteq *parent);
  ~biblioteq_otheroptions();
  QColor availabilityColor(const QString &it) const;
  QColor itemMandatoryFieldColor(void) const;
  QColor itemQueryResultColor(void) const;
  QColor membersMandatoryFieldColor(void) const;

  QMap<QPair<QString, QString>, QColor> specialValueColors(void) const
  {
    return m_specialValueColors;
  }

  QMap<QString, QColor> customQueryColors(void) const;

  QString isbn10DisplayFormat(const QString &str) const
  {
    if(QString(str).remove('-').length() != 10)
      return str;

    if(m_isbn10Format.isEmpty())
      {
	QSettings settings;
	auto const index = qBound
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
	auto const index = qBound
	  (0,
	   settings.value("otheroptions/isbn13_display_format_index").toInt(),
	   m_ui.isbn13_display_format->count() - 1);

	m_isbn13Format = m_ui.isbn13_display_format->itemText(index);
      }

    return isbnDisplayFormat(m_isbn13Format, str);
  }

  QString dateFormat(const QString &it) const;
  QString sqliteDatabaseEncoding(void) const;
  bool isMembersColumnVisible(const QString &text) const;
  bool showBookReadStatus(void) const;
  bool showMainTableImages(void) const;
  bool showMainTableProgressDialogs(void) const;
  int booksAccessionNumberIndex(void) const;
  int iconsViewColumnCount(void) const;
  static void prepareEnvironmentVariables(void);
  void prepareMembersVisibleColumns(QTableWidget *table);
  void prepareSettings(void);

  void sqliteConnected(void)
  {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_ui.sqlite_reminders->setPlainText
      (biblioteq_misc_functions::sqliteReturnReminders(m_qmain->getDB()));
    QApplication::restoreOverrideCursor();
  }

 private:
  enum class ItemsColumns
    {
      AVAILABILITY_COLOR = 1,
      DATE_FORMAT = 1,
      ITEM_TYPE = 0
    };

  enum class Limits
    {
      SpecialValueColorsRows = 5
    };

  enum class SpecialColorsColumns
    {
      CellText = 0,
      Color = 2,
      ColumnTitle = 1,
      Reset = 3
    };

  QMap<QPair<QString, QString>, QColor> m_specialValueColors;
  Ui_otheroptions m_ui;
  biblioteq *m_qmain;
  biblioteq_otheroptions_item_delegate *m_keywordsItemDelegate;
  biblioteq_otheroptions_item_delegate *m_shortcutsItemDelegate;
  biblioteq_otheroptions_item_delegate *m_specialValueColorsItemDelegate;
  mutable QString m_isbn10Format;
  mutable QString m_isbn13Format;

  QString isbnDisplayFormat(const QString &format, const QString &s) const
  {
    auto const str(QString(s).remove('-'));

    if(!(str.length() == 10 || str.length() == 13))
      return s;

    QString text("");
    auto const list(format.split('-'));

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
  void prepareIcons(void);
  void prepareSQLKeywords(void);
  void prepareShortcuts(void);
  void prepareSpecialColors(const bool reset = false);

 private slots:
  void setGlobalFonts(const QFont &font);
  void slotClose(void);
  void slotMainWindowShortcutChanged(void);
  void slotPreviewCanvasBackgroundColor(const QColor &color);
  void slotReset(void);
  void slotResetCustomQueryColors(void);
  void slotSave(void);
  void slotSelectAvailabilityColor(void);
  void slotSelectColor(void);
  void slotSelectMainwindowCanvasBackgroundColor(void);
  void slotShowCustomQuery(void);

 public slots:
  void showNormal(void);

 signals:
  void mainWindowCanvasBackgroundColorChanged(const QColor &color);
  void mainWindowCanvasBackgroundColorPreview(const QColor &color);
  void saved(void);
};

#endif
