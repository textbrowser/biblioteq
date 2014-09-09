/*
** -- Qt Includes --
*/

#include <QSpinBox>
#include <QComboBox>
#include <QDateEdit>
#include <QLineEdit>
#include <QTextEdit>
#include <QTimeEdit>

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
  row = -1;
  oldq = -1;
  isQueryEnabled = false;
  parentWid = 0;
}

/*
** -- qtbook_item() --
*/

qtbook_item::qtbook_item(const int rowArg)
{
  row = rowArg;
  oldq = -1;
  isQueryEnabled = false;
  parentWid = 0;
}

/*
** -- ~qtbook_item() --
*/

qtbook_item::~qtbook_item()
{
  imageValues.clear();
  widgetValues.clear();
}

/*
** -- getRow() --
*/

int qtbook_item::getRow(void) const
{
  return row;
}

/*
** -- getOldQ() --
*/

int qtbook_item::getOldQ(void) const
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

QString qtbook_item::getID(void) const
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

#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  dialog.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
#endif
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

  imageValues.clear();
  widgetValues.clear();

  foreach(QWidget *widget, window->findChildren<QWidget *>())
    {
      classname = widget->metaObject()->className();
      objectname = widget->objectName();

      if(classname == "QSpinBox")
	widgetValues[objectname] =
	  (static_cast<QSpinBox *> (widget))->text().trimmed();
      else if(classname == "QLineEdit")
	widgetValues[objectname] =
	  (static_cast<QLineEdit *> (widget))->text().trimmed();
      else if(classname == "QComboBox")
	widgetValues[objectname] =
	  (static_cast<QComboBox *> (widget))->currentText().trimmed();
      else if(classname == "QDateEdit")
	widgetValues[objectname] =
	  (static_cast<QDateEdit *> (widget))->date().toString("MM/dd/yyyy");
      else if(classname == "QTextEdit")
	widgetValues[objectname] =
	  (static_cast<QTextEdit *> (widget))->toPlainText().trimmed();
      else if(classname == "QDoubleSpinBox")
	widgetValues[objectname] =
	  (static_cast<QDoubleSpinBox *> (widget))->text().trimmed();
      else if(classname == "QTimeEdit")
	widgetValues[objectname] =
	  (static_cast<QTimeEdit *> (widget))->text().trimmed();
      else if(classname == "hyperlinked_text_edit")
	widgetValues[objectname] =
	  (static_cast<hyperlinked_text_edit *> (widget))->toPlainText().
	  trimmed();
      else if(classname == "image_drop_site")
	imageValues[objectname] =
	  (static_cast<image_drop_site *> (widget))->image;
    }
}

/*
** -- hasDataChanged() --
*/

bool qtbook_item::hasDataChanged(QMainWindow *window) const
{
  int i = 0;
  bool hasChanged = false;
  QString classname = "";
  QString objectname = "";
  QMap<QString, QImage> newimg;
  QMap<QString, QString> newdata;

  foreach(QWidget *widget, window->findChildren<QWidget *>())
    {
      classname = widget->metaObject()->className();
      objectname = widget->objectName();

      if(classname == "QSpinBox")
	newdata[objectname] =
	  (static_cast<QSpinBox *> (widget))->text().trimmed();
      else if(classname == "QLineEdit")
	newdata[objectname] =
	  (static_cast<QLineEdit *> (widget))->text().trimmed();
      else if(classname == "QComboBox")
	newdata[objectname] =
	  (static_cast<QComboBox *> (widget))->currentText().
	  trimmed();
      else if(classname == "QDateEdit")
	newdata[objectname] =
	  (static_cast<QDateEdit *> (widget))->date().toString
	  ("MM/dd/yyyy");
      else if(classname == "QTextEdit")
	newdata[objectname] =
	  (static_cast<QTextEdit *> (widget))->toPlainText().trimmed();
      else if(classname == "QDoubleSpinBox")
	newdata[objectname] =
	  (static_cast<QDoubleSpinBox *> (widget))->text().trimmed();
      else if(classname == "QTimeEdit")
	newdata[objectname] =
	  (static_cast<QTimeEdit *> (widget))->text().trimmed();
      else if(classname == "hyperlinked_text_edit")
	newdata[objectname] =
	  (static_cast<hyperlinked_text_edit *> (widget))->
	  toPlainText().trimmed();
      else if(classname == "image_drop_site")
	newimg[objectname] =
	  (static_cast<image_drop_site *> (widget))->image;
    }

  for(i = 0; i < widgetValues.size(); i++)
    if(widgetValues[widgetValues.keys()[i]] != newdata[widgetValues.keys()[i]])
      {
	hasChanged = true;
	break;
      }

  if(!hasChanged)
    for(i = 0; i < imageValues.size(); i++)
      if(imageValues[imageValues.keys()[i]] != newimg[imageValues.keys()[i]])
	{
	  hasChanged = true;
	  break;
	}

  newimg.clear();
  newdata.clear();
  return hasChanged;
}
