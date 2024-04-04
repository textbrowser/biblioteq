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

#include "biblioteq.h"
#include "biblioteq_hyperlinked_text_edit.h"
#include "biblioteq_image_drop_site.h"
#include "biblioteq_item.h"

#include <QApplication>
#include <QComboBox>
#include <QDateEdit>
#include <QLineEdit>
#include <QPersistentModelIndex>
#include <QPlainTextEdit>
#include <QSettings>
#include <QSpinBox>

biblioteq_item::biblioteq_item(const QModelIndex &index)
{
  m_index = new QPersistentModelIndex(index);
  m_isQueryEnabled = false;
  m_oldq = -1;
  m_parentWid = nullptr;
  m_queryHighlightColor = QColor(162, 205, 90);
  m_requiredHighlightColor = QColor(255, 248, 220);
  qmain = nullptr;
}

biblioteq_item::biblioteq_item(void):biblioteq_item(QModelIndex())
{
}

biblioteq_item::~biblioteq_item()
{
  delete m_index;
  m_imageValues.clear();
  m_widgetValues.clear();
}

QIcon biblioteq_item::iconForText(const QString &text) const
{
  QSettings setting;
  auto index = setting.value("otheroptions/display_icon_set_index", 0).toInt();
  auto system = false;

  if(index == 1)
    // System.

    system = true;

  if(text.startsWith(QObject::tr("Add")))
    return QIcon::fromTheme
      (system ? "list-add" : "", QIcon(":/16x16/add.png"));
  else if(text.startsWith(QObject::tr("Attach")) ||
	  text.startsWith(QObject::tr("Import")))
    return QIcon::fromTheme(system ? "document-import" : "", QIcon());
  else if(text.startsWith(QObject::tr("Close")))
    return QIcon::fromTheme
      (system ? "window-close" : "", QIcon(":/16x16/cancel.png"));
  else if(text.startsWith(QObject::tr("Compute")))
    return QIcon(":/16x16/sum.png");
  else if(text.startsWith(QObject::tr("Copies")))
    return QIcon::fromTheme
      (system ? "edit-copy" : "", QIcon(":/16x16/editcopy.png"));
  else if(text.startsWith(QObject::tr("Delete")))
    return QIcon::fromTheme
      (system ? "edit-delete" : "", QIcon(":/16x16/eraser.png"));
  else if(text.startsWith(QObject::tr("Download")))
    return QIcon(":/16x16/down.png");
  else if(text.startsWith(QObject::tr("Export")))
    return QIcon::fromTheme
      (system ? "document-export" : "", QIcon(":/16x16/fileexport.png"));
  else if(text.startsWith(QObject::tr("From ISBN")))
    return QIcon(":/16x16/convert.png");
  else if(text.startsWith(QObject::tr("Insert")))
    return QIcon::fromTheme
      (system ? "list-add" : "", QIcon(":/16x16/add.png"));
  else if(text.startsWith(QObject::tr("OK")))
    return QIcon::fromTheme
      (system ? "dialog-ok" : "", QIcon(":/16x16/ok.png"));
  else if(text.startsWith(QObject::tr("Print")))
    return QIcon::fromTheme
      (system ? "document-print" : "", QIcon(":/16x16/fileprint.png"));
  else if(text.startsWith(QObject::tr("Reservation Status")))
    return QIcon(":/16x16/members.png");
  else if(text.startsWith(QObject::tr("Reset")))
    return QIcon::fromTheme
      (system ? "edit-reset" : "", QIcon(":/16x16/reset.png"));
  else if(text.startsWith(QObject::tr("Save")))
    return QIcon::fromTheme
      (system ? "dialog-ok" : "", QIcon(":/16x16/ok.png"));
  else if(text.startsWith(QObject::tr("Search")))
    return QIcon::fromTheme
      (system ? "dialog-ok" : "", QIcon(":/16x16/ok.png"));
  else if(text.startsWith(QObject::tr("Select")))
    return QIcon::fromTheme
      (system ? "document-open" : "", QIcon(":/16x16/fileopen.png"));
  else if(text.startsWith(QObject::tr("Show Tracks")))
    return QIcon(":/16x16/edittracks.png");
  else
    return QIcon();
}

QString biblioteq_item::getID(void) const
{
  return m_oid;
}

bool biblioteq_item::hasDataChanged(QMainWindow *window) const
{
  if(!window)
    return false;

  QMap<QString, QImage> newimg;
  QMap<QString, QString> newdata;
  QString classname = "";
  QString objectname = "";
  auto hasChanged = false;
  int i = 0;

  foreach(auto widget, window->findChildren<QWidget *> ())
    {
      classname = widget->metaObject()->className();
      objectname = widget->objectName();

      if(classname == "QComboBox")
	newdata[objectname] =
	  (qobject_cast<QComboBox *> (widget))->currentText().trimmed();
      else if(classname == "QDateEdit")
	newdata[objectname] =
	  (qobject_cast<QDateEdit *> (widget))->date().toString
	  (biblioteq::s_databaseDateFormat);
      else if(classname == "QDoubleSpinBox")
	newdata[objectname] =
	  (qobject_cast<QDoubleSpinBox *> (widget))->text().trimmed();
      else if(classname == "QLineEdit")
	newdata[objectname] =
	  (qobject_cast<QLineEdit *> (widget))->text().trimmed();
      else if(classname == "QPlainTextEdit" ||
	      qobject_cast<QPlainTextEdit *> (widget))
	newdata[objectname] =
	  (qobject_cast<QPlainTextEdit *> (widget))->toPlainText().trimmed();
      else if(classname == "QSpinBox")
	newdata[objectname] =
	  (qobject_cast<QSpinBox *> (widget))->text().trimmed();
      else if(classname == "QTextEdit")
	newdata[objectname] =
	  (qobject_cast<QTextEdit *> (widget))->toPlainText().trimmed();
      else if(classname == "QTimeEdit")
	newdata[objectname] =
	  (qobject_cast<QTimeEdit *> (widget))->text().trimmed();
      else if(classname == "biblioteq_hyperlinked_text_edit")
	newdata[objectname] =
	  (qobject_cast<biblioteq_hyperlinked_text_edit *> (widget))->
	  toPlainText().trimmed();
      else if(classname == "biblioteq_image_drop_site")
	newimg[objectname] =
	  (qobject_cast<biblioteq_image_drop_site *> (widget))->m_image;
    }

  for(i = 0; i < m_imageValues.size(); i++)
    if(m_imageValues.value(m_imageValues.keys().at(i)) !=
       newimg.value(m_imageValues.keys().at(i)))
      {
	hasChanged = true;
	break;
      }

  for(i = 0; i < m_widgetValues.size(); i++)
    if(m_widgetValues.value(m_widgetValues.keys().at(i)) !=
       newdata.value(m_widgetValues.keys().at(i)))
      {
	hasChanged = true;
	break;
      }

  return hasChanged;
}

int biblioteq_item::getOldQ(void) const
{
  return m_oldq;
}

int biblioteq_item::getRow(void) const
{
  if(m_index->isValid())
    return m_index->row();
  else
    return -1;
}

void biblioteq_item::prepareIcons(QMainWindow *window)
{
  if(!window)
    return;

  foreach(auto pushButton, window->findChildren<QPushButton *> ())
    pushButton->setIcon(iconForText(pushButton->text().remove('&')));

  foreach(auto toolButton, window->findChildren<QToolButton *> ())
    toolButton->setIcon(iconForText(toolButton->text().remove('&')));
}

void biblioteq_item::print(QWidget *parent)
{
  QPrinter printer;
  QScopedPointer<QPrintDialog> dialog(new QPrintDialog(&printer, parent));
  QTextDocument document;

  printer.setColorMode(QPrinter::GrayScale);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
  printer.setPageSize(QPageSize(QPageSize::Letter));
#else
  printer.setPageSize(QPrinter::Letter);
#endif

  if(dialog->exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      document.setHtml(m_html);
      document.print(&printer);
    }

  QApplication::processEvents();
}

void biblioteq_item::setOldQ(const int q)
{
  m_oldq = q;
}

void biblioteq_item::setQMain(QMainWindow *window)
{
  if(!window)
    return;

  foreach(auto widget, window->findChildren<QWidget *> ())
    {
      const QString classname(widget->metaObject()->className());

      if(classname == "biblioteq_hyperlinked_text_edit")
	qobject_cast<biblioteq_hyperlinked_text_edit *> (widget)->
	  setQMain(qmain);
    }

  if(qmain)
    {
      m_queryHighlightColor = qmain->itemQueryResultColor();
      m_requiredHighlightColor = qmain->itemMandatoryFieldColor();
    }
}

void biblioteq_item::setReadOnlyFields(QMainWindow *window, const bool state)
{
  if(!window)
    return;

  foreach(auto widget, window->findChildren<QWidget *> ())
    {
      const QString className(widget->metaObject()->className());

      if(className == "QComboBox")
	qobject_cast<QComboBox *> (widget)->setEnabled(!state);
      else if(className == "QDateEdit")
	qobject_cast<QDateEdit *> (widget)->setReadOnly(state);
      else if(className == "QDoubleSpinBox")
	qobject_cast<QDoubleSpinBox *> (widget)->setReadOnly(state);
      else if(className == "QLineEdit")
	qobject_cast<QLineEdit *> (widget)->setReadOnly(state);
      else if(className == "QPlainTextEdit" ||
	      qobject_cast<QPlainTextEdit *> (widget))
	qobject_cast<QPlainTextEdit *> (widget)->setReadOnly(state);
      else if(className == "QSpinBox")
	qobject_cast<QSpinBox *> (widget)->setReadOnly(state);
      else if(className == "QTextEdit")
	qobject_cast<QTextEdit *> (widget)->setReadOnly(state);
      else if(className == "QTimeEdit")
	qobject_cast<QTimeEdit *> (widget)->setReadOnly(state);
      else if(className == "biblioteq_hyperlinked_text_edit")
	qobject_cast<biblioteq_hyperlinked_text_edit *> (widget)->
	  setReadOnly(state);
      else if(className == "biblioteq_image_drop_site")
	qobject_cast<biblioteq_image_drop_site *> (widget)->setReadOnly(state);
    }
}

void biblioteq_item::storeData(QMainWindow *window)
{
  if(!window)
    return;

  QString className = "";
  QString objectName = "";

  m_imageValues.clear();
  m_widgetValues.clear();

  foreach(auto widget, window->findChildren<QWidget *> ())
    {
      className = widget->metaObject()->className();
      objectName = widget->objectName();

      if(objectName == "qt_spinbox_lineedit")
	continue;

      if(className == "QComboBox")
	m_widgetValues[objectName] =
	  (qobject_cast<QComboBox *> (widget))->currentText().trimmed();
      else if(className == "QDateEdit")
	m_widgetValues[objectName] =
	  (qobject_cast<QDateEdit *> (widget))->date().toString
	  (biblioteq::s_databaseDateFormat);
      else if(className == "QDoubleSpinBox")
	m_widgetValues[objectName] =
	  (qobject_cast<QDoubleSpinBox *> (widget))->text().trimmed();
      else if(className == "QLineEdit")
	m_widgetValues[objectName] =
	  (qobject_cast<QLineEdit *> (widget))->text().trimmed();
      else if(className == "QPlainTextEdit" ||
	      qobject_cast<QPlainTextEdit *> (widget))
	m_widgetValues[objectName] =
	  (qobject_cast<QPlainTextEdit *> (widget))->toPlainText().trimmed();
      else if(className == "QSpinBox")
	m_widgetValues[objectName] =
	  (qobject_cast<QSpinBox *> (widget))->text().trimmed();
      else if(className == "QTextEdit")
	m_widgetValues[objectName] =
	  (qobject_cast<QTextEdit *> (widget))->toPlainText().trimmed();
      else if(className == "QTimeEdit")
	m_widgetValues[objectName] =
	  (qobject_cast<QTimeEdit *> (widget))->text().trimmed();
      else if(className == "biblioteq_hyperlinked_text_edit")
	m_widgetValues[objectName] =
	  (qobject_cast<biblioteq_hyperlinked_text_edit *> (widget))->
	  toPlainText().trimmed();
      else if(className == "biblioteq_image_drop_site")
	m_imageValues[objectName] =
	  (qobject_cast<biblioteq_image_drop_site *> (widget))->m_image;
    }
}

void biblioteq_item::updateFont(const QFont &font, QWidget *window)
{
  if(!window)
    return;

  window->setFont(font);

  foreach(auto widget, window->findChildren<QWidget *> ())
    {
      widget->setFont(font);
      widget->update();
    }

  window->update();
}

void biblioteq_item::updateQuantity(const int q)
{
  QMutableMapIterator<QString, QString> it(m_widgetValues);

  while(it.hasNext())
    {
      it.next();

      if(it.key().contains("quantity"))
	{
	  it.setValue(QString::number(q));
	  break;
	}
    }
}

void biblioteq_item::updateRow(const QModelIndex &index)
{
  delete m_index;
  m_index = new QPersistentModelIndex(index);
}
