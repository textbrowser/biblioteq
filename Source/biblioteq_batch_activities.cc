#include <QComboBox>
#include <QProgressDialog>

#include "biblioteq.h"
#include "biblioteq_batch_activities.h"

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
  auto memberId(m_ui.member_id->text().trimmed());

  if(memberId.isEmpty())
    {
      m_ui.member_id->setFocus();
      m_ui.member_id->setPlaceholderText
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
    (m_qmain->getDB(), memberId, "book");

  for(int i = 0; i < m_ui.borrow_table->rowCount(); i++)
    {
      if(progress.wasCanceled())
	break;

      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      progress.repaint();
      QApplication::processEvents();

      auto copyIdentifier = m_ui.borrow_table->item
	(i, BorrowTableColumns::COPY_IDENTIFIER_COLUMN);
      auto identifier = m_ui.borrow_table->item
	(i, BorrowTableColumns::IDENTIFIER_COLUMN);
      auto results = m_ui.borrow_table->item
	(i, BorrowTableColumns::RESULTS_COLUMN);

      if(!copyIdentifier || !identifier || !results)
	continue;

      results->setText("");

      if(expired)
	{
	  results->setText(tr("Membership has expired."));
	  continue;
	}

      if(copyIdentifier->text().trimmed().isEmpty())
	{
	  results->setText(tr("Empty copy identifier."));
	  continue;
	}
      else if(identifier->text().trimmed().isEmpty())
	{
	  results->setText(tr("Empty identifier."));
	  continue;
	}

      auto available = biblioteq_misc_functions::isItemAvailable
	(m_qmain->getDB(),
	 identifier->text().remove('-'),
	 copyIdentifier->text(),
	 "Book");

      if(!available)
	{
	  results->setText(tr("Item is not available for reservation."));
	  continue;
	}

      if(maximumReserved > 0)
	{
	  auto totalReserved = biblioteq_misc_functions::
	    getItemsReservedCounts
	    (m_qmain->getDB(), memberId, error).value("numbooks");

	  if(maximumReserved <= totalReserved)
	    {
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
	(identifier->text().remove('-'), "Book", m_qmain->getDB(), errorstr);
      int copyNumber = biblioteq_misc_functions::getCopyNumber
	(m_qmain->getDB(), copyIdentifier->text(), itemOid, "Book", errorstr);

      if(copyNumber == -1)
	{
	  results->setText(tr("Error retrieving copy number."));
	  continue;
	}

      dueDate = dueDate.addDays
	(biblioteq_misc_functions::
	 getMinimumDays(m_qmain->getDB(), "Book", errorstr));
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
      query.addBindValue("Book");

      if(query.exec())
	results->setText(tr("Reserved!"));
      else
	results->setText
	  (tr("Reservation problem (%1).").arg(query.lastError().text()));
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
    resize(qRound(0.50 * parent->size().width()),
	   qRound(0.80 * parent->size().height()));

  resized = true;
  biblioteq_misc_functions::center(this, parent);
  showNormal();
  activateWindow();
  raise();
}

void biblioteq_batch_activities::slotAddBorrowingRow(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_ui.borrow_table->setRowCount(m_ui.borrow_table->rowCount() + 1);

  auto row = m_ui.borrow_table->rowCount() - 1;

  for(int i = 0; i < m_ui.borrow_table->columnCount(); i++)
    if(i == BorrowTableColumns::CATEGORY_COLUMN)
      {
	QStringList list;
	auto comboBox = new QComboBox();
	auto widget = new QWidget();

	list << tr("Book");
	/*
	  << tr("CD")
	  << tr("DVD")
	  << tr("Grey Literature")
	  << tr("Journal")
	  << tr("Magazine")
	  << tr("Video Game");
	*/
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

	if(i == BorrowTableColumns::RESULTS_COLUMN)
	  item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	else
	  item->setFlags
	    (Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	m_ui.borrow_table->setItem(row, i, item);
      }

  m_ui.borrow_table->resizeRowsToContents();
  QApplication::restoreOverrideCursor();
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

  m_ui.borrow_table->clearContents();
  m_ui.borrow_table->setRowCount(0);
  m_ui.member_id->clear();
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
