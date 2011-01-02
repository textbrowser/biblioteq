#include "z3950results.h"

/*
** -- z3950results() --
*/

z3950results::z3950results(QWidget *parent, QStringList &list,
			   qtbook_magazine *magazine_arg,
			   const QFont &font):
  QDialog(parent)
{
  int i = 0;

  magazine = magazine_arg;
  setWindowModality(Qt::WindowModal);
  ui.setupUi(this);
#ifdef Q_WS_MAC
  setAttribute(Qt::WA_MacMetalStyle, true);
#endif

  for(i = 0; i < list.count(); i++)
    {
      ui.list->addItem(QString(tr("Record #")) + QString::number(i + 1));
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
  ui.splitter->setStretchFactor(ui.splitter->indexOf(ui.recRet),  0);
  ui.splitter->setStretchFactor(ui.splitter->indexOf(ui.recCont),  1);
  setGlobalFonts(font);
  exec();
}

/*
** -- ~z3950results() --
*/

z3950results::~z3950results()
{
  records.clear();
}

/*
** -- slotSelectRecord() --
*/

void z3950results::slotSelectRecord(void)
{
  QStringList list;

  list = ui.textarea->toPlainText().split("\n");
  close();
  magazine->populateDisplayAfterZ3950(list);
  list.clear();
  deleteLater();
}

/*
** -- slotUpdateQueryText() --
*/

void z3950results::slotUpdateQueryText(void)
{
  ui.textarea->setPlainText(records[ui.list->currentRow()]);
}

/*
** -- closeEvent() --
*/

void z3950results::closeEvent(QCloseEvent *e)
{
  slotClose();
  QDialog::closeEvent(e);
}

/*
** -- slotClose() --
*/

void z3950results::slotClose(void)
{
  deleteLater();
}

/*
** -- setGlobalFonts() --
*/

void z3950results::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(QWidget *widget, findChildren<QWidget *>())
    widget->setFont(font);
}

/*
** -- keyPressEvent() --
*/

void z3950results::keyPressEvent(QKeyEvent *event)
{
  if(event && event->key() == Qt::Key_Escape)
    slotClose();

  QDialog::keyPressEvent(event);
}
