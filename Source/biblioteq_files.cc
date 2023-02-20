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

#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QSettings>
#include <QtMath>

#include "biblioteq.h"
#include "biblioteq_files.h"
#include "biblioteq_filesize_table_item.h"

biblioteq_files::biblioteq_files(biblioteq *biblioteq):QMainWindow(biblioteq)
{
  m_biblioteq = biblioteq;
  m_ui.setupUi(this);
  m_ui.files_table->setColumnHidden(MYOID, true);

  QSettings settings;

  m_ui.pages->setValue(settings.value("filesPerPage", 500).toInt());
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
  connect(m_ui.export_files,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotExport(void)));
  connect(m_ui.pages,
	  SIGNAL(valueChanged(int)),
	  this,
	  SLOT(slotPagesChanged(int)));
  connect(m_ui.refresh,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRefresh(void)));
  statusBar()->showMessage(tr("0 Total Files"));
}

biblioteq_files::~biblioteq_files()
{
}

void biblioteq_files::changeEvent(QEvent *event)
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

  QMainWindow::changeEvent(event);
}

void biblioteq_files::reset(void)
{
  disconnect(m_ui.page,
	     SIGNAL(currentIndexChanged(int)),
	     this,
	     SLOT(slotRefresh(void)));
  m_ui.files_table->setRowCount(0);
  m_ui.page->clear();
  m_ui.page->addItem("1");
  connect(m_ui.page,
	  SIGNAL(currentIndexChanged(int)),
	  this,
	  SLOT(slotRefresh(void)));
  statusBar()->showMessage(tr("0 Total Files"));
}

void biblioteq_files::slotClose(void)
{
#ifdef Q_OS_ANDROID
  hide();
#else
  close();
#endif
}

void biblioteq_files::slotExport(void)
{
  auto list(m_ui.files_table->selectionModel()->selectedRows());

  if(biblioteq::MAXIMUM_DEVICES_CONFIRMATION <= list.size())
    {
      if(QMessageBox::question(this,
			       tr("BiblioteQ: Question"),
			       tr("Are you sure that you wish to export %1 "
				  "files?").arg(list.size()),
			       QMessageBox::No | QMessageBox::Yes,
			       QMessageBox::No) == QMessageBox::No)
	{
	  QApplication::processEvents();
	  return;
	}

      QApplication::processEvents();
    }
  else if(list.isEmpty())
    return;

  QFileDialog dialog(this);

  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::Directory);
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowTitle(tr("BiblioteQ: File Export"));

  if(dialog.exec() != QDialog::Accepted)
    {
      QApplication::processEvents();
      return;
    }

  QProgressDialog progress(this);

  progress.setLabelText(tr("Exporting file(s)..."));
  progress.setMaximum(list.size());
  progress.setMinimum(0);
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.show();
  progress.repaint();
  QApplication::processEvents();

  QSqlQuery query(m_biblioteq->getDB());
  int i = 0;

  query.setForwardOnly(true);

  foreach(const auto &index, list)
    {
      i += 1;
      progress.setValue(i);
      progress.repaint();
      QApplication::processEvents();

      if(progress.wasCanceled())
	break;

      auto fileName(index.data().toString());
      auto oid = index.sibling(index.row(), 6).data().toLongLong();
      auto tableName(index.sibling(index.row(), 5).data().toString());

      query.prepare
	(QString("SELECT file FROM %1_files WHERE myoid = ?").
	 arg(tableName.replace(' ', '_')));
      query.addBindValue(oid);

      if(query.exec() && query.next())
	{
	  QFile file(dialog.directory().absolutePath() +
		     QDir::separator() +
		     fileName);

	  file.open(QIODevice::Truncate | QIODevice::WriteOnly);
	  file.write(qUncompress(query.value(0).toByteArray()));
	  file.close();
	}
    }
}

void biblioteq_files::slotPagesChanged(int value)
{
  QSettings settings;

  settings.setValue("filesPerPage", value);
}

void biblioteq_files::slotRefresh(void)
{
  if(!m_biblioteq->getDB().isOpen())
    return;

  QProgressDialog progress(this);

  progress.setLabelText(tr("Populating..."));
  progress.setMaximum(0);
  progress.setMinimum(0);
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.show();
  progress.repaint();
  QApplication::processEvents();
  disconnect(m_ui.page,
	     SIGNAL(currentIndexChanged(int)),
	     this,
	     SLOT(slotRefresh(void)));
  m_ui.files_table->setRowCount(0);

  QSqlQuery query(m_biblioteq->getDB());
  auto page = m_ui.page->currentIndex();

  m_ui.page->clear();
  query.setForwardOnly(true);
  query.prepare
    ("SELECT (SELECT COUNT(*) FROM book_files) "
     "+ "
     "(SELECT COUNT(*) FROM grey_literature_files) "
     "+ "
     "(SELECT COUNT(*) FROM journal_files) "
     "+ "
     "(SELECT COUNT(*) FROM magazine_files) AS totalCount");

  if(query.exec() && query.next())
    {
      auto pages = qCeil
	(query.value(0).toDouble() / static_cast<double> (m_ui.pages->value()));

      for(int i = 0; i < pages; i++)
	m_ui.page->addItem(QString::number(i + 1));

      m_ui.files_table->setRowCount(query.value(0).toInt());
    }

  if(m_ui.page->count() == 0)
    m_ui.page->addItem("1");

  page = qBound(0, page, m_ui.page->count() - 1);
  m_ui.page->setCurrentIndex(page);
  query.prepare
    (QString("SELECT bf.file_name, "
	     "bf.description, "
	     "bf.file_digest, "
	     "LENGTH(bf.file), "
	     "b.title, "
	     "'book', "
	     "bf.myoid "
	     "FROM book b, book_files bf "
	     "WHERE b.myoid = bf.item_oid "
	     "UNION ALL "
	     "SELECT gf.file_name, "
	     "gf.description, "
	     "gf.file_digest, "
	     "LENGTH(gf.file), "
	     "g.document_title, "
	     "'grey literature', "
	     "gf.myoid "
	     "FROM grey_literature g, grey_literature_files gf "
	     "WHERE g.myoid = gf.item_oid "
	     "UNION ALL "
	     "SELECT jf.file_name, "
	     "jf.description, "
	     "jf.file_digest, "
	     "LENGTH(jf.file), "
	     "j.title, "
	     "'journal', "
	     "jf.myoid "
	     "FROM journal j, journal_files jf "
	     "WHERE j.myoid = jf.item_oid "
	     "UNION ALL "
	     "SELECT mf.file_name, "
	     "mf.description, "
	     "mf.file_digest, "
	     "LENGTH(mf.file), "
	     "m.title, "
	     "'magazine', "
	     "mf.myoid "
	     "FROM magazine m, magazine_files mf "
	     "WHERE m.myoid = mf.item_oid "
	     "LIMIT %1 OFFSET %2").
     arg(m_ui.pages->value()).
     arg(m_ui.page->currentIndex() * m_ui.pages->value()));
  statusBar()->showMessage
    (tr("%1 Total Files").arg(m_ui.files_table->rowCount()));

  if(query.exec())
    {
      QLocale locale;
      int row = 0;
      int totalRows = 0;

      m_ui.files_table->setSortingEnabled(false);

      while(!progress.wasCanceled() &&
	    query.next() &&
	    totalRows < m_ui.files_table->rowCount())
	{
	  totalRows += 1;

	  for(int i = 0; i < m_ui.files_table->columnCount(); i++)
	    {
	      QTableWidgetItem *item = nullptr;

	      if(i == COMPRESSED_SIZE)
		item = new biblioteq_filesize_table_item
		  (locale.toString(query.value(i).toLongLong()));
	      else
		item = new QTableWidgetItem(query.value(i).toString());

	      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	      item->setToolTip(item->text());
	      m_ui.files_table->setItem(row, i, item);
	    }

	  progress.repaint();
	  row += 1;
	  QApplication::processEvents();
	}

      m_ui.files_table->scrollToTop();
      m_ui.files_table->setRowCount(totalRows);
      m_ui.files_table->setSortingEnabled(true);
      m_ui.files_table->sortByColumn(FILE, Qt::AscendingOrder);
    }

  connect(m_ui.page,
	  SIGNAL(currentIndexChanged(int)),
	  this,
	  SLOT(slotRefresh(void)));
}
