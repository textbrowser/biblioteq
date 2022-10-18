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
#include <QProgressBar>
#include <QProgressDialog>
#include <QPushButton>
#include <QSqlRecord>

#include "biblioteq.h"
#include "biblioteq_sqlite_merge_databases.h"

biblioteq_sqlite_merge_databases::biblioteq_sqlite_merge_databases
(biblioteq *parent):QMainWindow(parent)
{
  m_qmain = parent;
  m_ui.setupUi(this);
  m_ui.splitter->setStretchFactor(0, 1);
  m_ui.splitter->setStretchFactor(1, 0);
  connect(m_qmain,
	  SIGNAL(fontChanged(const QFont &)),
	  this,
	  SLOT(slotSetGlobalFonts(const QFont &)));
  connect(m_ui.add_row,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAddRow(void)));
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(close(void)));
  connect(m_ui.delete_row,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotDeleteRow(void)));
  connect(m_ui.merge,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotMerge(void)));
  connect(m_ui.reset,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotReset(void)));
}

void biblioteq_sqlite_merge_databases::changeEvent(QEvent *event)
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
	{
	  break;
	}
      }

  QMainWindow::changeEvent(event);
}

void biblioteq_sqlite_merge_databases::slotAddRow(void)
{
  m_ui.databases->setRowCount(m_ui.databases->rowCount() + 1);

  auto row = m_ui.databases->rowCount() - 1;

  for(int i = 0; i < m_ui.databases->columnCount(); i++)
    switch(i)
      {
      case PROGRESS_COLUMN:
	{
	  auto progress = new QProgressBar();

	  m_ui.databases->setCellWidget(row, i, progress);
	  break;
	}
      case SELECT_COLUMN:
	{
	  auto pushButton = new QPushButton(tr("Select SQLite Database"));

	  connect(pushButton,
		  SIGNAL(clicked(void)),
		  this,
		  SLOT(slotSelect(void)));
	  m_ui.databases->setCellWidget(row, i, pushButton);
	  break;
	}
      case SQLITE_DATABASE_COLUMN:
	{
	  auto item = new QTableWidgetItem();

	  item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	  m_ui.databases->setItem(row, i, item);
	}
      default:
	{
	  break;
	}
      }
}

void biblioteq_sqlite_merge_databases::slotDeleteRow(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  auto rows(biblioteq_misc_functions::selectedRows(m_ui.databases));

  for(auto i = rows.size() - 1; i >= 0; i--)
    m_ui.databases->removeRow(rows.at(i));

  QApplication::restoreOverrideCursor();
}

void biblioteq_sqlite_merge_databases::slotMerge(void)
{
  if(!m_qmain || m_ui.databases->rowCount() == 0)
    return;

  QScopedPointer<QProgressDialog> progress(new QProgressDialog(this));

  progress->setLabelText(tr("Merging databases..."));
  progress->setMaximum(0);
  progress->setMinimum(0);
  progress->setModal(true);
  progress->setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress->show();
  progress->repaint();
  QApplication::processEvents();

  QString errors("<html>");
  auto tables(QStringList()
	      << "book"
	      << "book_binding_types"
	      << "book_copy_info"
	      << "book_files"
	      << "book_sequence"
	      << "cd"
	      << "cd_copy_info"
	      << "cd_formats"
	      << "cd_songs"
	      << "dvd"
	      << "dvd_aspect_ratios"
	      << "dvd_copy_info"
	      << "dvd_ratings"
	      << "dvd_regions"
	      << "grey_literature"
	      << "grey_literature_files"
	      << "grey_literature_types"
	      << "item_borrower"
	      << "journal"
	      << "journal_copy_info"
	      << "journal_files"
	      << "languages"
	      << "locations"
	      << "magazine"
	      << "magazine_copy_info"
	      << "magazine_files"
	      << "member"
	      << "member_history"
	      << "minimum_days"
	      << "monetary_units"
	      << "photograph"
	      << "photograph_collection"
	      << "sequence"
	      << "videogame"
	      << "videogame_copy_info"
	      << "videogame_platforms"
	      << "videogame_ratings");
  int ct = 0;

  for(int i = 0; i < m_ui.databases->rowCount(); i++)
    {
      if(progress->wasCanceled())
	break;

      progress->repaint();
      QApplication::processEvents();

      auto item = m_ui.databases->item(i, SQLITE_DATABASE_COLUMN);
      auto progressBar = qobject_cast<QProgressBar *>
	(m_ui.databases->cellWidget(i, PROGRESS_COLUMN));

      if(!QFileInfo(item->text()).isReadable() || !item || !progressBar)
	continue;

      progressBar->setMaximum(tables.size());
      progressBar->setMinimum(0);

      auto dbName(item->text());

      {
	auto db = QSqlDatabase::addDatabase("QSQLITE", dbName);

	db.setDatabaseName(dbName);

	if(db.open())
	  {
	    for(int j = 0; j < tables.size(); j++)
	      {
		progressBar->setValue(j + 1);
		progressBar->repaint();
		QApplication::processEvents();

		QSqlQuery query(db);

		if(query.exec(QString("SELECT * FROM %1").arg(tables.at(j))))
		  while(query.next())
		    {
		      QList<QVariant> values;
		      QString str1("");
		      QString str2("");
		      QVariant myoid;

		      auto record(query.record());

		      for(int k = 0; k < record.count(); k++)
			{
			  str1 += record.fieldName(k) + ",";
			  str2 += "?,";
			  values << record.value(k);

			  if(record.fieldName(k) == "myoid")
			    myoid = record.value(k);
			}

		      if(str1.endsWith(','))
			str1 = str1.mid(0, str1.length() - 1);

		      if(str2.endsWith(','))
			str2 = str2.mid(0, str2.length() - 1);

		      QSqlQuery q(m_qmain->getDB());

		      q.prepare
			(QString("INSERT INTO %1 (%2) VALUES (%3)").
			 arg(tables.at(j)).arg(str1).arg(str2));

		      for(int k = 0; k < values.size(); k++)
			q.addBindValue(values.at(k));

		      if(!q.exec())
			{
			  ct += 1;
			  errors.append
			    (tr("<font color='red'>Error %1: %2. "
				"Statement: %3, myoid %4.</font>").
			     arg(ct).
			     arg(q.lastError().text().toLower()).
			     arg(q.lastQuery()).
			     arg(myoid.toString()));
			  errors.append("<br><br>");
			}
		    }
	      }
	  }

	db.close();
      }

      QSqlDatabase::removeDatabase(dbName);
    }

  errors.append("</html>");
  m_ui.results->setHtml(errors);
  progress->close();
  QApplication::processEvents();
}

void biblioteq_sqlite_merge_databases::slotReset(void)
{
  m_ui.databases->clearContents();
  m_ui.databases->setRowCount(0);
  m_ui.results->clear();
}

void biblioteq_sqlite_merge_databases::slotSelect(void)
{
  auto pushButton = qobject_cast<QPushButton *> (sender());

  if(!pushButton)
    return;

  QFileDialog dialog(this);

  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter("SQLite Database (*.sqlite)");
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowTitle(tr("BiblioteQ: SQLite Database Selection"));
  dialog.exec();
  QApplication::processEvents();

  if(dialog.result() == QDialog::Accepted)
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      for(int i = 0; i < m_ui.databases->rowCount(); i++)
	if(m_ui.databases->cellWidget(i, SELECT_COLUMN) == pushButton)
	  if(m_ui.databases->item(i, SQLITE_DATABASE_COLUMN))
	    {
	      m_ui.databases->item(i, SQLITE_DATABASE_COLUMN)->
		setText(dialog.selectedFiles().value(0));
	      m_ui.databases->item(i, SQLITE_DATABASE_COLUMN)->setToolTip
		(dialog.selectedFiles().value(0));
	      break;
	    }

      QApplication::restoreOverrideCursor();
    }
}

void biblioteq_sqlite_merge_databases::slotSetGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(auto widget, findChildren<QWidget *> ())
    {
      widget->setFont(font);
      widget->update();
    }

  update();
}
