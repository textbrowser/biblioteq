#include "biblioteq_hyperlinked_text_edit.h"
#include "biblioteq_image_drop_site.h"
#include "biblioteq_item.h"

#include <QApplication>
#include <QComboBox>
#include <QDateEdit>
#include <QLineEdit>
#include <QSpinBox>

biblioteq_item::biblioteq_item(const int rowArg)
{
  m_isQueryEnabled = false;
  m_oldq = -1;
  m_parentWid = 0;
  m_row = rowArg;
  qmain = 0;
}

biblioteq_item::biblioteq_item(void)
{
  m_isQueryEnabled = false;
  m_oldq = -1;
  m_parentWid = 0;
  m_row = -1;
  qmain = 0;
}

biblioteq_item::~biblioteq_item()
{
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
  bool hasChanged = false;
  int i = 0;

  foreach(QWidget *widget, window->findChildren<QWidget *> ())
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
    if(m_imageValues[m_imageValues.keys()[i]] !=
       newimg[m_imageValues.keys()[i]])
      {
	hasChanged = true;
	break;
      }

  for(i = 0; i < m_widgetValues.size(); i++)
    if(m_widgetValues[m_widgetValues.keys()[i]] !=
       newdata[m_widgetValues.keys()[i]])
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
  return m_row;
}

void biblioteq_item::print(QWidget *parent)
{
  QPrinter printer;
  QPrintDialog dialog(&printer, parent);
  QTextDocument document;

  printer.setPageSize(QPrinter::Letter);
  printer.setColorMode(QPrinter::GrayScale);

  if(dialog.exec() == QDialog::Accepted)
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

  foreach(QWidget *widget, window->findChildren<QWidget *> ())
    {
      QString classname(widget->metaObject()->className());

      if(classname == "biblioteq_hyperlinked_text_edit")
	qobject_cast<biblioteq_hyperlinked_text_edit *> (widget)->
	  setQMain(qmain);
    }
}

void biblioteq_item::setReadOnlyFields(QMainWindow *window, const bool state)
{
  if(!window)
    return;

  foreach(QWidget *widget, window->findChildren<QWidget *> ())
    {
      QString classname(widget->metaObject()->className());

      if(classname == "QComboBox")
	qobject_cast<QComboBox *> (widget)->setEnabled(!state);
      else if(classname == "QDateEdit")
	qobject_cast<QDateEdit *> (widget)->setReadOnly(state);
      else if(classname == "QDoubleSpinBox")
	qobject_cast<QDoubleSpinBox *> (widget)->setReadOnly(state);
      else if(classname == "QLineEdit")
	qobject_cast<QLineEdit *> (widget)->setReadOnly(state);
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

  foreach(QWidget *widget, window->findChildren<QWidget *> ())
    {
      classname = widget->metaObject()->className();
      objectname = widget->objectName();

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
	  toPlainText().
	  trimmed();
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

  foreach(QWidget *widget, window->findChildren<QWidget *> ())
    {
      widget->setFont(font);
      widget->update();
    }

  window->update();
}

void biblioteq_item::updateRow(const int rowArg)
{
  m_row = rowArg;
}
