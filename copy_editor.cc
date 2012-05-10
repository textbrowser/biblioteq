/*
** -- Local Includes --
*/

#include "qtbook.h"
#include "copy_editor.h"

extern qtbook *qmain;
extern QApplication *qapp;

/*
** -- copy_editor() --
*/

copy_editor::copy_editor(QWidget *parent, qtbook_item *bitemArg,
			 const bool showForLendingArg,
			 const int quantityArg, const QString &ioidArg,
			 QSpinBox *spinboxArg,
			 const QFont &font,
			 const QString &itemTypeArg): QDialog(parent)
{
  if(parent == qmain->getMembersBrowser())
    setWindowModality(Qt::ApplicationModal);
  else
    setWindowModality(Qt::WindowModal);

  cb.setupUi(this);
#ifdef Q_WS_MAC
  setAttribute(Qt::WA_MacMetalStyle, true);
#endif
  bitem = bitemArg;
  ioid = ioidArg;
  quantity = quantityArg;
  spinbox = spinboxArg;
  itemType = itemTypeArg;
  showForLending = showForLendingArg;
  cb.table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
  setWindowTitle
    (tr("BiblioteQ: Copy Browser (%1)").arg(ioid));
  setGlobalFonts(font);
}

/*
** -- ~copy_editor() --
*/

copy_editor::~copy_editor()
{
  clearCopiesList();
}

/*
** -- slotDeleteCopy() --
*/

void copy_editor::slotDeleteCopy(void)
{
  int row = cb.table->currentRow();
  bool isCheckedOut = false;
  QString copyid = "";
  QString errorstr = "";

  if(row < 0)
    {
      QMessageBox::critical(this, tr("BiblioteQ: User Error"),
			    tr("Please select the copy that you intend to "
			       "delete."));
      return;
    }
  else if(cb.table->rowCount() == 1)
    {
      QMessageBox::critical(this, tr("BiblioteQ: User Error"),
			    tr("You must have at least one copy."));
      return;
    }

  copyid = misc_functions::getColumnString(cb.table, row, tr("Barcode"));
  qapp->setOverrideCursor(Qt::WaitCursor);
  isCheckedOut = misc_functions::isCopyCheckedOut(qmain->getDB(),
						  copyid,
						  ioid,
						  itemType,
						  errorstr);
  qapp->restoreOverrideCursor();

  if(isCheckedOut)
    {
      if(cb.table->item(row, 1) != 0)
	cb.table->item(row, 1)->setFlags(0);

      if(cb.table->item(row, 2) != 0)
	{
	  cb.table->item(row, 2)->setFlags(0);
	  cb.table->item(row, 2)->setText("0");
	}

      QMessageBox::critical(this, tr("BiblioteQ: User Error"),
			    tr("It appears that the copy you selected to "
			       "delete is reserved."));
      return;
    }
  else if(errorstr.length() > 0)
    {
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to determine the reservation "
				 "status of the selected copy.")),
		      errorstr, __FILE__, __LINE__);
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to determine the reservation status "
			       "of the selected copy."));
      return;
    }

  cb.table->removeRow(cb.table->currentRow());
}

/*
** -- populateCopiesEditor() --
*/

void copy_editor::populateCopiesEditor(void)
{
  int i = 0;
  int j = 0;
  int row = 0;
  bool terminate = false;
  QString str = "";
  QString querystr = "";
  QSqlQuery query(qmain->getDB());
  QStringList list;
  QTableWidgetItem *item = 0;

  cb.dueDateFrame->setVisible(showForLending);
  cb.deleteButton->setVisible(!showForLending);

  if(!showForLending)
    {
      cb.saveButton->setText(tr("&Save"));
      connect(cb.saveButton, SIGNAL(clicked(void)), this,
	      SLOT(slotSaveCopies(void)));
      connect(cb.deleteButton, SIGNAL(clicked(void)), this,
	      SLOT(slotDeleteCopy(void)));
    }
  else
    {
      /*
      ** Set the minimum date to duedate.
      */

      QDate duedate = QDate::currentDate();
      QString errorstr("");

      qapp->setOverrideCursor(Qt::WaitCursor);
      duedate = duedate.addDays
	(misc_functions::getMinimumDays
	 (qmain->getDB(),
	  itemType,
	  errorstr));
      qapp->restoreOverrideCursor();

      if(!errorstr.isEmpty())
	qmain->addError(QString(tr("Database Error")),
			QString(tr("Unable to retrieve "
				   "the minimum number of days.")),
			errorstr, __FILE__, __LINE__);

      cb.dueDate->setMinimumDate(duedate);
      cb.saveButton->setText(tr("&Reserve"));
      connect(cb.saveButton, SIGNAL(clicked(void)), this,
	      SLOT(slotCheckoutCopy(void)));
    }

  connect(cb.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCloseCopyEditor(void)));

  QProgressDialog progress1(this);
  QProgressDialog progress2(this);

#ifdef Q_WS_MAC
  progress1.setAttribute(Qt::WA_MacMetalStyle, true);
  progress2.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
  cb.table->clear();
  cb.table->setColumnCount(0);
  cb.table->setRowCount(0);
  list.append(tr("Title"));
  list.append(tr("Barcode"));
  list.append(tr("Availability"));
  list.append("ITEM_OID");
  list.append("Copy Number");
  cb.table->setColumnCount(list.size());
  cb.table->setHorizontalHeaderLabels(list);
  list.clear();
  cb.table->setRowCount(quantity);
  cb.table->scrollToTop();
  cb.table->horizontalScrollBar()->setValue(0);

  /*
  ** Hide the Copy Number and ITEM_OID columns.
  */

  cb.table->setColumnHidden(cb.table->columnCount() - 1, true);
  cb.table->setColumnHidden(cb.table->columnCount() - 2, true);
  updateGeometry();
  show();
  progress1.setModal(true);
  progress1.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress1.setLabelText(tr("Constructing objects..."));
  progress1.setMaximum(quantity);
  progress1.setMinimum(0);
  progress1.show();
  progress1.update();

  for(i = 0; i < quantity && !progress1.wasCanceled(); i++)
    {
      for(j = 0; j < cb.table->columnCount(); j++)
	if((item = new(std::nothrow) QTableWidgetItem()) != 0)
	  {
	    if(showForLending)
	      item->setFlags(0);
	    else if(j == 1)
	      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled |
			     Qt::ItemIsEditable);
	    else
	      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	    if(j == 1)
	      {
		str = QString("CopyIdentifier-%1-%2").arg(ioid).arg(i + 1);
		item->setText(str);
	      }
	    else if(j == 2)
	      {
		if(showForLending)
		  item->setText("0");
		else
		  item->setText("1");
	      }
	    else if(j == 3)
	      item->setText(ioid);
	    else
	      item->setText("");

	    cb.table->setItem(i, j, item);
	    cb.table->resizeColumnToContents(j);
	  }
	else
	  qmain->addError(QString(tr("Memory Error")),
			  QString(tr("Unable to allocate memory for the "
				     "\"item\" object. "
				     "This is a serious problem!")),
			  QString(""), __FILE__, __LINE__);

      if(i + 1 <= progress1.maximum())
	progress1.setValue(i + 1);

      progress1.update();
#ifndef Q_WS_MAC
      qapp->processEvents();
#endif
    }

  progress1.hide();
  cb.table->setRowCount(i);
  querystr = QString
    ("SELECT %1.title, "
     "%1_copy_info.copyid, "
     "(1 - COUNT(item_borrower_vw.copyid)), "
     "%1_copy_info.item_oid, "
     "%1_copy_info.copy_number "
     "FROM "
     "%1, "
     "%1_copy_info LEFT JOIN item_borrower_vw ON "
     "%1_copy_info.copyid = "
     "item_borrower_vw.copyid AND "
     "%1_copy_info.item_oid = "
     "item_borrower_vw.item_oid AND "
     "item_borrower_vw.type = '%3' "
     "WHERE %1_copy_info.item_oid = %2 AND "
     "%1.myoid = %2 "
     "GROUP BY %1.title, "
     "%1_copy_info.copyid, "
     "%1_copy_info.item_oid, "
     "%1_copy_info.copy_number "
     "ORDER BY %1_copy_info.copy_number").arg
    (itemType.toLower().remove(" ")).arg(ioid).arg(itemType);
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(querystr))
    qmain->addError(QString(tr("Database Error")),
		    QString(tr("Unable to retrieve copy data.")),
		    query.lastError().text(), __FILE__, __LINE__);

  qapp->restoreOverrideCursor();
  progress2.setModal(true);
  progress2.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress2.setLabelText(tr("Retrieving copy information..."));
  progress2.setMinimum(0);

  /*
  ** SQLite does not support query.size().
  */

  if(qmain->getDB().driverName() == "QSQLITE")
    progress2.setMaximum(misc_functions::sqliteQuerySize(querystr,
							 qmain->getDB(),
							 __FILE__, __LINE__));
  else
    progress2.setMaximum(query.size());

  progress2.show();
  progress2.update();
  i = -1;

  while(i++, !progress2.wasCanceled() && query.next())
    {
      if(query.isValid())
	{
	  row = query.value(4).toInt() - 1;

	  for(j = 0; j < cb.table->columnCount(); j++)
	    if(cb.table->item(row, j) != 0)
	      {
		str = query.value(j).toString();

		if(query.value(2).toString() == "0")
		  cb.table->item(row, j)->setFlags(0);
		else if(showForLending)
		  {
		    cb.table->item(row, j)->setFlags
		      (Qt::ItemIsSelectable | Qt::ItemIsEnabled);

		    if(cb.table->currentRow() == -1)
		      cb.table->selectRow(row);
		  }
		else if(j == 1)
		  cb.table->item(row, 1)->setFlags
		    (Qt::ItemIsSelectable | Qt::ItemIsEnabled |
		     Qt::ItemIsEditable);

		if(j == 2)
		  cb.table->item(row, 2)->setText(query.value(2).toString());
		else
		  {
		    if(i == 0 && j == 0)
		      cb.table->item(row, j)->setText(str);
		    else if(j != 0)
		      cb.table->item(row, j)->setText(str);
		  }

		cb.table->resizeColumnToContents(j);
	      }
	    else
	      terminate = true;
	}

      if(i + 1 <= progress2.maximum())
	progress2.setValue(i + 1);

      progress2.update();
#ifndef Q_WS_MAC
      qapp->processEvents();
#endif
      if(terminate)
	break; // Out of resources?
    }

  progress2.hide();
  cb.table->resizeColumnsToContents();
}

/*
** -- slotCheckoutCopy() --
*/

void copy_editor::slotCheckoutCopy(void)
{
  int copyrow = cb.table->currentRow();
  int memberrow = qmain->getBB().table->currentRow();
  bool available = false;
  QDate now = QDate::currentDate();
  QString name = "";
  QString copyid = "";
  QString duedate = cb.dueDate->date().toString("MM/dd/yyyy");
  QString errorstr = "";
  QString memberid = "";
  QString checkedout = now.toString("MM/dd/yyyy");
  QString copynumber = "";
  QString availability = "";
  QSqlQuery query(qmain->getDB());

  if(copyrow < 0 || cb.table->item(copyrow, 1) == 0)
    {
      QMessageBox::critical(this, tr("BiblioteQ: User Error"),
			    tr("Please select a copy to reserve."));
      return;
    }
  else if((cb.table->item(copyrow, 1)->flags() & Qt::ItemIsEnabled) == 0)
    {
      QMessageBox::critical(this, tr("BiblioteQ: User Error"),
			    tr("It appears that the copy you've selected "
			       "is either unavailable or does not exist."));
      return;
    }
  else if(cb.dueDate->date() <= now)
    {
      QMessageBox::critical(this, tr("BiblioteQ: User Error"),
			    tr("Please select a future Due Date."));
      return;
    }

  memberid = misc_functions::getColumnString
    (qmain->getBB().table, memberrow, QString(tr("Member ID")));

  /*
  ** The "Copy Number" does not require tr().
  */

  copynumber = misc_functions::getColumnString(cb.table, copyrow,
					       QString("Copy Number"));
  copyid = misc_functions::getColumnString(cb.table, copyrow,
					   QString(tr("Barcode")));
  qapp->setOverrideCursor(Qt::WaitCursor);
  available = misc_functions::isCopyAvailable(qmain->getDB(), ioid, copyid,
					      itemType, errorstr);
  qapp->restoreOverrideCursor();

  if(!available && errorstr.length() > 0)
    {
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to determine the selected copy's "
				 "availability.")),
		      errorstr, __FILE__, __LINE__);
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to determine the selected copy's "
			       "availability."));
      return;
    }
  else if(!available)
    {
      QMessageBox::critical(this, tr("BiblioteQ: User Error"),
			    tr("The copy that you have selected is either "
			       "unavailable or is reserved."));
      return;
    }

  query.prepare("INSERT INTO item_borrower "
		"(item_oid, "
		"memberid, "
		"reserved_date, "
		"duedate, "
		"copyid, "
		"copy_number, "
		"reserved_by, "
		"type) "
		"VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
  query.bindValue(0, ioid);
  query.bindValue(1, memberid);
  query.bindValue(2, checkedout);
  query.bindValue(3, duedate);
  query.bindValue(4, copyid);
  query.bindValue(5, copynumber);
  query.bindValue(6, qmain->getAdminID());
  query.bindValue(7, itemType);
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
    {
      qapp->restoreOverrideCursor();
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to create a reserve record.")),
		      query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to create a reserve record."));
      return;
    }

  /*
  ** Record the reservation in the member's history
  ** table.
  */

  query.prepare("INSERT INTO member_history "
		"(memberid, "
		"item_oid, "
		"copyid, "
		"reserved_date, "
		"duedate, "
		"returned_date, "
		"reserved_by, "
		"type) "
		"VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
  query.bindValue(0, memberid);
  query.bindValue(1, ioid);
  query.bindValue(2, copyid);
  query.bindValue(3, checkedout);
  query.bindValue(4, duedate);
  query.bindValue(5, QString("N/A"));
  query.bindValue(6, qmain->getAdminID());
  query.bindValue(7, itemType);

  if(!query.exec())
    qmain->addError(QString(tr("Database Error")),
		    QString(tr("Unable to create a history record.")),
		    query.lastError().text(), __FILE__, __LINE__);

  /*
  ** Update the Reserved Items count on the Members Browser.
  */

  qmain->updateMembersBrowser();
  qapp->restoreOverrideCursor();

  /*
  ** Update the availability column.
  */

  /*
    availability = misc_functions::getColumnString
    (qmain->getUI().table, bitem->getRow(), tr("Availability"));

    if(availability != "0")
    availability.setNum(availability.toInt() - 1);

    misc_functions::updateColumn
    (qmain->getUI().table, bitem->getRow(), tr("Availability"), availability);
  */

  /*
  ** Update some additional columns.
  */

  /*
    if(misc_functions::getColumnString(qmain->getUI().table, bitem->getRow(),
    tr("Barcode")) == copyid)
    {
    misc_functions::updateColumn
    (qmain->getUI().table, bitem->getRow(), tr("Due Date"),
    QDate::fromString(duedate, "MM/dd/yyyy").
    toString(Qt::SystemLocaleShortDate));
    misc_functions::updateColumn
    (qmain->getUI().table, bitem->getRow(), tr("Reservation Date"),
    QDate::fromString(checkedout, "MM/dd/yyyy").toString
    (Qt::SystemLocaleShortDate));
    misc_functions::updateColumn
    (qmain->getUI().table, bitem->getRow(), tr("Member ID"),
    memberid);
    qapp->setOverrideCursor(Qt::WaitCursor);
    name = misc_functions::getMemberName(qmain->getDB(), memberid,
    errorstr);
    qapp->restoreOverrideCursor();

    if(errorstr.length() > 0)
    qmain->addError(QString(tr("Database Error")),
    QString(tr("Unable to determine the selected copy's "
    "availability.")),
    errorstr, __FILE__, __LINE__);

    misc_functions::updateColumn
    (qmain->getUI().table, bitem->getRow(), tr("Borrower"), name);
    }
  */

  slotCloseCopyEditor();

  /*
  ** Update the main window's summary panel, if necessary.
  */

  if(ioid == misc_functions::getColumnString(qmain->getUI().table,
					     bitem->getRow(), "MYOID"))
    qmain->slotDisplaySummary();
}

/*
** -- slotSaveCopies() --
*/

void copy_editor::slotSaveCopies(void)
{
  int i = 0;
  QString str = "";
  QString errormsg = "";
  QString errorstr = "";
  QString availability = "";
  copy_class *copy = 0;
  QStringList duplicates;
  QTableWidgetItem *item1 = 0;
  QTableWidgetItem *item2 = 0;

  cb.table->setFocus();

  for(i = 0; i < cb.table->rowCount(); i++)
    if(cb.table->item(i, 1) != 0 &&
       cb.table->item(i, 1)->text().trimmed().isEmpty())
      {
	errormsg = QString(tr("Row number ")) + QString::number(i + 1) +
	  tr(" contains an empty Barcode.");
	QMessageBox::critical(this, tr("BiblioteQ: User Error"), errormsg);
	duplicates.clear();
	return;
      }
    else if(cb.table->item(i, 1) != 0)
      {
	if(duplicates.contains(cb.table->item(i, 1)->text()))
	  {
	    errormsg = QString(tr("Row number ")) + QString::number(i + 1) +
	      tr(" contains a duplicate Barcode.");
	    QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				  errormsg);
	    duplicates.clear();
	    return;
	  }
	else
	  duplicates.append(cb.table->item(i, 1)->text());
      }

  duplicates.clear();

  while(!copies.isEmpty())
    delete copies.takeFirst();

  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!qmain->getDB().transaction())
    {
      qapp->restoreOverrideCursor();
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to create a database transaction.")),
		      qmain->getDB().lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to create a database transaction."));
      return;
    }

  qapp->restoreOverrideCursor();

  for(i = 0; i < cb.table->rowCount(); i++)
    {
      item1 = cb.table->item(i, 1);
      item2 = cb.table->item(i, 3);

      if(item1 == 0 || item2 == 0)
	continue;

      if((copy = new(std::nothrow) copy_class(item1->text().trimmed(),
					      item2->text())) != 0)
	copies.append(copy);
      else
	qmain->addError
	  (QString(tr("Memory Error")),
	   QString(tr("Unable to allocate memory for the \"copy\" object. "
		      "This is a serious problem!")),
	   QString(""), __FILE__, __LINE__);
    }

  if(saveCopies().isEmpty())
    {
      qapp->setOverrideCursor(Qt::WaitCursor);
      misc_functions::saveQuantity(qmain->getDB(), ioid, copies.size(),
				   itemType, errorstr);
      qapp->restoreOverrideCursor();

      if(!errorstr.isEmpty())
	qmain->addError(QString(tr("Database Error")),
			QString(tr("Unable to save the item's quantity.")),
			errorstr, __FILE__, __LINE__);
      else
	goto success_label;
    }

  qapp->setOverrideCursor(Qt::WaitCursor);
  copies.clear();

  if(!qmain->getDB().rollback())
    qmain->addError(QString(tr("Database Error")),
		    QString(tr("Rollback failure.")),
		    qmain->getDB().lastError().text(), __FILE__, __LINE__);

  qapp->restoreOverrideCursor();
  QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			tr("Unable to save the copy data."));
  return;

 success_label:

  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!qmain->getDB().commit())
    {
      copies.clear();
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Commit failure.")),
		      qmain->getDB().lastError().text(), __FILE__, __LINE__);
      qmain->getDB().rollback();
      qapp->restoreOverrideCursor();
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to commit the copy data."));
      return;
    }

  qapp->restoreOverrideCursor();
  spinbox->setValue(copies.size());

  /*
    qapp->setOverrideCursor(Qt::WaitCursor);
    availability = misc_functions::getAvailability(ioid, qmain->getDB(),
    itemType, errorstr);
    qapp->restoreOverrideCursor();

    if(!errorstr.isEmpty())
    qmain->addError(QString(tr("Database Error")),
    QString(tr("Retrieving availability.")),
    errorstr, __FILE__, __LINE__);

    misc_functions::updateColumn(qmain->getUI().table, bitem->getRow(),
    tr("Availability"), availability);
    str.setNum(copies.size());
    misc_functions::updateColumn(qmain->getUI().table, bitem->getRow(),
    tr("Quantity"), str);
  */

  bitem->setOldQ(copies.size());
  copies.clear();
}

/*
** -- saveCopies() --
*/

QString copy_editor::saveCopies(void)
{
  int i = 0;
  QString lastError = "";
  QSqlQuery query(qmain->getDB());
  copy_class *copy = 0;
  QProgressDialog progress(this);

#ifdef Q_WS_MAC
  progress.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
  query.prepare(QString("DELETE FROM %1_copy_info WHERE "
			"item_oid = ?").arg(itemType.toLower().remove(" ")));
  query.bindValue(0, ioid);
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
    {
      qapp->restoreOverrideCursor();
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to purge copy data.")),
		      query.lastError().text(), __FILE__, __LINE__);
      return query.lastError().text();
    }
  else
    {
      qapp->restoreOverrideCursor();
      progress.setModal(true);
      progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
      progress.setLabelText(tr("Saving the copy data..."));
      progress.setMaximum(copies.size());
      progress.setMinimum(0);
      progress.show();
      progress.update();

      for(i = 0; i < copies.size(); i++)
	{
	  copy = copies.at(i);

	  if(qmain->getDB().driverName() != "QSQLITE")
	    query.prepare(QString("INSERT INTO %1_copy_info "
				  "(item_oid, copy_number, "
				  "copyid) "
				  "VALUES (?, "
				  "?, ?)").arg
			  (itemType.toLower().remove(" ")));
	  else
	    query.prepare(QString("INSERT INTO %1_copy_info "
				  "(item_oid, copy_number, "
				  "copyid, myoid) "
				  "VALUES (?, "
				  "?, ?, ?)").arg
			  (itemType.toLower().remove(" ")));

	  query.bindValue(0, copy->itemoid);
	  query.bindValue(1, i + 1);
	  query.bindValue(2, copy->copyid);

	  if(qmain->getDB().driverName() == "QSQLITE")
	    {
	      qint64 value = 0;
	      QString errorstr("");

	      value = misc_functions::getSqliteUniqueId(qmain->getDB(),
							errorstr);

	      if(errorstr.isEmpty())
		query.bindValue(3, value);
	      else
		qmain->addError(QString(tr("Database Error")),
				QString(tr("Unable to generate a unique "
					   "integer.")),
				errorstr);
	    }

	  if(!query.exec())
	    {
	      lastError = query.lastError().text();
	      qmain->addError(QString(tr("Database Error")),
			      QString(tr("Unable to create copy data.")),
			      query.lastError().text(), __FILE__, __LINE__);
	    }

	  if(i + 1 <= progress.maximum())
	    progress.setValue(i + 1);

	  progress.update();
#ifndef Q_WS_MAC
	  qapp->processEvents();
#endif
	}

      progress.hide();
    }

  return lastError;
}

/*
** -- keyPressEvent() --
*/

void copy_editor::keyPressEvent(QKeyEvent *event)
{
  if(event && event->key() == Qt::Key_Escape)
    slotCloseCopyEditor();

  QDialog::keyPressEvent(event);
}

/*
** -- slotCloseCopyEditor() --
*/

void copy_editor::slotCloseCopyEditor(void)
{
  clearCopiesList();
  deleteLater();
}

/*
** -- clearCopiesList() --
*/

void copy_editor::clearCopiesList(void)
{
  while(!copies.isEmpty())
    delete copies.takeFirst();
}

/*
** -- closeEvent() --
*/

void copy_editor::closeEvent(QCloseEvent *e)
{
  slotCloseCopyEditor();
  QDialog::closeEvent(e);
}

/*
** -- setGlobalFonts() --
*/

void copy_editor::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(QWidget *widget, findChildren<QWidget *>())
    widget->setFont(font);
}
