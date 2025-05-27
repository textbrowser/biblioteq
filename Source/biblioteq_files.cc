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
#include <QTimer>
#include <QtMath>

#include "biblioteq.h"
#ifdef BIBLIOTEQ_QT_PDF_SUPPORTED
#include "biblioteq_documentationwindow.h"
#endif
#include "biblioteq_files.h"
#include "biblioteq_filesize_table_item.h"
#include "biblioteq_pdfreader.h"

biblioteq_files::biblioteq_files(biblioteq *biblioteq):QMainWindow(biblioteq)
{
  m_biblioteq = biblioteq;
  m_ui.setupUi(this);
  m_ui.digests->setEnabled(false);
  m_ui.digests->setToolTip
    (tr("Requires administrator or librarian permissions."));
  m_ui.files_table->setColumnHidden(static_cast<int> (Columns::MYOID), true);
  m_ui.pages->setValue(QSettings().value("filesPerPage", 500).toInt());
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
  connect(m_ui.digests,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotComputeDigests(void)));
  connect(m_ui.export_files,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotExport(void)));
  connect(m_ui.files_table,
	  SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
	  this,
	  SLOT(slotFilesDoubleClicked(QTableWidgetItem *)));
  connect(m_ui.pages,
	  SIGNAL(valueChanged(int)),
	  this,
	  SLOT(slotPagesChanged(int)));
  connect(m_ui.refresh,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRefresh(void)));
  prepareIcons();
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

void biblioteq_files::prepareIcons(void)
{
  QSettings setting;
  auto const index = setting.value
    ("otheroptions/display_icon_set_index", 0).toInt();

  if(index == 1)
    {
      // System.

      m_ui.close->setIcon
	(QIcon::fromTheme("window-close", QIcon(":/16x16/cancel.png")));
      m_ui.export_files->setIcon
	(QIcon::fromTheme("document-export", QIcon(":/16x16/fileexport.png")));
      m_ui.refresh->setIcon
	(QIcon::fromTheme("view-refresh", QIcon(":/16x16/reload.png")));
    }
  else
    {
      // Faenza.

      m_ui.close->setIcon(QIcon(":/16x16/cancel.png"));
      m_ui.export_files->setIcon(QIcon(":/16x16/fileexport.png"));
      m_ui.refresh->setIcon(QIcon(":/16x16/reload.png"));
    }
}

void biblioteq_files::reset(void)
{
  QTimer::singleShot(1500, this, SLOT(slotEnableWidgets(void)));
  m_ui.digests->setEnabled(false);
  m_ui.digests->setToolTip
    (tr("Requires administrator or librarian permissions."));
  m_ui.files_table->setRowCount(0);
  disconnect(m_ui.page,
	     SIGNAL(valueChanged(int)),
	     this,
	     SLOT(slotRefresh(void)));
  m_ui.page->setMaximum(1);
  m_ui.page->setToolTip(tr("[1, 1]"));
  m_ui.page->setValue(1);
  connect(m_ui.page,
	  SIGNAL(valueChanged(int)),
	  this,
	  SLOT(slotRefresh(void)));
  statusBar()->showMessage(tr("0 Total Files"));
}

void biblioteq_files::showEvent(QShowEvent *event)
{
  QMainWindow::showEvent(event);
  slotEnableWidgets();
}

void biblioteq_files::slotClose(void)
{
#ifdef Q_OS_ANDROID
  hide();
#else
  close();
#endif
}

void biblioteq_files::slotComputeDigests(void)
{
  auto const list(m_ui.files_table->selectionModel()->selectedRows());

  if(list.isEmpty())
    return;

  QProgressDialog progress(this);

  progress.setLabelText(tr("Computing digests..."));
  progress.setMaximum(list.size());
  progress.setMinimum(0);
  progress.setMinimumWidth
    (qCeil(biblioteq::PROGRESS_DIALOG_WIDTH_MULTIPLIER *
	   progress.sizeHint().width()));
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.show();
  progress.repaint();
  QApplication::processEvents();

  for(int i = 0; i < list.size(); i++)
    {
      progress.setValue(i);
      progress.repaint();
      QApplication::processEvents();

      if(progress.wasCanceled())
	break;

      QSqlQuery query(m_biblioteq->getDB());
      auto const index(list.at(i));
      auto const oid = index.sibling
	(index.row(), static_cast<int> (Columns::MYOID)).data().toLongLong();
      auto const tableName
	(index.sibling(index.row(), static_cast<int> (Columns::ITEM_TYPE)).
	 data().toString());

      query.setForwardOnly(true);
      query.prepare
	(QString("SELECT file FROM %1_files WHERE myoid = ?").
	 arg(QString(tableName).replace(' ', '_')));
      query.addBindValue(oid);

      if(query.exec() && query.next())
	{
	  QBuffer buffer;
	  auto const data(qUncompress(query.value(0).toByteArray()));

	  buffer.setData(data);

	  if(buffer.open(QIODevice::ReadOnly))
	    {
	      QByteArray bytes(4096, 0);
	      QCryptographicHash digest(QCryptographicHash::Sha3_512);
	      QSqlQuery query(m_biblioteq->getDB());
	      qint64 rc = 0;

	      while((rc = buffer.read(bytes.data(), bytes.size())) > 0)
		digest.addData(bytes.mid(0, static_cast<int> (rc)));

	      query.prepare
		(QString("UPDATE %1_files SET file_digest = ? "
			 "WHERE myoid = ?").
		 arg(QString(tableName).replace(' ', '_')));
	      query.addBindValue(digest.result().toHex().constData());
	      query.addBindValue(oid);

	      if(query.exec())
		{
		  auto item = m_ui.files_table->item
		    (index.row(), static_cast<int> (Columns::DIGEST));

		  if(item)
		    {
		      m_ui.files_table->setSortingEnabled(false);
		      item->setText(digest.result().toHex());
		      m_ui.files_table->setSortingEnabled(true);
		    }
		}
	    }
	}
    }
}

void biblioteq_files::slotEnableWidgets(void)
{
  if(!m_biblioteq->getDB().isOpen())
    {
      isVisible() ?
	QTimer::singleShot(1500, this, SLOT(slotEnableWidgets(void))) :
	(void) 0;
      m_ui.digests->setEnabled(false);
      m_ui.digests->setToolTip
	(tr("Requires administrator or librarian permissions."));
      return;
    }

  m_ui.digests->setEnabled
    (m_biblioteq->getRoles() == "librarian" ||
     m_biblioteq->getRoles().contains("administrator"));
  m_ui.digests->setToolTip
    (m_ui.digests->isEnabled() ?
     tr("Compute digests of selected file(s).") :
     tr("Requires administrator or librarian permissions."));
}

void biblioteq_files::slotExport(void)
{
  auto const list(m_ui.files_table->selectionModel()->selectedRows());

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
  progress.setMinimumWidth
    (qCeil(biblioteq::PROGRESS_DIALOG_WIDTH_MULTIPLIER *
	   progress.sizeHint().width()));
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.show();
  progress.repaint();
  QApplication::processEvents();

  QSqlQuery query(m_biblioteq->getDB());
  int i = 0;

  query.setForwardOnly(true);

  foreach(auto const &index, list)
    {
      i += 1;
      progress.setValue(i);
      progress.repaint();
      QApplication::processEvents();

      if(progress.wasCanceled())
	break;

      auto const fileName(index.data().toString());
      auto const oid = index.sibling
	(index.row(), static_cast<int> (Columns::MYOID)).data().toLongLong();
      auto const tableName
	(index.sibling(index.row(), static_cast<int> (Columns::ITEM_TYPE)).
	 data().toString());

      query.prepare
	(QString("SELECT file FROM %1_files WHERE myoid = ?").
	 arg(QString(tableName).replace(' ', '_')));
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

void biblioteq_files::slotFilesDoubleClicked(QTableWidgetItem *item)
{
  Q_UNUSED(item);

#if defined(BIBLIOTEQ_LINKED_WITH_POPPLER) ||	\
    defined(BIBLIOTEQ_QT_PDF_SUPPORTED)

  auto const index
    (m_ui.files_table->selectionModel()->
     selectedRows(static_cast<int> (Columns::FILE)).value(0));

  if(index.data().toString().toLower().trimmed().endsWith(".pdf"))
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      QByteArray data;
      QSqlQuery query(m_biblioteq->getDB());
      auto const fileName(index.data().toString());
      auto const oid = index.sibling
	(index.row(), static_cast<int> (Columns::MYOID)).data().toLongLong();
      auto const tableName
	(index.sibling(index.row(), static_cast<int> (Columns::ITEM_TYPE)).
	 data().toString());

      query.prepare
	(QString("SELECT file FROM %1_files WHERE myoid = ?").
	 arg(QString(tableName).replace(' ', '_')));
      query.addBindValue(oid);

      if(query.exec() && query.next())
	data = qUncompress(query.value(0).toByteArray());

      if(!data.isEmpty())
	{
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
	  auto reader = new biblioteq_pdfreader(m_biblioteq);

	  reader->load(data, fileName);
#else
	  auto reader = new biblioteq_documentationwindow(m_biblioteq);

	  reader->load(data);
#endif
#ifdef Q_OS_ANDROID
	  reader->showMaximized();
#else
	  biblioteq_misc_functions::center(reader, this);
	  reader->show();
#endif
	}

      QApplication::restoreOverrideCursor();
    }
#endif
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
  progress.setMinimumWidth
    (qCeil(biblioteq::PROGRESS_DIALOG_WIDTH_MULTIPLIER *
	   progress.sizeHint().width()));
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.show();
  progress.repaint();
  QApplication::processEvents();
  disconnect(m_ui.page,
	     SIGNAL(valueChanged(int)),
	     this,
	     SLOT(slotRefresh(void)));
  m_ui.files_table->setRowCount(0);

  QSqlQuery query(m_biblioteq->getDB());
  auto const page = m_ui.page->value() - 1;
  int totalFiles = 0;

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
      m_ui.page->setMaximum
	(qCeil(query.value(0).toDouble() /
	       static_cast<double> (m_ui.pages->value())));
      m_ui.page->setToolTip(tr("[1, %1]").arg(m_ui.page->maximum()));
      totalFiles = query.value(0).toInt();
    }

  m_ui.page->setValue(page + 1);
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
     arg((m_ui.page->value() - 1)* m_ui.pages->value()));

  if(query.exec())
    {
      QLocale locale;
      int row = 0;

      m_ui.files_table->setRowCount(m_ui.pages->value());
      m_ui.files_table->setSortingEnabled(false);

      while(progress.wasCanceled() == false && query.next())
	{
	  for(int i = 0; i < m_ui.files_table->columnCount(); i++)
	    {
	      QTableWidgetItem *item = nullptr;

	      if(i == static_cast<int> (Columns::COMPRESSED_SIZE))
		item = new biblioteq_filesize_table_item
		  (locale.toString(query.value(i).toLongLong()));
	      else
		item = new QTableWidgetItem(query.value(i).toString());

	      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	      item->setToolTip(QString("<html>%1</html>").arg(item->text()));
	      m_ui.files_table->setItem(row, i, item);
	    }

	  progress.repaint();
	  row += 1;
	  QApplication::processEvents();
	}

      m_ui.files_table->scrollToTop();
      m_ui.files_table->setRowCount(row);
      m_ui.files_table->setSortingEnabled(true);
      m_ui.files_table->sortByColumn
	(static_cast<int> (Columns::FILE), Qt::AscendingOrder);
    }
  else
    m_ui.files_table->setRowCount(0);

  connect(m_ui.page,
	  SIGNAL(valueChanged(int)),
	  this,
	  SLOT(slotRefresh(void)));
  statusBar()->showMessage
    (tr("%1 Shown Files / %2 Total Files").
     arg(m_ui.files_table->rowCount()).arg(totalFiles));
}
