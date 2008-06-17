/*
** -- Qt Includes --
*/

#include <QSpinBox>
#include <QComboBox>
#include <QDateEdit>
#include <QLineEdit>
#include <QTextEdit>

/*
** -- Local Includes --
*/

#include "qtbook_item.h"
#include "hyperlinked_text_edit.h"

/*
** -- qtbook_item() --
*/

qtbook_item::qtbook_item(void)
{
}

/*
** -- qtbook_item() --
*/

qtbook_item::qtbook_item(const int rowArg)
{
  row = rowArg;
}

/*
** -- ~qtbook_item() --
*/

qtbook_item::~qtbook_item()
{
  widgetValues.clear();
}

/*
** -- getRow() --
*/

int qtbook_item::getRow(void)
{
  return row;
}

/*
** -- getOldQ() --
*/

int qtbook_item::getOldQ(void)
{
  return oldq;
}

/*
** -- updateRow() --
*/

void qtbook_item::updateRow(const int rowArg)
{
  row = rowArg;
}

/*
** -- getID() --
*/

QString qtbook_item::getID(void)
{
  return oid;
}

/*
** -- print() --
*/

void qtbook_item::print(QWidget *parent)
{
  QPrinter printer;
  QPrintDialog dialog(&printer, parent);
  QTextDocument document;

  printer.setPageSize(QPrinter::Letter);
  printer.setColorMode(QPrinter::GrayScale);

  if(dialog.exec() == QDialog::Accepted)
    {
      document.setHtml(html);
      document.print(&printer);
    }
}

/*
** -- updateFont() --
*/

void qtbook_item::updateFont(const QFont &font, QWidget *window)
{
  window->setFont(font);

  foreach(QWidget *widget, window->findChildren<QWidget *>())
    widget->setFont(font);

  window->resize(window->sizeHint());
}

/*
** -- setOldQ() --
*/

void qtbook_item::setOldQ(const int q)
{
  oldq = q;
}

/*
** -- storeData() --
*/

void qtbook_item::storeData(QMainWindow *window)
{
  QString classname = "";
  QString objectname = "";

  widgetValues.clear();

  foreach(QWidget *widget, window->findChildren<QWidget *>())
    {
      classname = widget->metaObject()->className();
      objectname = widget->objectName();

      if(classname == "QSpinBox")
	widgetValues[objectname] = ((QSpinBox *) widget)->text().trimmed();
      else if(classname == "QLineEdit")
	widgetValues[objectname] = ((QLineEdit *) widget)->text().trimmed();
      else if(classname == "QComboBox")
	widgetValues[objectname] = ((QComboBox *) widget)->currentText().
	  trimmed();
      else if(classname == "QDateEdit")
	widgetValues[objectname] = ((QDateTimeEdit *) widget)->date().toString
	  ("MM/dd/yyyy");
      else if(classname == "QTextEdit")
	widgetValues[objectname] = ((QTextEdit *) widget)->
	  toPlainText().trimmed();
      else if(classname == "QDoubleSpinBox")
	widgetValues[objectname] = ((QDoubleSpinBox *) widget)->text().
	  trimmed();
      else if(classname == "hyperlinked_text_edit")
	widgetValues[objectname] = ((hyperlinked_text_edit *) widget)->
	  toPlainText().trimmed();
    }
}

/*
** -- isDataDifferent() --
*/

bool qtbook_item::isDataDifferent(QMainWindow *window)
{
  int i = 0;
  bool isDifferent = false;
  QString classname = "";
  QString objectname = "";
  QMap<QString, QString> newdata;

  foreach(QWidget *widget, window->findChildren<QWidget *>())
    {
      classname = widget->metaObject()->className();
      objectname = widget->objectName();

      if(classname == "QSpinBox")
	newdata[objectname] = ((QSpinBox *) widget)->text().trimmed();
      else if(classname == "QLineEdit")
	newdata[objectname] = ((QLineEdit *) widget)->text().trimmed();
      else if(classname == "QComboBox")
	newdata[objectname] = ((QComboBox *) widget)->currentText().
	  trimmed();
      else if(classname == "QDateEdit")
	newdata[objectname] = ((QDateTimeEdit *) widget)->date().toString
	  ("MM/dd/yyyy");
      else if(classname == "QTextEdit")
	newdata[objectname] = ((QTextEdit *) widget)->
	  toPlainText().trimmed();
      else if(classname == "QDoubleSpinBox")
	newdata[objectname] = ((QDoubleSpinBox *) widget)->text().
	  trimmed();
      else if(classname == "hyperlinked_text_edit")
	newdata[objectname] = ((hyperlinked_text_edit *) widget)->
	  toPlainText().trimmed();
    }

  for(i = 0; i < widgetValues.size(); i++)
    if(widgetValues[widgetValues.keys()[i]] != newdata[widgetValues.keys()[i]])
      {
	isDifferent = true;
	break;
      }

  newdata.clear();
  return isDifferent;
}
