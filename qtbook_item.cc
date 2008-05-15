/*
** -- Local Includes --
*/

#include "qtbook_item.h"

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
