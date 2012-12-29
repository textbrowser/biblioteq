/*
** -- Local Includes --
*/

#include "z3950results.h"
#include "qtbook_magazine.h"

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
#ifdef Q_OS_MAC
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
	    */

	    issn = parsedList.at(j);
	    issn = issn.mid(issn.indexOf("$a") + 2).trimmed();

	    if(issn.contains(" "))
	      issn = issn.mid(0, issn.indexOf(" ")).trimmed();

	    if(issn.length() >= 9)
	      issn = issn.mid(0, 9).trimmed();

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
  ui.splitter->setStretchFactor(0,  0);
  ui.splitter->setStretchFactor(1,  1);
  setGlobalFonts(font);
  resize(0.75 * parent->size().width(),
	 0.75 * parent->size().height());
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
  QStringList list(records.value(ui.list->currentRow()).split("\n"));

  for(int i = 0; i < list.size(); i++)
    if(list.at(i).startsWith("245 "))
      {
	/*
	** $a - Title (NR)
	** $b - Remainder of title (NR)
	** $c - Statement of responsibility, etc. (NR)
	** $f - Inclusive dates (NR)
	** $g - Bulk dates (NR)
	** $h - Medium (NR)
	** $k - Form (R)
	** $n - Number of part/section of a work (R)
	** $p - Name of part/section of a work (R)
	** $s - Version (NR)
	** $6 - Linkage (NR)
	** $8 - Field link and sequence number (R)
	*/

	title = list.at(i);
	title = title.mid(title.indexOf("$a") + 2).trimmed();
	title = title.remove(" $b").trimmed();

	QStringList subfields;

	subfields << "$c"
		  << "$f"
		  << "$g"
		  << "$h"
		  << "$k"
		  << "$n"
		  << "$p"
		  << "$s"
		  << "$6"
		  << "$8";

	while(!subfields.isEmpty())
	  if(title.contains(subfields.first()))
	    {
	      title = title.mid
		(0, title.indexOf(subfields.first())).trimmed();
	      break;
	    }
	  else
	    subfields.takeFirst();

	title = title.mid(0, title.lastIndexOf('/')).trimmed();

	if(!title.isEmpty() && title[title.length() - 1].isPunct())
	  title = title.remove(title.length() - 1, 1).trimmed();

	break;
      }

  ui.title->setText(title);
  ui.title->setCursorPosition(0);
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

/*
** -- changeEvent() --
*/

void z3950results::changeEvent(QEvent *event)
{
  if(event)
    switch(event->type())
      {
      case QEvent::LanguageChange:
	{
	  ui.retranslateUi(this);
	  break;
	}
      default:
	break;
      }

  QDialog::changeEvent(event);
}
