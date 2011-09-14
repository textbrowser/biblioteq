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
  int row = 0;

  magazine = magazine_arg;
  setWindowModality(Qt::WindowModal);
  ui.setupUi(this);
#ifdef Q_WS_MAC
  setAttribute(Qt::WA_MacMetalStyle, true);
#endif

  for(i = 0; i < list.count(); i++)
    {
      QString issn("");
      QStringList parsedList(list.at(i).split("\n"));

      for(int j = 0; j < parsedList.size(); j++)
	if(parsedList.at(j).startsWith("022 "))
	  {
	    /*
	    ** $a - International Standard Serial Number (NR)
	    ** $l - ISSN-L (NR)
	    ** $m - Canceled ISSN-L (R)
	    ** $y - Incorrect ISSN (R)
	    ** $z - Canceled ISSN (R)
	    ** $2 - Source (NR)
	    ** $6 - Linkage (NR)
	    ** $8 - Field link and sequence number (R) 
	    */

	    issn = parsedList.at(j);
	    issn = issn.mid(issn.indexOf("$a") + 2).trimmed();

	    if(issn.contains(" "))
	      issn = issn.mid(0, issn.indexOf(" "));

	    if(magazine_arg->dialog().id->text() == issn)
	      row = i;

	    break;
	  }

      if(!issn.isEmpty())
	ui.list->addItem(issn);
      else
	ui.list->addItem
	  (QString(tr("Record #")) + QString::number(i + 1));

      records.append(list[i]);
    }

  list.clear();
  connect(ui.list, SIGNAL(currentRowChanged(int)), this,
	  SLOT(slotUpdateQueryText(void)));
  connect(ui.okButton, SIGNAL(clicked(void)), this,
	  SLOT(slotSelectRecord(void)));
  connect(ui.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotClose(void)));
  ui.list->setCurrentRow(row);
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
  QString title("");
  QStringList list(records[ui.list->currentRow()].split("\n"));

  for(int i = 0; i < list.size(); i++)
    if(list.at(i).startsWith("245 "))
      {
	/*
	** $a - Title
	** $b - Remainder of Title
	** $c - Statement of Responsibility
	** $d - Designation of section (Obsolete)
	** $e - Name of Part/Section
	** $f - Inclusive Dates
	** $g - Bulk Dates
	** $h - Medium
	** $k - Form
	** $n - Number of Part/Section of a Work
	** $p - Name of Part/Section of a Work
	** $s - Version (NR)
	** $6 - Linkage (NR)
	** $8 - Field Link and Sequence Number
	*/

	title = list.at(i);
	title = title.mid(title.indexOf("$a") + 2).trimmed();
	title = title.remove(" $b").trimmed();
	title = title.remove(" $c").trimmed();
	title = title.remove(" $d").trimmed();
	title = title.remove(" $f").trimmed();
	title = title.remove(" $g").trimmed();
	title = title.remove(" $h").trimmed();
	title = title.remove(" $k").trimmed();
	title = title.remove(" $n").trimmed();
	title = title.remove(" $p").trimmed();
	title = title.remove(" $s").trimmed();
	title = title.remove(" $6").trimmed();
	title = title.remove(" $8").trimmed();
	break;
      }

  ui.title->setText(title);
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
