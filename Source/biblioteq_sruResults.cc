/*
** -- Qt Includes --
*/

#include <QXmlStreamReader>

/*
** -- Local Includes --
*/

#include "biblioteq_magazine.h"
#include "biblioteq_sruResults.h"

/*
** -- biblioteq_sruresults() --
*/

biblioteq_sruresults::biblioteq_sruresults
(QWidget *parent,
 const QList<QByteArray> &list,
 biblioteq_magazine *magazine_arg,
 const QFont &font):QDialog(parent)
{
  int row = -1;

  m_records = list;
  m_magazine = magazine_arg;
  setWindowModality(Qt::WindowModal);
  m_ui.setupUi(this);
#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
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

      if(row == -1)
	if(m_magazine)
	  if(issn == m_magazine->dialog().id->text())
	    row = i;

      if(!issn.isEmpty())
	m_ui.list->addItem(issn);
      else
	m_ui.list->addItem
	  (QString(tr("Record #")) + QString::number(i + 1));
    }

  connect(m_ui.list, SIGNAL(currentRowChanged(int)), this,
	  SLOT(slotUpdateQueryText(void)));
  connect(m_ui.okButton, SIGNAL(clicked(void)), this,
	  SLOT(slotSelectRecord(void)));
  connect(m_ui.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotClose(void)));

  if(row == -1)
    row = 0;

  m_ui.list->setCurrentRow(row);
  m_ui.splitter->setStretchFactor(0,  0);
  m_ui.splitter->setStretchFactor(1,  1);
  setGlobalFonts(font);

  if(parent)
    resize(qRound(0.75 * parent->size().width()),
	   qRound(0.75 * parent->size().height()));

  exec();
}

/*
** -- ~biblioteq_sruresults() --
*/

biblioteq_sruresults::~biblioteq_sruresults()
{
  m_records.clear();
}

/*
** -- slotSelectRecord() --
*/

void biblioteq_sruresults::slotSelectRecord(void)
{
  if(m_magazine)
    m_magazine->populateDisplayAfterSRU
      (m_records.value(m_ui.list->currentRow()));

  close();
}

/*
** -- slotUpdateQueryText() --
*/

void biblioteq_sruresults::slotUpdateQueryText(void)
{
  QString title("");
  QXmlStreamReader reader(m_records.value(m_ui.list->currentRow()));

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
  m_ui.title->setText(title);
  m_ui.title->setCursorPosition(0);
  m_ui.textarea->setPlainText(m_records.value(m_ui.list->currentRow()));
}

/*
** -- closeEvent() --
*/

void biblioteq_sruresults::closeEvent(QCloseEvent *e)
{
  Q_UNUSED(e);
  slotClose();
}

/*
** -- slotClose() --
*/

void biblioteq_sruresults::slotClose(void)
{
  deleteLater();
}

/*
** -- setGlobalFonts() --
*/

void biblioteq_sruresults::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(QWidget *widget, findChildren<QWidget *> ())
    widget->setFont(font);
}

/*
** -- keyPressEvent() --
*/

void biblioteq_sruresults::keyPressEvent(QKeyEvent *event)
{
  if(event && event->key() == Qt::Key_Escape)
    close();

  QDialog::keyPressEvent(event);
}

/*
** -- changeEvent() --
*/

void biblioteq_sruresults::changeEvent(QEvent *event)
{
  if(event)
    switch(event->type())
      {
      case QEvent::LanguageChange:
	{
	  m_ui.retranslateUi(this);
	  break;
	}
      default:
	break;
      }

  QDialog::changeEvent(event);
}
