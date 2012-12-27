/*
** -- Qt Includes --
*/

#include <QXmlStreamReader>

/*
** -- Local Includes --
*/

#include "sruResults.h"
#include "qtbook_magazine.h"

/*
** -- sruresults() --
*/

sruresults::sruresults(QWidget *parent, const QList<QByteArray> &list,
		       qtbook_magazine *magazine_arg,
		       const QFont &font):
  QDialog(parent)
{
  int row = 0;

  m_records = list;
  magazine = magazine_arg;
  setWindowModality(Qt::WindowModal);
  ui.setupUi(this);
#ifdef Q_OS_MAC
  setAttribute(Qt::WA_MacMetalStyle, true);
#endif
  for(int i = 0; i < m_records.size(); i++)
    {
      QString issn("");
      QXmlStreamReader reader(m_records.at(i));

      while(!reader.atEnd())
	if(reader.readNextStartElement())
	  {
	    if(reader.name().toString().toLower().trimmed() == "datafield")
	      {
		QString tag(reader.attributes().value("tag").
			    toString().trimmed());

		if(tag == "022")
		  {
		    /*
		    ** $a - International Standard Serial Number (NR)
		    */

		    while(reader.readNextStartElement())
		      if(reader.name().toString().toLower().
			 trimmed() == "subfield")
			{
			  if(reader.attributes().value("code").
			     toString().trimmed() == "a")
			    {
			      issn.append(reader.readElementText());
			      break;
			    }
			}
		      else
			break;
		  }
	      }
	  }

      if(issn == magazine_arg->dialog().id->text())
	row = i;

      if(!issn.isEmpty())
	ui.list->addItem(issn);
      else
	ui.list->addItem
	  (QString(tr("Record #")) + QString::number(i + 1));
    }

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
  resize(0.95 * parent->size().width(),
	 0.95 * parent->size().height());
  exec();
}

/*
** -- ~sruresults() --
*/

sruresults::~sruresults()
{
  m_records.clear();
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
  QXmlStreamReader reader(m_records.value(ui.list->currentRow()));

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

  while(!reader.atEnd())
    if(reader.readNextStartElement())
      if(reader.name().toString().toLower().trimmed() == "datafield")
	{
	  QString tag(reader.attributes().value("tag").toString().trimmed());

	  if(tag == "245")
	    {
	      while(reader.readNextStartElement())
		if(reader.name().toString().toLower().
		   trimmed() == "subfield")
		  {
		    if(reader.attributes().value("code").
		       toString().trimmed() == "a" ||
		       reader.attributes().value("code").
		       toString().trimmed() == "b")
		      title.append(reader.readElementText());
		  }
		else
		  break;
	    }
	}

  title = title.mid(0, title.lastIndexOf('/')).trimmed();

  if(!title.isEmpty() && title[title.length() - 1].isPunct())
    title = title.remove(title.length() - 1, 1).trimmed();

  ui.title->setText(title);
  ui.title->setCursorPosition(0);
  ui.textarea->setPlainText(m_records[ui.list->currentRow()]);
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
