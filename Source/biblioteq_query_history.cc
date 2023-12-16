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

#include <QClipboard>

#include "biblioteq.h"
#include "biblioteq_query_history.h"

biblioteq_query_history::biblioteq_query_history(biblioteq *biblioteq):
  QDialog(biblioteq)
{
  m_biblioteq = biblioteq;
  m_ui.setupUi(this);
  connect(m_ui.clipboard,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotCopy(void)));
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
  connect(m_ui.table,
	  SIGNAL(doubleClicked(const QModelIndex &)),
	  this,
	  SLOT(slotExecuteQuery(const QModelIndex &)));
}

biblioteq_query_history::~biblioteq_query_history()
{
}

void biblioteq_query_history::changeEvent(QEvent *event)
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

void biblioteq_query_history::reset(void)
{
  m_ui.table->setRowCount(0);
}

void biblioteq_query_history::slotClose(void)
{
#ifdef Q_OS_ANDROID
  hide();
#else
  close();
#endif
}

void biblioteq_query_history::slotCopy(void)
{
  auto clipboard = QApplication::clipboard();

  if(!clipboard)
    return;

  if(m_ui.table->item(m_ui.table->currentRow(), 1))
    clipboard->setText(m_ui.table->item(m_ui.table->currentRow(), 1)->text());
}

void biblioteq_query_history::slotExecuteQuery(const QModelIndex &index)
{
  auto query(index.sibling(index.row(), 1).data().toString().trimmed());

  if(query.isEmpty())
    return;

  auto q(query.toLower());

  if(q.startsWith("alter ") ||
     q.startsWith("cluster ") ||
     q.startsWith("create " ) ||
     q.startsWith("drop ") ||
     q.startsWith("grant ") ||
     q.startsWith("insert ") ||
     q.startsWith("lock ") ||
     q.startsWith("revoke ") ||
     q.startsWith("truncate "))
    {
      QMessageBox::critical
	(this,
	 tr("BiblioteQ: User Error"),
	 tr("Please provide a non-destructive SQL statement."));
      QApplication::processEvents();
    }
  else if(q.startsWith("delete ") || q.startsWith("update "))
    {
      if(QMessageBox::
	 question(this,
		  tr("BiblioteQ: Question"),
		  tr("Are you sure that you wish to execute the statement?"),
		  QMessageBox::No | QMessageBox::Yes,
		  QMessageBox::No) == QMessageBox::No)
	{
	  QApplication::processEvents();
	  return;
	}
      else
	QApplication::processEvents();
    }

  emit executeQuery(query);
}

void biblioteq_query_history::slotQueryCompleted(const QString &text)
{
  if(text.trimmed().isEmpty())
    return;

  auto item1 = new QTableWidgetItem
    (QDateTime::currentDateTime().toString(Qt::ISODate));
  auto item2 = new QTableWidgetItem(text.trimmed());

  item1->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  item2->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  m_ui.table->setRowCount(m_ui.table->rowCount() + 1);
  m_ui.table->setItem(m_ui.table->rowCount() - 1, 0, item1);
  m_ui.table->setItem(m_ui.table->rowCount() - 1, 1, item2);
  m_ui.table->scrollToBottom();
}
