/*
** Copyright (c) 2006 - present, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from BiblioteQ without specific prior written permission.
**
** BIBLIOTEQ IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** BIBLIOTEQ, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "biblioteq_magazine.h"
#include "biblioteq_sruResults.h"

#include <QXmlStreamReader>

biblioteq_sruresults::biblioteq_sruresults
(QWidget *parent,
 const QList<QByteArray> &list,
 biblioteq_magazine *magazine_arg,
 const QFont &font):QDialog(parent)
{
  int row = -1;

  m_records = list;
  m_magazine = magazine_arg;
  setWindowModality(Qt::ApplicationModal);
  m_ui.setupUi(this);

  for(int i = 0; i < m_records.size(); i++)
    {
      QString issn("");
      QXmlStreamReader reader(m_records.at(i));

      while(!reader.atEnd())
	if(reader.readNextStartElement())
	  {
	    if(reader.name().toString().toLower().trimmed() == "datafield")
	      {
		auto tag
		  (reader.attributes().value("tag").toString().trimmed());

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
			  else
			    reader.skipCurrentElement();
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
    resize(qRound(0.85 * parent->size().width()),
	   qRound(0.85 * parent->size().height()));

  exec();
  QApplication::processEvents();
}

biblioteq_sruresults::~biblioteq_sruresults()
{
  m_records.clear();
}

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

void biblioteq_sruresults::closeEvent(QCloseEvent *event)
{
  Q_UNUSED(event);
  slotClose();
}

void biblioteq_sruresults::keyPressEvent(QKeyEvent *event)
{
  if(event && event->key() == Qt::Key_Escape)
    close();

  QDialog::keyPressEvent(event);
}

void biblioteq_sruresults::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(auto widget, findChildren<QWidget *> ())
    {
      widget->setFont(font);
      widget->update();
    }

  update();
}

void biblioteq_sruresults::slotClose(void)
{
  deleteLater();
}

void biblioteq_sruresults::slotSelectRecord(void)
{
  if(m_magazine)
    m_magazine->populateDisplayAfterSRU
      (m_records.value(m_ui.list->currentRow()));

  close();
}

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
	  auto tag(reader.attributes().value("tag").toString().trimmed());

	  if(tag == "245")
	    {
	      while(reader.readNextStartElement())
		if(reader.name().toString().toLower().trimmed() == "subfield")
		  {
		    if(reader.attributes().value("code").
		       toString().trimmed() == "a" ||
		       reader.attributes().value("code").
		       toString().trimmed() == "b")
		      title.append(reader.readElementText());
		    else
		      reader.skipCurrentElement();
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
