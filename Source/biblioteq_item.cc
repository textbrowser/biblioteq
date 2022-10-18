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

#include "biblioteq_hyperlinked_text_edit.h"
#include "biblioteq_image_drop_site.h"
#include "biblioteq_item.h"

#include <QApplication>
#include <QComboBox>
#include <QDateEdit>
#include <QLineEdit>
#include <QPersistentModelIndex>
#include <QPlainTextEdit>
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

biblioteq_item::biblioteq_item(void)
{
  m_index = new QPersistentModelIndex(QModelIndex());
  m_isQueryEnabled = false;
  m_oldq = -1;
  m_parentWid = nullptr;
  m_queryHighlightColor = QColor(162, 205, 90);
  m_requiredHighlightColor = QColor(255, 248, 220);
  qmain = nullptr;
}

biblioteq_item::~biblioteq_item()
{
  delete m_index;
  m_imageValues.clear();
  m_widgetValues.clear();
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
	  (qobject_cast<QComboBox *> (widget))->currentText().
	  trimmed();
      else if(classname == "QDateEdit")
	newdata[objectname] =
	  (qobject_cast<QDateEdit *> (widget))->date().toString("MM/dd/yyyy");
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

  newdata.clear();
  newimg.clear();
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
      const QString &classname(widget->metaObject()->className());

      if(classname == "biblioteq_hyperlinked_text_edit")
	qobject_cast<biblioteq_hyperlinked_text_edit *> (widget)->
	  setQMain(qmain);
    }
}

void biblioteq_item::setReadOnlyFields(QMainWindow *window, const bool state)
{
  if(!window)
    return;

  foreach(auto widget, window->findChildren<QWidget *> ())
    {
      const QString &classname(widget->metaObject()->className());

      if(classname == "QComboBox")
	qobject_cast<QComboBox *> (widget)->setEnabled(!state);
      else if(classname == "QDateEdit")
	qobject_cast<QDateEdit *> (widget)->setReadOnly(state);
      else if(classname == "QDoubleSpinBox")
	qobject_cast<QDoubleSpinBox *> (widget)->setReadOnly(state);
      else if(classname == "QLineEdit")
	qobject_cast<QLineEdit *> (widget)->setReadOnly(state);
      else if(classname == "QPlainTextEdit" ||
	      qobject_cast<QPlainTextEdit *> (widget))
	qobject_cast<QPlainTextEdit *> (widget)->setReadOnly(state);
      else if(classname == "QSpinBox")
	qobject_cast<QSpinBox *> (widget)->setReadOnly(state);
      else if(classname == "QTextEdit")
	qobject_cast<QTextEdit *> (widget)->setReadOnly(state);
      else if(classname == "QTimeEdit")
	qobject_cast<QTimeEdit *> (widget)->setReadOnly(state);
      else if(classname == "biblioteq_hyperlinked_text_edit")
	qobject_cast<biblioteq_hyperlinked_text_edit *> (widget)->
	  setReadOnly(state);
      else if(classname == "biblioteq_image_drop_site")
	qobject_cast<biblioteq_image_drop_site *> (widget)->setReadOnly(state);
    }
}

void biblioteq_item::storeData(QMainWindow *window)
{
  if(!window)
    return;

  QString classname = "";
  QString objectname = "";

  m_imageValues.clear();
  m_widgetValues.clear();

  foreach(auto widget, window->findChildren<QWidget *> ())
    {
      classname = widget->metaObject()->className();
      objectname = widget->objectName();

      if(objectname == "qt_spinbox_lineedit")
	continue;

      if(classname == "QComboBox")
	m_widgetValues[objectname] =
	  (qobject_cast<QComboBox *> (widget))->currentText().trimmed();
      else if(classname == "QDateEdit")
	m_widgetValues[objectname] =
	  (qobject_cast<QDateEdit *> (widget))->date().toString("MM/dd/yyyy");
      else if(classname == "QDoubleSpinBox")
	m_widgetValues[objectname] =
	  (qobject_cast<QDoubleSpinBox *> (widget))->text().trimmed();
      else if(classname == "QLineEdit")
	m_widgetValues[objectname] =
	  (qobject_cast<QLineEdit *> (widget))->text().trimmed();
      else if(classname == "QPlainTextEdit" ||
	      qobject_cast<QPlainTextEdit *> (widget))
	m_widgetValues[objectname] =
	  (qobject_cast<QPlainTextEdit *> (widget))->toPlainText().trimmed();
      else if(classname == "QSpinBox")
	m_widgetValues[objectname] =
	  (qobject_cast<QSpinBox *> (widget))->text().trimmed();
      else if(classname == "QTextEdit")
	m_widgetValues[objectname] =
	  (qobject_cast<QTextEdit *> (widget))->toPlainText().trimmed();
      else if(classname == "QTimeEdit")
	m_widgetValues[objectname] =
	  (qobject_cast<QTimeEdit *> (widget))->text().trimmed();
      else if(classname == "biblioteq_hyperlinked_text_edit")
	m_widgetValues[objectname] =
	  (qobject_cast<biblioteq_hyperlinked_text_edit *> (widget))->
	  toPlainText().trimmed();
      else if(classname == "biblioteq_image_drop_site")
	m_imageValues[objectname] =
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
