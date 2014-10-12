/*
** -- Local Includes --
*/

#include "qtbook.h"
#include "copy_editor_book.h"

extern qtbook *qmain;
extern QApplication *qapp;

/*
** -- copy_editor_book() --
*/

copy_editor_book::copy_editor_book
(QWidget *parent, qtbook_item *bitemArg,
 const bool showForLendingArg,
 const int quantityArg, const QString &ioidArg,
 QSpinBox *spinboxArg,
 const QFont &font,
 const QString &uniqueIdArg): copy_editor(parent)
{
  if(parent == qmain->getMembersBrowser())
    setWindowModality(Qt::ApplicationModal);
  else
    setWindowModality(Qt::WindowModal);

  cb.setupUi(this);
#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  setAttribute(Qt::WA_MacMetalStyle, true);
#endif
#endif
  bitem = bitemArg;
  ioid = ioidArg;
  quantity = quantityArg;
  spinbox = spinboxArg;
  itemType = "Book";
  showForLending = showForLendingArg;
#if QT_VERSION >= 0x050000
  cb.table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
  cb.table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
#endif

  if(!uniqueIdArg.trimmed().isEmpty())
    setWindowTitle
      (tr("BiblioteQ: Copy Browser (%1)").arg(uniqueIdArg));
  else
    setWindowTitle(tr("BiblioteQ: Copy Browser"));

  setGlobalFonts(font);
}

/*
** -- ~copy_editor_book() --
*/

copy_editor_book::~copy_editor_book()
{
  clearCopiesList();
}

/*
** -- slotDeleteCopy() --
*/

void copy_editor_book::slotDeleteCopy(void)
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

  copyid = misc_functions::getColumnString
    (cb.table, row, m_columnHeaderIndexes.indexOf("Barcode"));
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

void copy_editor_book::populateCopiesEditor(void)
{
  int i = 0;
  int j = 0;
  int row = 0;
  bool terminate = false;
  QString str = "";
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

#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  progress1.setAttribute(Qt::WA_MacMetalStyle, true);
  progress2.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
#endif
  cb.table->clear();
  cb.table->setColumnCount(0);
  cb.table->setRowCount(0);
  list.append(tr("Title"));
  list.append(tr("Barcode"));
  list.append(tr("Availability"));
  list.append(tr("Originality"));
  list.append(tr("Condition"));
  list.append("ITEM_OID");
  list.append("Copy Number");
  m_columnHeaderIndexes.clear();
  m_columnHeaderIndexes.append("Title");
  m_columnHeaderIndexes.append("Barcode");
  m_columnHeaderIndexes.append("Availability");
  m_columnHeaderIndexes.append("Originality");
  m_columnHeaderIndexes.append("Condition");
  m_columnHeaderIndexes.append("ITEM_OID");
  m_columnHeaderIndexes.append("Copy Number");
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
	if(j == 3 || j == 4)
	  {
	    QComboBox *combobox = new(std::nothrow) QComboBox();

	    if(combobox)
	      {
		QStringList list;

		if(j == 3)
		  list << tr("Black & White Copy")
		       << tr("Color Copy")
		       << tr("Original");
		else
		  list << tr("As New")
		       << tr("Binding Copy")
		       << tr("Book Club")
		       << tr("Ex-Library")
		       << tr("Fair")
		       << tr("Fine")
		       << tr("Good")
		       << tr("Poor")
		       << tr("Very Good");

		combobox->addItems(list);

		if(j == 3)
		  combobox->setCurrentIndex(2);

		cb.table->setCellWidget(i, j, combobox);
	      }
	    else
	      qmain->addError(QString(tr("Memory Error")),
			      QString(tr("Unable to allocate memory for the "
					 "\"combobox\" object. "
					 "This is a serious problem!")),
			      QString(""), __FILE__, __LINE__);
	  }
	else if((item = new(std::nothrow) QTableWidgetItem()) != 0)
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
	    else if(j == 5)
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
#ifndef Q_OS_MAC
      qapp->processEvents();
#endif
    }

  progress1.hide();
  cb.table->setRowCount(i);
  query.prepare(QString("SELECT %1.title, "
			"%1_copy_info.copyid, "
			"(1 - COUNT(item_borrower_vw.copyid)), "
			"%1_copy_info.originality, "
			"%1_copy_info.condition, "
			"%1_copy_info.item_oid, "
			"%1_copy_info.copy_number "
			"FROM "
			"%1, "
			"%1_copy_info LEFT JOIN item_borrower_vw ON "
			"%1_copy_info.copyid = "
			"item_borrower_vw.copyid AND "
			"%1_copy_info.item_oid = "
			"item_borrower_vw.item_oid AND "
			"item_borrower_vw.type = ? "
			"WHERE %1_copy_info.item_oid = ? AND "
			"%1.myoid = ? "
			"GROUP BY %1.title, "
			"%1_copy_info.copyid, "
			"%1_copy_info.originality, "
			"%1_copy_info.condition, "
			"%1_copy_info.item_oid, "
			"%1_copy_info.copy_number "
			"ORDER BY %1_copy_info.copy_number").arg
		(itemType.toLower().remove(" ")));
  query.bindValue(0, itemType);
  query.bindValue(1, ioid);
  query.bindValue(2, ioid);
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
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
    {
      if(query.lastError().isValid())
	progress2.setMaximum(0);
      else
	progress2.setMaximum
	  (misc_functions::sqliteQuerySize(query.lastQuery(),
					   query.boundValues(),
					   qmain->getDB(),
					   __FILE__, __LINE__));
    }
  else
    progress2.setMaximum(query.size());

  progress2.show();
  progress2.update();
  i = -1;

  while(i++, !progress2.wasCanceled() && query.next())
    {
      if(query.isValid())
	{
	  row = query.value(6).toInt() - 1;

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
	    else if(cb.table->cellWidget(row, j) != 0)
	      {
		QComboBox *combobox = qobject_cast<QComboBox *>
		  (cb.table->cellWidget(row, j));

		if(combobox)
		  {
		    str = query.value(j).toString();

		    if(combobox->findText(str) > -1)
		      combobox->setCurrentIndex(combobox->findText(str));
		  }
	      }
	    else
	      terminate = true;
	}

      if(i + 1 <= progress2.maximum())
	progress2.setValue(i + 1);

      progress2.update();
#ifndef Q_OS_MAC
      qapp->processEvents();
#endif
      if(terminate)
	break; // Out of resources?
    }

  progress2.hide();
  cb.table->resizeColumnsToContents();
}

/*
** -- slotSaveCopies() --
*/

void copy_editor_book::slotSaveCopies(void)
{
  int i = 0;
  QComboBox *combobox1 = 0;
  QComboBox *combobox2 = 0;
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
      combobox1 = qobject_cast<QComboBox *> (cb.table->cellWidget(i, 3));
      combobox2 = qobject_cast<QComboBox *> (cb.table->cellWidget(i, 4));
      item1 = cb.table->item(i, 1);
      item2 = cb.table->item(i, 5);

      if(combobox1 == 0 || combobox2 == 0 || item1 == 0 || item2 == 0)
	continue;

      if((copy = new(std::nothrow) copy_class
	  (combobox2->currentText().trimmed(),
	   item1->text().trimmed(),
	   item2->text(),
	   combobox1->currentText().trimmed())) != 0)
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

  while(!copies.isEmpty())
    delete copies.takeFirst();

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
      while(!copies.isEmpty())
	delete copies.takeFirst();

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

  while(!copies.isEmpty())
    delete copies.takeFirst();
}

/*
** -- keyPressEvent() --
*/

void copy_editor_book::keyPressEvent(QKeyEvent *event)
{
  if(event && event->key() == Qt::Key_Escape)
    slotCloseCopyEditor();

  QDialog::keyPressEvent(event);
}

/*
** -- closeEvent() --
*/

void copy_editor_book::closeEvent(QCloseEvent *e)
{
  slotCloseCopyEditor();
  QDialog::closeEvent(e);
}

/*
** -- changeEvent() --
*/

void copy_editor_book::changeEvent(QEvent *event)
{
  if(event)
    switch(event->type())
      {
      case QEvent::LanguageChange:
	{
	  cb.retranslateUi(this);
	  break;
	}
      default:
	break;
      }

  QDialog::changeEvent(event);
}

/*
** -- saveCopies() --
*/

QString copy_editor_book::saveCopies(void)
{
  int i = 0;
  QString lastError = "";
  QSqlQuery query(qmain->getDB());
  copy_class *copy = 0;
  QProgressDialog progress(this);

#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  progress.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
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
      progress.setCancelButton(0);
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
				  "copyid, originality, condition) "
				  "VALUES (?, ?, ?, "
				  "?, ?)").arg
			  (itemType.toLower().remove(" ")));
	  else
	    query.prepare(QString("INSERT INTO %1_copy_info "
				  "(item_oid, copy_number, "
				  "copyid, originality, condition, myoid) "
				  "VALUES (?, ?, ?, "
				  "?, ?, ?)").arg
			  (itemType.toLower().remove(" ")));

	  query.bindValue(0, copy->itemoid);
	  query.bindValue(1, i + 1);
	  query.bindValue(2, copy->copyid);
	  query.bindValue(3, copy->originality);
	  query.bindValue(4, copy->condition);

	  if(qmain->getDB().driverName() == "QSQLITE")
	    {
	      qint64 value = 0;
	      QString errorstr("");

	      value = misc_functions::getSqliteUniqueId(qmain->getDB(),
							errorstr);

	      if(errorstr.isEmpty())
		query.bindValue(5, value);
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
#ifndef Q_OS_MAC
	  qapp->processEvents();
#endif
	}

      progress.hide();
    }

  return lastError;
}
