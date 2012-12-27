/*
** -- Local Includes --
*/

#include "sruResults.h"
#include "qtbook_magazine.h"

/*
** -- sruresults() --
*/

sruresults::sruresults(QWidget *parent, const QByteArray &data,
		       qtbook_magazine *magazine_arg,
		       const QFont &font):
  QDialog(parent)
{
  int row = 0;

  m_data = data;
  magazine = magazine_arg;
  setWindowModality(Qt::WindowModal);
  ui.setupUi(this);
#ifdef Q_OS_MAC
  setAttribute(Qt::WA_MacMetalStyle, true);
#endif
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
  exec();
}

/*
** -- ~sruresults() --
*/

sruresults::~sruresults()
{
  records.clear();
}

/*
** -- slotSelectRecord() --
*/

void sruresults::slotSelectRecord(void)
{
  close();
  magazine->populateDisplayAfterSRU(ui.textarea->toHtml().toUtf8());
  deleteLater();
}

/*
** -- slotUpdateQueryText() --
*/

void sruresults::slotUpdateQueryText(void)
{
  QString title("");
  QStringList list(records[ui.list->currentRow()].split("\n"));

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

void sruresults::closeEvent(QCloseEvent *e)
{
  slotClose();
  QDialog::closeEvent(e);
}

/*
** -- slotClose() --
*/

void sruresults::slotClose(void)
{
  deleteLater();
}

/*
** -- setGlobalFonts() --
*/

void sruresults::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(QWidget *widget, findChildren<QWidget *>())
    widget->setFont(font);
}

/*
** -- keyPressEvent() --
*/

void sruresults::keyPressEvent(QKeyEvent *event)
{
  if(event && event->key() == Qt::Key_Escape)
    slotClose();

  QDialog::keyPressEvent(event);
}

/*
** -- changeEvent() --
*/

void sruresults::changeEvent(QEvent *event)
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
