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
#include "biblioteq_z3950results.h"

biblioteq_z3950results::biblioteq_z3950results
(QWidget *parent,
 QStringList &list,
 biblioteq_magazine *magazine_arg,
 const QFont &font,
 const QString &recordSyntax):QDialog(parent)
{
  int i = 0;
  int row = -1;

  m_magazine = magazine_arg;
  m_recordSyntax = recordSyntax;
  setWindowModality(Qt::ApplicationModal);
  m_ui.setupUi(this);

  for(i = 0; i < list.size(); i++)
    {
      QString issn("");
      auto parsedList(list.at(i).split("\n"));

      for(int j = 0; j < parsedList.size(); j++)
	if(parsedList.at(j).startsWith("022 "))
	  {
	    /*
	    ** $a - International Standard Serial Number (NR)
	    */

	    issn = parsedList.at(j);

	    if(issn.indexOf("$a") > -1)
	      issn = issn.mid(issn.indexOf("$a") + 2).trimmed();

	    if(issn.contains(" "))
	      issn = issn.mid(0, issn.indexOf(" ")).trimmed();

	    if(issn.length() >= 9)
	      issn = issn.mid(0, 9).trimmed();

	    if(row == -1)
	      if(m_magazine && m_magazine->dialog().id->text() == issn)
		row = i;

	    break;
	  }

      if(!issn.isEmpty())
	m_ui.list->addItem(issn);
      else
	m_ui.list->addItem
	  (QString(tr("Record #")) + QString::number(i + 1));

      m_records.append(list[i]);
    }

  list.clear();
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

biblioteq_z3950results::~biblioteq_z3950results()
{
  m_records.clear();
}

void biblioteq_z3950results::changeEvent(QEvent *event)
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

void biblioteq_z3950results::closeEvent(QCloseEvent *event)
{
  Q_UNUSED(event);
  slotClose();
}

void biblioteq_z3950results::keyPressEvent(QKeyEvent *event)
{
  if(event && event->key() == Qt::Key_Escape)
    close();

  QDialog::keyPressEvent(event);
}

void biblioteq_z3950results::slotClose(void)
{
  deleteLater();
}

void biblioteq_z3950results::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(auto widget, findChildren<QWidget *> ())
    {
      widget->setFont(font);
      widget->update();
    }

  update();
}

void biblioteq_z3950results::slotSelectRecord(void)
{
  QStringList list;

  list = m_ui.textarea->toPlainText().split("\n");

  if(m_magazine)
    m_magazine->populateDisplayAfterZ3950(list, m_recordSyntax);

  list.clear();
  close();
}

void biblioteq_z3950results::slotUpdateQueryText(void)
{
  QString title("");
  auto list(m_records.value(m_ui.list->currentRow()).split("\n"));

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

	if(title.indexOf("$a") > -1)
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

	for(int i = 0; i < subfields.size(); i++)
	  if(title.contains(subfields.at(i)))
	    {
	      title = title.mid(0, title.indexOf(subfields.at(i))).trimmed();
	      break;
	    }

	if(title.lastIndexOf('/') > -1)
	  title = title.mid(0, title.lastIndexOf('/')).trimmed();

	break;
      }

  m_ui.title->setText(title);
  m_ui.title->setCursorPosition(0);
  m_ui.textarea->setPlainText(m_records.value(m_ui.list->currentRow()));
}
