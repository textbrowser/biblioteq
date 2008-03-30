#include "borrowers_editor.h"

extern qtbook *qmain;
extern QApplication *qapp;

/*
** -- borrowers_editor() --
*/

borrowers_editor::borrowers_editor(QWidget *parent,
				   qtbook_item *bitemArg,
				   const int quantityArg,
				   const QString &ioidArg,
				   const QString &uniqueidArg,
				   const QFont &font,
				   const QString &itemTypeArg,
				   const int stateArg):
  QDialog(parent)
{
  setWindowModality(Qt::WindowModal);
  bd.setupUi(this);
  ioid = ioidArg;
  quantity = quantityArg;
  bitem = bitemArg;
  state = stateArg;
  itemType = itemTypeArg.toLower().remove(" ");

  if(state == qtbook::EDITABLE)
    {
      connect(bd.saveButton, SIGNAL(clicked(void)), this,
	      SLOT(slotSave(void)));
      connect(bd.eraseButton, SIGNAL(clicked(void)), this,
	      SLOT(slotEraseBorrower(void)));
    }
  else
    {
      bd.saveButton->setVisible(false);
      bd.eraseButton->setVisible(false);
    }

  connect(bd.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCloseCurrentBorrowers(void)));
  bd.table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
  setGlobalFonts(font);

  if(!uniqueidArg.isEmpty())
    setWindowTitle(QString("BiblioteQ: Item Reservation Status (%1)").arg
		       (uniqueidArg));
}

/*
** -- ~borrowers_editor() --
*/

borrowers_editor::~borrowers_editor()
{
}

/*
** -- showUsers() --
*/

void borrowers_editor::showUsers(void)
{
  int i = 0;
  int j = 0;
  int row = 0;
  QString str = "";
  QString tmpStr = "";
  QDateEdit *dateEdit = NULL;
  QSqlQuery query(qmain->getDB());
  QStringList list;
  QTableWidgetItem *item = NULL;
  QProgressDialog progress1(this);
  QProgressDialog progress2(this);

  str = ioid;
  bd.table->clear();
  bd.table->setColumnCount(0);
  bd.table->setRowCount(0);

  if(state == qtbook::EDITABLE)
    {
      list.append("Copy Number");
      list.append("Barcode");
      list.append("Member ID");
      list.append("First Name");
      list.append("Last Name");
      list.append("Reservation Date");
      list.append("Copy Due Date");
      list.append("Lender");
      list.append("OID");
    }
  else
    {
      list.append("Copy Number");
      list.append("Barcode");
      list.append("Reservation Date");
      list.append("Copy Due Date");
    }

  bd.table->setColumnCount(list.size());
  bd.table->setHorizontalHeaderLabels(list);
  list.clear();
  bd.table->setRowCount(quantity);
  bd.table->scrollToTop();
  bd.table->horizontalScrollBar()->setValue(0);

  /*
  ** Hide the OID column.
  */

  if(state == qtbook::EDITABLE)
    bd.table->setColumnHidden(bd.table->columnCount() - 1, true);

  if(!isVisible())
    {
      updateGeometry();
      resize(sizeHint());
    }

  show();
  progress1.setModal(true);
  progress1.setWindowTitle("BiblioteQ: Progress Dialog");
  progress1.setLabelText("Constructing objects...");
  progress1.setMaximum(quantity);
  progress1.setCancelButton(NULL);
  progress1.show();
  progress1.update();

  for(i = 0; i < quantity; i++)
    {
      for(j = 0; j < bd.table->columnCount(); j++)
	if(j == 6 && state == qtbook::EDITABLE)
	  {
	    if((dateEdit = new QDateEdit()) != NULL)
	      {
		dateEdit->setDisplayFormat("MM/dd/yyyy");
		dateEdit->setDate
		  (QDate::fromString("01/01/2000", "MM/dd/yyyy"));
		dateEdit->setCalendarPopup(true);
		bd.table->setCellWidget(i, j, dateEdit);
	      }
	    else
	      qmain->addError(QString("Memory Error"),
			      QString("Unable to allocate memory for the "
				      "\"dateEdit\" object. "
				      "This is a serious problem!"),
			      QString(""), __FILE__, __LINE__);
	  }
	else if((item = new QTableWidgetItem()) != NULL)
	  {
	    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	    if(j == 0)
	      {
		tmpStr.setNum(i + 1);
		item->setText(tmpStr);
	      }

	    bd.table->setItem(i, j, item);
	  }
	else
	  qmain->addError
	    (QString("Memory Error"),
	     QString("Unable to allocate memory for the \"item\" "
		     "object. "
		     "This is a serious problem!"),
	     QString(""), __FILE__, __LINE__);

      progress1.setValue(i + 1);
      progress1.update();
      qapp->processEvents();
    }

  bd.table->setRowCount(i);

  if(state == qtbook::EDITABLE)
    query.prepare(QString("SELECT borrowers.copy_number, "
			  "borrowers.copyid, "
			  "member.memberid, "
			  "member.first_name, "
			  "member.last_name, "
			  "borrowers.reserved_date, "
			  "borrowers.duedate, "
			  "borrowers.reserved_by, "
			  "borrowers.myoid "
			  "FROM member member, "
			  "%1_borrower borrowers "
			  "WHERE borrowers.item_oid = ? AND "
			  "borrowers.memberid = member.memberid "
			  "ORDER BY borrowers.copy_number").arg(itemType));
  else
    query.prepare(QString("SELECT borrowers.copy_number, "
			  "copy.copyid, "
			  "borrowers.reserved_date, "
			  "borrowers.duedate "
			  "FROM "
			  "%1_copy_info copy "
			  "LEFT JOIN %1_borrower_vw borrowers "
			  "ON copy.item_oid = borrowers.item_oid "
			  "WHERE copy.item_oid = ? "
			  "ORDER BY borrowers.copy_number").arg(itemType));

  query.bindValue(0, str);
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
    qmain->addError(QString("Database Error"),
		    QString("Unable to retrieve borrower data."),
		    query.lastError().text(), __FILE__, __LINE__);

  qapp->restoreOverrideCursor();
  progress2.setModal(true);
  progress2.setWindowTitle("BiblioteQ: Progress Dialog");
  progress2.setLabelText("Retrieving borrower data...");
  progress2.setMaximum(query.size());
  progress2.show();
  progress2.update();
  i = -1;

  while(i++, !progress2.wasCanceled() && query.next())
    {
      if(query.isValid())
	for(j = 0; j < bd.table->columnCount(); j++)
	  {
	    if(query.value(0).isNull())
	      row = i;
	    else
	      row = query.value(0).toInt() - 1;

	    if(j == 0 && query.value(0).isNull())
	      str = QString::number(i + 1);
	    else
	      str = query.value(j).toString();

	    if(j == 6 && state == qtbook::EDITABLE)
	      {
		if(bd.table->cellWidget(row, j) != NULL)
		  {
		    qobject_cast<QDateEdit *>(bd.table->cellWidget
					      (row, j))->setDate
		      (QDate::fromString(str, "MM/dd/yyyy"));
		    qobject_cast<QDateEdit *>(bd.table->cellWidget
					      (row, j))->setMinimumDate
		      (QDate::fromString(bd.table->item(row, 5)->text(),
					 "MM/dd/yyyy").addDays(15));
		  }
	      }
	    else if(bd.table->item(row, j) != NULL)
	      bd.table->item(row, j)->setText(str);
	  }

      progress2.setValue(i + 1);
      progress2.update();
      qapp->processEvents();
    }

  query.clear();
  bd.table->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}

/*
** -- slotEraseBorrower() --
*/

void borrowers_editor::slotEraseBorrower(void)
{
  int row = bd.table->currentRow();
  QString oid = "";
  QString copyid = "";
  QString errorstr = "";
  QString memberid = "";
  QString availability = "";
  QString returnedDate = QDateTime::currentDateTime().toString("MM/dd/yyyy");
  QSqlQuery query(qmain->getDB());

  if(row < 0)
    {
      QMessageBox::critical(this, "BiblioteQ: User Error",
			    "Please select the copy that is being returned.");
      return;
    }

  oid = misc_functions::getColumnString(bd.table, row, "OID");

  if(oid.isEmpty())
    {
      QMessageBox::critical(this, "BiblioteQ: User Error",
			    "To modify a copy's reservation status, "
			    "you must first select a reserved copy.");
      return;
    }

  if(QMessageBox::question(this, "BiblioteQ: Question",
			   "Are you sure that the copy is being returned?",
			   QMessageBox::Yes | QMessageBox::No,
			   QMessageBox::No) == QMessageBox::No)
    return;

  query.prepare(QString("DELETE FROM %1_borrower WHERE "
			"myoid = ?").arg(itemType));
  query.bindValue(0, oid);
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
    {
      qapp->restoreOverrideCursor();
      qmain->addError(QString("Database Error"),
		      QString("Unable to modify the reservation status of "
			      "the selected copy."),
		      query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(this, "BiblioteQ: Database Error",
			    "Unable to modify the reservation status of "
			    "the selected copy.");
      return;
    }
  else
    {
      /*
      ** Record the return in the history table.
      */

      copyid = misc_functions::getColumnString(bd.table, row,
					       "Barcode");
      memberid = misc_functions::getColumnString(bd.table, row,
						 "Member ID");
      query.prepare(QString("UPDATE member_history SET returned_date = ? "
			    "WHERE item_oid = ? AND copyid = ? AND "
			    "memberid = ?"));
      query.bindValue(0, returnedDate);
      query.bindValue(1, ioid);
      query.bindValue(2, copyid);
      query.bindValue(3, memberid);

      if(!query.exec())
	qmain->addError(QString("Database Error"),
			QString("Unable to modify the returned date of "
				"the selected copy."),
			query.lastError().text(), __FILE__, __LINE__);

      qapp->restoreOverrideCursor();
      qmain->updateMembersBrowser(memberid);
      qapp->setOverrideCursor(Qt::WaitCursor);
      availability = misc_functions::getAvailability(ioid, qmain->getDB(),
						     itemType, errorstr);
      qapp->restoreOverrideCursor();

      if(!errorstr.isEmpty())
	qmain->addError(QString("Database Error"),
			QString("Retrieving availability."),
			errorstr, __FILE__, __LINE__);

      misc_functions::updateColumn(qmain->getUI().table, bitem->getRow(),
				   "Availability", availability);
      misc_functions::updateColumn(qmain->getUI().table, bitem->getRow(),
				   "Due Date", "Returned");

      /*
      ** Update the Reservation History panel, if necessary.
      */

      qmain->updateReservationHistoryBrowser(memberid, ioid, copyid,
					     itemType, returnedDate);
      showUsers();
    }
}

/*
** -- slotCloseCurrentBorrowers() --
*/

void borrowers_editor::slotCloseCurrentBorrowers(void)
{
  deleteLater();
}

/*
** -- slotSave() --
*/

void borrowers_editor::slotSave(void)
{
  int i = 0;
  bool error = false;
  QDate now = QDate::currentDate();
  QString oid = "";
  QDateEdit *dueDate = NULL;
  QSqlQuery query(qmain->getDB());
  QProgressDialog progress(this);

  progress.setModal(true);
  progress.setWindowTitle("BiblioteQ: Progress Dialog");
  progress.setLabelText("Updating the due date(s)...");
  progress.setMaximum(bd.table->rowCount());
  progress.show();
  progress.update();

  for(i = 0; i < bd.table->rowCount(); i++)
    {
      oid = misc_functions::getColumnString(bd.table, i, "OID");

      if(!oid.isEmpty())
	{
	  dueDate = qobject_cast<QDateEdit *>(bd.table->cellWidget(i, 6));

	  if(dueDate->date() <= now)
	    error = true;
	  else
	    {
	      query.prepare(QString("UPDATE %1_borrower "
				    "SET duedate = ? "
				    "WHERE myoid = ?").arg(itemType));
	      query.bindValue(0, dueDate->date().toString("MM/dd/yyyy"));
	      query.bindValue(1, oid);

	      if(!query.exec())
		qmain->addError(QString("Database Error"),
				QString("Unable to update the due date."),
				query.lastError().text(), __FILE__,
				__LINE__);
	      else
		misc_functions::updateColumn
		  (qmain->getUI().table,
		   bitem->getRow(), "Due Date",
		   dueDate->date().toString("MM/dd/yyyy"));
	    }
	}

      progress.setValue(i + 1);
      progress.update();
      qapp->processEvents();
    }

  progress.hide();

  if(error)
    QMessageBox::critical(this, "BiblioteQ: User Error",
			  "Some or all of the dates were not updated "
			  "due to invalid dates.");

  if(query.lastError().isValid())
    QMessageBox::critical(this, "BiblioteQ: Database Error",
			  "Some or all of the dates were not updated "
			  "due to database errors.");
}

/*
** -- closeEvent() --
*/

void borrowers_editor::closeEvent(QCloseEvent *e)
{
  (void) e;
  slotCloseCurrentBorrowers();
}

/*
** -- setGlobalFonts() --
*/

void borrowers_editor::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(QWidget *widget, findChildren<QWidget *>())
    widget->setFont(font);
}
