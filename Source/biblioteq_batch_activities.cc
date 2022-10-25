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

#include <QComboBox>
#include <QProgressDialog>

#include "biblioteq.h"
#include "biblioteq_batch_activities.h"

QColor biblioteq_batch_activities::s_notSoOkColor =
  QColor(255, 114, 118); // Red light.
QColor biblioteq_batch_activities::s_okColor =
  QColor(144, 238, 144); // Green light.

biblioteq_batch_activities::biblioteq_batch_activities(biblioteq *parent):
  QMainWindow(parent)
{
  m_qmain = parent;
  m_ui.setupUi(this);
  connect(m_qmain,
	  SIGNAL(fontChanged(const QFont &)),
	  this,
	  SLOT(slotSetGlobalFonts(const QFont &)));
  connect(m_ui.borrow_add_row,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAddBorrowingRow(void)));
  connect(m_ui.borrow_delete_row,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotDeleteBorrowingRow(void)));
  connect(m_ui.borrow_list,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotListMembersReservedItems(void)));
  connect(m_ui.scan,
	  SIGNAL(returnPressed(void)),
	  this,
	  SLOT(slotScannedBorrowing(void)));
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
  connect(m_ui.go,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotGo(void)));
  connect(m_ui.reset,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotReset(void)));
}

void biblioteq_batch_activities::borrow(void)
{
  auto memberId(m_ui.borrow_member_id->text().trimmed());

  if(memberId.isEmpty())
    {
      m_ui.borrow_member_id->setFocus();
      m_ui.borrow_member_id->setPlaceholderText
	(tr("Please provide the patron's identifier."));
      return;
    }

  QProgressDialog progress(this);

  progress.setLabelText(tr("Borrowing item(s)..."));
  progress.setMaximum(m_ui.borrow_table->rowCount());
  progress.setMinimum(0);
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.show();
  progress.repaint();

  QString error("");
  auto expired = biblioteq_misc_functions::hasMemberExpired
    (m_qmain->getDB(), memberId, error);
  auto maximumReserved = biblioteq_misc_functions::maximumReserved
    (m_qmain->getDB(), memberId, "book"); // Only books offer maximums.

  for(int i = 0; i < m_ui.borrow_table->rowCount(); i++)
    {
      if(progress.wasCanceled())
	break;

      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      progress.repaint();
      QApplication::processEvents();

      QComboBox *category = nullptr;
      auto widget = m_ui.borrow_table->cellWidget
	(i, BorrowTableColumns::CATEGORY_COLUMN);

      if(widget)
	category = widget->findChild<QComboBox *> ();

      auto copyIdentifier = m_ui.borrow_table->item
	(i, BorrowTableColumns::COPY_IDENTIFIER_COLUMN);
      auto identifier = m_ui.borrow_table->item
	(i, BorrowTableColumns::IDENTIFIER_COLUMN);
      auto results = m_ui.borrow_table->item
	(i, BorrowTableColumns::RESULTS_COLUMN);

      if(!category || !copyIdentifier || !identifier || !results)
	{
	  if(results)
	    {
	      results->setBackground(s_notSoOkColor);
	      results->setText(tr("Critical error! Missing table item(s)."));
	    }

	  continue;
	}

      results->setBackground(copyIdentifier->background());
      results->setText("");

      if(expired)
	{
	  results->setBackground(s_notSoOkColor);

	  if(error.isEmpty())
	    results->setText(tr("Membership has expired."));
	  else
	    results->setText(tr("Cannot locate member (%1).").arg(error));

	  continue;
	}

      if(copyIdentifier->text().trimmed().isEmpty())
	{
	  results->setBackground(s_notSoOkColor);
	  results->setText(tr("Empty copy identifier."));
	  continue;
	}
      else if(identifier->text().trimmed().isEmpty())
	{
	  results->setBackground(s_notSoOkColor);
	  results->setText(tr("Empty identifier."));
	  continue;
	}

      QString error("");
      QString type("");

      if(category && category->currentText() == tr("Book"))
	type = "Book";
      else if(category && category->currentText() == tr("CD"))
	type = "CD";
      else if(category && category->currentText() == tr("DVD"))
	type = "DVD";
      else if(category && category->currentText() == tr("Grey Literature"))
	type = "Grey Literature";
      else if(category && category->currentText() == tr("Journal"))
	type = "Journal";
      else if(category && category->currentText() == tr("Magazine"))
	type = "Magazine";
      else if(category && category->currentText() == tr("Video Game"))
	type = "Video Game";

      auto available = biblioteq_misc_functions::isItemAvailable
	(error,
	 m_qmain->getDB(),
	 identifier->text(),
	 copyIdentifier->text(),
	 type);

      if(!available)
	{
	  results->setBackground(s_notSoOkColor);

	  if(error.isEmpty())
	    results->setText(tr("Item is not available for reservation."));
	  else
	    results->setText
	      (tr("Item is not available (%1) for reservation.").arg(error));

	  continue;
	}

      if(maximumReserved > 0)
	{
	  auto totalReserved = biblioteq_misc_functions::
	    getItemsReservedCounts(m_qmain->getDB(), memberId, error).
	    value("numbooks");

	  if(maximumReserved <= totalReserved)
	    {
	      results->setBackground(s_notSoOkColor);
	      results->setText
		(tr("Maximum (%1) number of reserved (%2) items exceeded.").
		 arg(maximumReserved).arg(totalReserved));
	      continue;
	    }
	}

      /*
      ** Reserve the item.
      */

      QSqlQuery query(m_qmain->getDB());
      QString errorstr("");
      auto dueDate(QDate::currentDate());
      auto itemOid = biblioteq_misc_functions::getOID
	(identifier->text(), type, m_qmain->getDB(), errorstr);
      int copyNumber = biblioteq_misc_functions::getCopyNumber
	(m_qmain->getDB(), copyIdentifier->text(), itemOid, type, errorstr);

      if(copyNumber == -1)
	{
	  results->setBackground(s_notSoOkColor);
	  results->setText(tr("Error retrieving copy number."));
	  continue;
	}

      dueDate = dueDate.addDays
	(biblioteq_misc_functions::
	 getMinimumDays(m_qmain->getDB(), type, errorstr));
      query.prepare("INSERT INTO item_borrower "
		    "(copy_number, "
		    "copyid, "
		    "duedate, "
		    "item_oid, "
		    "memberid, "
		    "reserved_by, "
		    "reserved_date, "
		    "type) "
		    "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
      query.addBindValue(copyNumber);
      query.addBindValue(copyIdentifier->text());
      query.addBindValue(dueDate.toString("MM/dd/yyyy"));
      query.addBindValue(itemOid);
      query.addBindValue(memberId);
      query.addBindValue(m_qmain->getAdminID());
      query.addBindValue(QDate::currentDate().toString("MM/dd/yyyy"));
      query.addBindValue(type);

      if(query.exec())
	{
	  copyIdentifier->setData(Qt::BackgroundRole, QVariant());
	  results->setBackground(s_okColor);
	  results->setText(tr("Reserved!"));
	}
      else
	{
	  results->setBackground(s_notSoOkColor);
	  results->setText
	    (tr("Reservation problem (%1).").arg(query.lastError().text()));
	}
    }

  progress.close();
  QApplication::processEvents();
}

void biblioteq_batch_activities::changeEvent(QEvent *event)
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

void biblioteq_batch_activities::show(QMainWindow *parent)
{
  static auto resized = false;

  if(!resized && parent)
    resize(qRound(0.70 * parent->size().width()),
	   qRound(0.80 * parent->size().height()));

  resized = true;
  biblioteq_misc_functions::center(this, parent);
  showNormal();
  activateWindow();
  raise();
  m_ui.borrow_member_id->setFocus();
}

void biblioteq_batch_activities::slotAddBorrowingRow(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  disconnect(m_ui.borrow_table,
	     SIGNAL(itemChanged(QTableWidgetItem *)),
	     this,
	     SLOT(slotBorrowItemChanged(QTableWidgetItem *)));
  m_ui.borrow_table->setRowCount(m_ui.borrow_table->rowCount() + 1);

  auto row = m_ui.borrow_table->rowCount() - 1;

  for(int i = 0; i < m_ui.borrow_table->columnCount(); i++)
    if(i == BorrowTableColumns::CATEGORY_COLUMN)
      {
	QStringList list;
	auto comboBox = new QComboBox();
	auto widget = new QWidget();

	list << tr("Book")
	     << tr("CD")
	     << tr("DVD")
	     << tr("Grey Literature")
	     << tr("Journal")
	     << tr("Magazine")
	     << tr("Video Game");
	std::sort(list.begin(), list.end());
	comboBox->addItems(list);
	comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

	auto layout = new QHBoxLayout(widget);
	auto spacer = new QSpacerItem
	  (40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);

	layout->addWidget(comboBox);
	layout->addSpacerItem(spacer);
	layout->setContentsMargins(0, 0, 0, 0);
	m_ui.borrow_table->setCellWidget(row, i, widget);
      }
    else
      {
	auto item = new QTableWidgetItem();

	if(i == BorrowTableColumns::FIELD_COLUMN ||
	   i == BorrowTableColumns::RESULTS_COLUMN)
	  item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	else
	  item->setFlags
	    (Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	m_ui.borrow_table->setItem(row, i, item);
      }

  for(int i = 0; i < m_ui.borrow_table->columnCount() - 1; i++)
    m_ui.borrow_table->resizeColumnToContents(i);

  m_ui.borrow_table->resizeRowsToContents();
  connect(m_ui.borrow_table,
	  SIGNAL(itemChanged(QTableWidgetItem *)),
	  this,
	  SLOT(slotBorrowItemChanged(QTableWidgetItem *)));
  QApplication::restoreOverrideCursor();
}

void biblioteq_batch_activities::slotBorrowItemChanged(QTableWidgetItem *item)
{
  if(!item)
    return;

  if(item->column() == BorrowTableColumns::COPY_IDENTIFIER_COLUMN)
    {
      m_ui.borrow_table->blockSignals(true);
      item->setData(Qt::BackgroundRole, QVariant());
      m_ui.borrow_table->blockSignals(false);
    }
}

void biblioteq_batch_activities::slotClose(void)
{
#ifdef Q_OS_ANDROID
  hide();
#else
  close();
#endif
}

void biblioteq_batch_activities::slotDeleteBorrowingRow(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  auto rows(biblioteq_misc_functions::selectedRows(m_ui.borrow_table));

  for(auto i = rows.size() - 1; i >= 0; i--)
    m_ui.borrow_table->removeRow(rows.at(i));

  QApplication::restoreOverrideCursor();
}

void biblioteq_batch_activities::slotGo(void)
{
  if(m_ui.tab->currentIndex() == Pages::Borrow)
    borrow();
}

void biblioteq_batch_activities::slotListMembersReservedItems(void)
{
  emit listMembersReservedItems(m_ui.borrow_member_id->text());
  show(m_qmain);
}

void biblioteq_batch_activities::slotReset(void)
{
  if(m_ui.borrow_table->rowCount() > 0)
    if(QMessageBox::question(this,
			     tr("BiblioteQ: Question"),
			     tr("Are you sure that you wish to reset?"),
			     QMessageBox::No | QMessageBox::Yes,
			     QMessageBox::No) == QMessageBox::No)
      {
	QApplication::processEvents();
	return;
      }

  m_ui.borrow_member_id->clear();
  m_ui.borrow_table->clearContents();
  m_ui.borrow_table->setRowCount(0);
  m_ui.scan->clear();
  m_ui.scan->setFocus();
  m_ui.scan_type->setCurrentIndex(0);
}

void biblioteq_batch_activities::slotScanBorrowingTimerTimeout(void)
{
  if(!m_ui.scan->text().trimmed().isEmpty())
    {
      slotAddBorrowingRow();

      QString field("");
      auto copyIdentifier = m_ui.borrow_table->item
	(m_ui.borrow_table->rowCount() - 1,
	 BorrowTableColumns::COPY_IDENTIFIER_COLUMN);

      if(copyIdentifier)
	{
	  QString type("");

	  if(m_ui.scan_type->currentText() == tr("Book"))
	    type = "Book";
	  else if(m_ui.scan_type->currentText() == tr("CD"))
	    type = "CD";
	  else if(m_ui.scan_type->currentText() == tr("DVD"))
	    type = "DVD";
	  else if(m_ui.scan_type->currentText() == tr("Grey Literature"))
	    type = "Grey Literature";
	  else if(m_ui.scan_type->currentText() == tr("Journal"))
	    type = "Journal";
	  else if(m_ui.scan_type->currentText() == tr("Magazine"))
	    type = "Magazine";
	  else if(m_ui.scan_type->currentText() == tr("Video Game"))
	    type = "Video Game";

	  auto ok = true;
	  auto str(biblioteq_misc_functions::
		   getNextCopy(field,
			       ok,
			       m_qmain->getDB(),
			       m_ui.scan->text(),
			       type));

	  m_ui.borrow_table->blockSignals(true);

	  if(ok)
	    copyIdentifier->setText(str);
	  else
	    {
	      copyIdentifier->setBackground(s_notSoOkColor);
	      copyIdentifier->setText
		(tr("A copy is not available (%1).").arg(str));
	    }

	  m_ui.borrow_table->blockSignals(false);
	}

      auto fieldItem = m_ui.borrow_table->item
	(m_ui.borrow_table->rowCount() - 1, BorrowTableColumns::FIELD_COLUMN);

      if(fieldItem)
	fieldItem->setText(field);

      auto identifier = m_ui.borrow_table->item
	(m_ui.borrow_table->rowCount() - 1,
	 BorrowTableColumns::IDENTIFIER_COLUMN);

      if(identifier)
	identifier->setText(m_ui.scan->text().trimmed());

      /*
      ** Discover the desired category. Unpleasant, yet again!
      */

      auto widget = m_ui.borrow_table->cellWidget
	(m_ui.borrow_table->rowCount() - 1,
	 BorrowTableColumns::CATEGORY_COLUMN);

      if(widget)
	{
	  auto comboBox = widget->findChild<QComboBox *> ();

	  if(comboBox)
	    {
	      comboBox->setCurrentIndex
		(comboBox->findText(m_ui.scan_type->currentText()));

	      if(comboBox->currentIndex() < 0)
		comboBox->setCurrentIndex(0);
	    }
	}
    }

  m_ui.scan->clear();
}

void biblioteq_batch_activities::slotScannedBorrowing(void)
{
  QTimer::singleShot(100, this, SLOT(slotScanBorrowingTimerTimeout(void)));
}

void biblioteq_batch_activities::slotSetGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(auto widget, findChildren<QWidget *> ())
    {
      widget->setFont(font);
      widget->update();
    }

  m_ui.borrow_table->resizeColumnsToContents();
  update();
}
