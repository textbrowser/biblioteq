#include "locresults.h"

/*
** -- locresults() --
*/

locresults::locresults(QWidget *parent, QStringList &list,
		       qtbook_magazine *magazine_arg,
		       const QFont &font):
  QDialog(parent)
{
  int i = 0;

  magazine = magazine_arg;
  setWindowModality(Qt::WindowModal);
  ui.setupUi(this);

  for(i = 0; i < list.count(); i++)
    {
      ui.list->addItem(QString("Record #%1").arg(i + 1));
      records.append(list[i]);
    }

  list.clear();
  connect(ui.list, SIGNAL(currentRowChanged(int)), this,
	  SLOT(slotUpdateQueryText(void)));
  connect(ui.okButton, SIGNAL(clicked(void)), this,
	  SLOT(slotSelectRecord(void)));
  connect(ui.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotClose(void)));
  ui.list->setCurrentRow(0);
  setGlobalFonts(font);
  show();
}

/*
** -- ~locresults() --
*/

locresults::~locresults()
{
  records.clear();
}

/*
** -- slotSelectRecord() --
*/

void locresults::slotSelectRecord(void)
{
  QStringList list;

  list = ui.textarea->toPlainText().split("\n");
  magazine->populateDisplayAfterLOC(list);
  list.clear();
  deleteLater();
}

/*
** -- slotUpdateQueryText() --
*/

void locresults::slotUpdateQueryText(void)
{
  ui.textarea->setPlainText(records[ui.list->currentRow()]);
}

/*
** -- closeEvent() --
*/

void locresults::closeEvent(QCloseEvent *e)
{
  (void) e;
  deleteLater();
}

/*
** -- slotClose() --
*/

void locresults::slotClose(void)
{
  deleteLater();
}

/*
** -- setGlobalFonts() --
*/

void locresults::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(QWidget *widget, findChildren<QWidget *>())
    widget->setFont(font);
}
