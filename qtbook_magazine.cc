/*
** Includes magazine-specific methods.
*/

/*
** Note: The ISSN is not a unique value.
*/

/*
** -- Local Includes --
*/

#include "ui_locresults.h"
#include "qtbook_magazine.h"

extern qtbook *qmain;
extern QApplication *qapp;

/*
** -- qtbook_magazine() --
*/

qtbook_magazine::qtbook_magazine(QMainWindow *parentArg,
				 const QStringList &languages,
				 const QStringList &monetary_units,
				 const QStringList &locations,
				 const QString &oidArg,
				 const int rowArg):
  QMainWindow()
{
  QMenu *menu = NULL;
  QRegExp rx("[0-9][0-9][0-9][0-9]-[0-9][0-9][0-9][0-9X]");
  QValidator *validator1 = NULL;
  QGraphicsScene *scene1 = NULL;
  QGraphicsScene *scene2 = NULL;

  if((menu = new QMenu()) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((validator1 = new QRegExpValidator(rx, this)) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene1 = new QGraphicsScene()) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene2 = new QGraphicsScene()) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  thread = NULL;
  oid = oidArg;
  row = rowArg;
  subType = "Magazine";
  parentWid = parentArg;
  oldq = misc_functions::getColumnString
    (qmain->getUI().table, row, "Quantity").toInt();
  ma.setupUi(this);
  updateFont(qapp->font(), (QWidget *) this);
  connect(ma.okButton, SIGNAL(clicked(void)), this, SLOT(slotGo(void)));
  connect(ma.showUserButton, SIGNAL(clicked(void)), this,
	  SLOT(slotShowUsers(void)));
  connect(ma.queryButton, SIGNAL(clicked(void)), this,
	  SLOT(slotQuery(void)));
  connect(ma.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCancel(void)));
  connect(ma.copiesButton, SIGNAL(clicked(void)), this,
	  SLOT(slotPopulateCopiesEditor(void)));
  connect(ma.resetButton, SIGNAL(clicked(void)), this,
	  SLOT(slotReset(void)));
  connect(ma.printButton, SIGNAL(clicked(void)), this, SLOT(slotPrint(void)));
  connect(menu->addAction("Reset &Front Cover Image"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Back Cover Image"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &ISSN"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Volume"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Issue"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &LC Control Number"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Call Number"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Dewey Number"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Title"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Publication Date"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Publisher"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Categories"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Price"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Language"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Monetary Units"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Copies"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Location"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Abstract"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &OFFSYSTEM URL"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(ma.frontButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(ma.backButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  ma.id->setCursorPosition(0);
  ma.id->setValidator(validator1);
  ma.resetButton->setMenu(menu);
  ma.language->addItems(languages);
  ma.monetary_units->addItems(monetary_units);
  ma.location->addItems(locations);
  ma.front_image->setScene(scene1);
  ma.back_image->setScene(scene2);

  if(ma.language->count() == 0)
    ma.language->addItem("UNKNOWN");

  if(ma.monetary_units->count() == 0)
    ma.monetary_units->addItem("UNKNOWN");

  if(ma.location->count() == 0)
    ma.location->addItem("UNKNOWN");

  /*
  ** Save some palettes.
  */

  cb_orig_pal = ma.language->palette();
  dt_orig_pal = ma.publication_date->palette();
  te_orig_pal = ma.description->viewport()->palette();

  /*
  ** Prepare the form.
  */

  resize(baseSize());
  misc_functions::center(this, parentWid);
  misc_functions::hideAdminFields(this, qmain->getRoles());
}

/*
** -- ~qtbook_magazine() --
*/

qtbook_magazine::~qtbook_magazine()
{
  if(thread != NULL && thread->isRunning())
    qapp->restoreOverrideCursor();
}

/*
** -- slotGo() --
*/

void qtbook_magazine::slotGo(void)
{
  int i = 0;
  int newq = 0;
  int maxcopynumber = 0;
  QString str = "";
  QString errorstr = "";
  QString searchstr = "";
  QSqlQuery query(qmain->getDB());
  QTableWidgetItem *column = NULL;

  if(windowTitle().contains("Create") ||
     windowTitle().contains("Modify"))
    {
      if(windowTitle().contains("Modify") && row > -1)
	{
	  newq = ma.quantity->value();
	  qapp->setOverrideCursor(Qt::WaitCursor);
	  maxcopynumber = misc_functions::getMaxCopyNumber
	    (qmain->getDB(), oid, subType, errorstr);

	  if(maxcopynumber < 0)
	    {
	      qapp->restoreOverrideCursor();
	      qmain->addError
		(QString("Database Error"),
		 QString("Unable to determine the maximum copy number of "
			 "the item."),
		 errorstr, __FILE__, __LINE__);
	      QMessageBox::critical
		(this, "BiblioteQ: Database Error",
		 "Unable to determine the maximum copy number of "
		 "the item.");
	      return;
	    }

	  qapp->restoreOverrideCursor();

	  if(newq < maxcopynumber)
	    {
	      QMessageBox::critical
		(this, "BiblioteQ: User Error",
		 "It appears that you are attempting to decrease the "
		 "number of copies while there are copies "
		 "that have been reserved.");
	      ma.quantity->setValue(oldq);
	      return;
	    }
	  else if(newq > oldq)
	    if(QMessageBox::question
	       (this, "BiblioteQ: Question",
		"Would you like to modify copy information?",
		QMessageBox::Yes | QMessageBox::No,
		QMessageBox::No) == QMessageBox::Yes)
	      slotPopulateCopiesEditor();
	}

      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!qmain->getDB().transaction())
	{
	  qapp->restoreOverrideCursor();
	  qmain->addError
	    (QString("Database Error"),
	     QString("Unable to create a database transaction."),
	     qmain->getDB().lastError().text(), __FILE__, __LINE__);
	  QMessageBox::critical
	    (this, "BiblioteQ: Database Error",
	     "Unable to create a database transaction.");
	  return;
	}

      qapp->restoreOverrideCursor();
      str = ma.id->text().trimmed();
      ma.id->setText(str);

      if(ma.id->text().length() != 9)
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the ISSN field.");
	  ma.id->setFocus();
	  goto db_rollback;
	}

      str = ma.title->text().trimmed();
      ma.title->setText(str);

      if(ma.title->text().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Title field.");
	  ma.title->setFocus();
	  goto db_rollback;
	}

      str = ma.publisher->toPlainText().trimmed();
      ma.publisher->setPlainText(str);

      if(ma.publisher->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Publisher field.");
	  ma.publisher->setFocus();
	  goto db_rollback;
	}

      str = ma.category->toPlainText().trimmed();
      ma.category->setPlainText(str);

      if(ma.category->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Categories field.");
	  ma.category->setFocus();
	  goto db_rollback;
	}

      str = ma.description->toPlainText().trimmed();
      ma.description->setPlainText(str);

      if(ma.description->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Abstract field.");
	  ma.description->setFocus();
	  goto db_rollback;
	}

      str = ma.lcnum->text().trimmed();
      ma.lcnum->setText(str);
      str = ma.callnum->text().trimmed();
      ma.callnum->setText(str);
      str = ma.deweynum->text().trimmed();
      ma.deweynum->setText(str);
      str = ma.url->toPlainText().trimmed();
      ma.url->setPlainText(str);

      if(windowTitle().contains("Modify"))
	query.prepare(QString("UPDATE %1 SET "
			      "id = ?, "
			      "title = ?, "
			      "pdate = ?, "
			      "publisher = ?, "
			      "category = ?, price = ?, "
			      "description = ?, "
			      "language = ?, "
			      "monetary_units = ?, "
			      "quantity = ?, "
			      "location = ?, "
			      "issuevolume = ?, "
			      "issueno = ?, "
			      "lccontrolnumber = ?, "
			      "callnumber = ?, "
			      "deweynumber = ?, "
			      "front_cover = ?, "
			      "back_cover = ?, "
			      "offsystem_url = ? "
			      "WHERE "
			      "myoid = ?").arg(subType));
      else if(qmain->getDB().driverName() != "QSQLITE")
	query.prepare(QString("INSERT INTO %1 "
			      "(id, "
			      "title, "
			      "pdate, publisher, "
			      "category, price, description, language, "
			      "monetary_units, quantity, "
			      "location, issuevolume, issueno, "
			      "lccontrolnumber, callnumber, deweynumber, "
			      "front_cover, back_cover, "
			      "offsystem_url, type) "
			      "VALUES (?, ?, "
			      "?, ?, "
			      "?, ?, ?, "
			      "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)").arg
		      (subType));
      else
	query.prepare(QString("INSERT INTO %1 "
			      "(id, "
			      "title, "
			      "pdate, publisher, "
			      "category, price, description, language, "
			      "monetary_units, quantity, "
			      "location, issuevolume, issueno, "
			      "lccontrolnumber, callnumber, deweynumber, "
			      "front_cover, back_cover, "
			      "offsystem_url, type, myoid) "
			      "VALUES (?, ?, "
			      "?, ?, ?, "
			      "?, ?, "
			      "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)").arg
		      (subType));

      query.bindValue(0, ma.id->text());
      query.bindValue(1, ma.title->text());
      query.bindValue(2, ma.publication_date->date().toString("MM/dd/yyyy"));
      query.bindValue(3, ma.publisher->toPlainText());
      query.bindValue(4, ma.category->toPlainText().trimmed());
      query.bindValue(5, ma.price->text());
      query.bindValue(6, ma.description->toPlainText());
      query.bindValue(7, ma.language->currentText().trimmed());
      query.bindValue(8, ma.monetary_units->currentText().trimmed());
      query.bindValue(9, ma.quantity->text());
      query.bindValue(10, ma.location->currentText().trimmed());
      query.bindValue(11, ma.volume->text());
      query.bindValue(12, ma.issue->text());

      if(!ma.lcnum->text().isEmpty())
	query.bindValue(13, ma.lcnum->text());
      else
	query.bindValue(13, QVariant(QVariant::String));

      if(!ma.callnum->text().isEmpty())
	query.bindValue(14, ma.callnum->text());
      else
	query.bindValue(14, QVariant(QVariant::String));

      if(!ma.deweynum->text().isEmpty())
	query.bindValue(15, ma.deweynum->text());
      else
	query.bindValue(15, QVariant(QVariant::String));

      if(!ma.front_image->image.isNull())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);
	  buffer.open(QIODevice::WriteOnly);
	  ma.front_image->image.save
	    (&buffer, ma.front_image->imageFormat.toAscii(), 100);
	  query.bindValue(16, bytes);
	}
      else
	{
	  ma.front_image->imageFormat = "";
	  query.bindValue(16, QVariant());
	}

      if(!ma.back_image->image.isNull())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);
	  buffer.open(QIODevice::WriteOnly);
	  ma.back_image->image.save
	    (&buffer, ma.back_image->imageFormat.toAscii(), 100);
	  query.bindValue(17, bytes);
	}
      else
	{
	  ma.back_image->imageFormat = "";
	  query.bindValue(17, QVariant());
	}

      if(!ma.url->toPlainText().isEmpty())
	query.bindValue(18, ma.url->toPlainText());
      else
	query.bindValue(18, QVariant(QVariant::String));

      if(windowTitle().contains("Modify"))
	query.bindValue(19, oid);
      else
	query.bindValue(19, subType);

      if(windowTitle().contains("Create"))
	if(qmain->getDB().driverName() == "QSQLITE")
	  query.bindValue(20,
			  ma.id->text().remove("-") +
			  ma.volume->text() + ma.issue->text());

      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!query.exec())
	{
	  qapp->restoreOverrideCursor();
	  qmain->addError(QString("Database Error"),
			  QString("Unable to create or update the entry."),
			  query.lastError().text(), __FILE__, __LINE__);
	  QMessageBox::critical
	    (this, "BiblioteQ: Database Error",
	     "Unable to create or update the entry. "
	     "Please verify that "
	     "the entry does not already exist.");
	  goto db_rollback;
	}
      else
	{
	  /*
	  ** Remove copies if the quantity has been decreased.
	  */

	  if(windowTitle().contains("Modify"))
	    {
	      query.prepare(QString("DELETE FROM %1_copy_info WHERE "
				    "copy_number > ? AND "
				    "item_oid = "
				    "?").arg(subType));
	      query.bindValue(0, ma.quantity->text());
	      query.bindValue(1, oid);

	      if(!query.exec())
		{
		  qapp->restoreOverrideCursor();
		  qmain->addError(QString("Database Error"),
				  QString("Unable to purge unnecessary copy "
					  "data."),
				  query.lastError().text(), __FILE__,
				  __LINE__);
		  QMessageBox::critical(this,
					"BiblioteQ: Database Error",
					"Unable to purge unnecessary "
					"copy data.");
		  goto db_rollback;
		}

	      if(!qmain->getDB().commit())
		{
		  qapp->restoreOverrideCursor();
		  qmain->addError
		    (QString("Database Error"),
		     QString("Unable to commit the current database "
			     "transaction."),
		     qmain->getDB().lastError().text(), __FILE__, __LINE__);
		  QMessageBox::critical(this, "BiblioteQ: Database Error",
					"Unable to commit the current "
					"database transaction.");
		  goto db_rollback;
		}
	    }
	  else
	    {
	      /*
	      ** Create initial copies.
	      */

	      misc_functions::createInitialCopies
		(ma.id->text() + "," + ma.volume->text() + "," +
		 ma.issue->text(), ma.quantity->value(),
		 qmain->getDB(), subType, errorstr);

	      if(!errorstr.isEmpty())
		{
		  qapp->restoreOverrideCursor();
		  qmain->addError
		    (QString("Database Error"),
		     QString("Unable to create initial copies."),
		     errorstr, __FILE__, __LINE__);
		  QMessageBox::critical(this,
					"BiblioteQ: Database Error",
					"Unable to create initial copies.");
		  goto db_rollback;
		}

	      if(!qmain->getDB().commit())
		{
		  qapp->restoreOverrideCursor();
		  qmain->addError
		    (QString("Database Error"),
		     QString("Unable to commit the current database "
			     "transaction."),
		     qmain->getDB().lastError().text(), __FILE__,
		     __LINE__);
		  QMessageBox::critical(this, "BiblioteQ: Database Error",
					"Unable to commit the current "
					"database transaction.");
		  goto db_rollback;
		}
	    }

	  ma.id->setPalette(te_orig_pal);
	  ma.category->viewport()->setPalette(te_orig_pal);
	  ma.lcnum->setPalette(ma.url->viewport()->palette());
	  ma.callnum->setPalette(ma.url->viewport()->palette());
	  ma.deweynum->setPalette(ma.url->viewport()->palette());
	  ma.title->setPalette(te_orig_pal);
	  ma.publication_date->setPalette(dt_orig_pal);
	  ma.description->viewport()->setPalette(te_orig_pal);
	  ma.publisher->viewport()->setPalette(te_orig_pal);
	  oldq = ma.quantity->value();

	  if(ma.front_image->image.isNull())
	    ma.front_image->imageFormat = "";

	  if(ma.back_image->image.isNull())
	    ma.back_image->imageFormat = "";

	  if(subType == "Journal")
	    ma.publisher->setText
	      (QString("<a href=\"journal_search?publisher?%1\">" +
		       ma.publisher->toPlainText() + "</a>").arg
	       (ma.publisher->toPlainText()));
	  else
	    ma.publisher->setText
	      (QString("<a href=\"magazine_search?publisher?%1\">" +
		       ma.publisher->toPlainText() + "</a>").arg
	       (ma.publisher->toPlainText()));

	  if(!ma.url->toPlainText().isEmpty())
	    ma.url->setText(QString("<a href=\"%1\">%1</a>").arg
			    (ma.url->toPlainText()));

	  qapp->restoreOverrideCursor();

	  if(windowTitle().contains("Modify"))
	    {
	      str = QString("BiblioteQ: Modify %1 Entry (%2)").arg
		(subType).arg(ma.id->text());
	      setWindowTitle(str);

	      if((qmain->getUI().typefilter->currentText() == "All" ||
		  qmain->getUI().typefilter->currentText() ==
		  (subType + "s")) &&
		 oid == misc_functions::getColumnString(qmain->getUI().table,
							row, "OID"))
		{
		  for(i = 0; i < qmain->getUI().table->columnCount(); i++)
		    {
		      column = qmain->getUI().table->horizontalHeaderItem(i);

		      if(column == NULL)
			continue;

		      if(column->text() == "ISSN" ||
			 column->text() == "ID Number")
			qmain->getUI().table->item(row, i)->setText
			  (ma.id->text());
		      else if(column->text() == "Title")
			qmain->getUI().table->item(row, i)->setText
			  (ma.title->text());
		      else if(column->text() == "Publication Date")
			qmain->getUI().table->item(row, i)->setText
			  (ma.publication_date->date().toString("MM/dd/yyyy"));
		      else if(column->text() == "Publisher")
			qmain->getUI().table->item(row, i)->setText
			  (ma.publisher->toPlainText());
		      else if(column->text() == "Categories")
			qmain->getUI().table->item(row, i)->setText
			  (ma.category->toPlainText().trimmed());
		      else if(column->text() == "Price")
			qmain->getUI().table->item(row, i)->setText
			  (ma.price->text());
		      else if(column->text() == "Language")
			qmain->getUI().table->item(row, i)->setText
			  (ma.language->currentText().trimmed());
		      else if(column->text() == "Monetary Units")
			qmain->getUI().table->item(row, i)->setText
			  (ma.monetary_units->currentText().trimmed());
		      else if(column->text() == "Quantity")
			qmain->getUI().table->item(row, i)->setText
			  (ma.quantity->text());
		      else if(column->text() == "Location")
			qmain->getUI().table->item(row, i)->setText
			  (ma.location->currentText().trimmed());
		      else if(column->text() == "Volume")
			qmain->getUI().table->item(row, i)->setText
			  (ma.volume->text());
		      else if(column->text() == "Issue")
			qmain->getUI().table->item(row, i)->setText
			  (ma.issue->text());
		      else if(column->text() == "LC Control Number")
			qmain->getUI().table->item(row, i)->setText
			  (ma.lcnum->text());
		      else if(column->text() == "Call Number")
			qmain->getUI().table->item(row, i)->setText
			  (ma.callnum->text());
		      else if(column->text() == "Dewey Number")
			qmain->getUI().table->item(row, i)->setText
			  (ma.deweynum->text());
		      else if(column->text() == "Availability")
			{
			  qmain->getUI().table->item(row, i)->setText
			    (misc_functions::getAvailability
			     (oid, qmain->getDB(), subType,
			      errorstr));

			  if(!errorstr.isEmpty())
			    qmain->addError
			      (QString("Database Error"),
			       QString("Retrieving availability."),
			       errorstr, __FILE__, __LINE__);
			}
		    }

		  foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
		    textfield->setCursorPosition(0);

		  if(qmain->getUI().actionAutoResizeColumns->isChecked())
		    qmain->slotResizeColumns();

		  qmain->slotDisplaySummary();
		}
	    }
	  else
	    {
	      qapp->setOverrideCursor(Qt::WaitCursor);
	      oid = misc_functions::getOID(ma.id->text() + "," +
					   ma.volume->text() + "," +
					   ma.issue->text(),
					   subType,
					   qmain->getDB(),
					   errorstr);
	      qapp->restoreOverrideCursor();

	      if(!errorstr.isEmpty())
		{
		  oid = "insert";
		  qmain->addError(QString("Database Error"),
				  QString("Unable to retrieve the "
					  "%1's OID.").arg
				  (subType.toLower()),
				  errorstr, __FILE__, __LINE__);
		  QMessageBox::critical
		    (this, "BiblioteQ: Database Error",
		     QString("Unable to retrieve the "
			     "%1's OID.").arg(subType.toLower()));
		}
	      else if(subType == "Journal")
		qmain->replaceJournal
		  ("insert", (qobject_cast<qtbook_journal *>)(this));
	      else if(subType == "Magazine")
		qmain->replaceMagazine("insert", this);

	      updateWindow(qtbook::EDITABLE);

	      if(qmain->getUI().actionAutoPopulateOnCreation->isChecked())
		(void) qmain->populateTable
		  (qtbook::POPULATE_ALL, QString("%1s").arg(subType),
		   QString(""));

	      raise();
	    }

	  storeData(this);
	}

      return;

    db_rollback:

      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!qmain->getDB().rollback())
	qmain->addError(QString("Database Error"),
			QString("Rollback failure."),
			qmain->getDB().lastError().text(), __FILE__,
			__LINE__);

      qapp->restoreOverrideCursor();
    }
  else
    {
      searchstr = QString("SELECT %1.title, "
			  "%1.publisher, %1.pdate, "
			  "%1.issuevolume, "
			  "%1.issueno, "
			  "%1.category, %1.language, "
			  "%1.id, "
			  "%1.price, %1.monetary_units, "
			  "%1.quantity, "
			  "%1.location, "
			  "%1.lccontrolnumber, "
			  "%1.callnumber, "
			  "%1.deweynumber, "
			  "%1.quantity - COUNT(item_borrower_vw."
			  "item_oid) "
			  "AS availability, "
			  "%1.type, "
			  "%1.myoid "
			  "FROM "
			  "%1 LEFT JOIN item_borrower_vw ON "
			  "%1.myoid = item_borrower_vw.item_oid "
			  "AND item_borrower_vw.type = '%1' "
			  "WHERE %1.type = '%1' AND ").arg(subType);
      searchstr.append("id LIKE '%" + ma.id->text() + "%' AND ");
      searchstr.append("LOWER(COALESCE(lccontrolnumber, '')) LIKE '%" +
		       myqstring::escape(ma.lcnum->text().toLower()) +
		       "%' AND ");
      searchstr.append("LOWER(COALESCE(callnumber, '')) LIKE '%" +
		       myqstring::escape(ma.callnum->text().toLower()) +
		       "%' AND ");
      searchstr.append("LOWER(COALESCE(deweynumber, '')) LIKE '%" +
		       myqstring::escape(ma.deweynum->text().toLower()) +
		       "%' AND ");

      if(ma.volume->value() != -1)
	searchstr.append("issuevolume = " + ma.volume->text() +
			 " AND ");

      if(ma.issue->value() != -1)
	searchstr.append("issueno = " + ma.issue->text() +
			 " AND ");

      searchstr.append("LOWER(title) LIKE '%" +
		       myqstring::escape(ma.title->text().toLower()) +
		       "%' AND ");

      if(ma.publication_date->date().toString
	 ("MM/dd/yyyy") != "01/01/7999")
	searchstr.append("pdate = '" +
			 ma.publication_date->date().toString
			 ("MM/dd/yyyy") +
			 "' AND ");

      searchstr.append("LOWER(publisher) LIKE '%" +
		       myqstring::escape
		       (ma.publisher->toPlainText().toLower()) +
		       "%' AND ");
      searchstr.append("LOWER(category) LIKE '%" +
		       myqstring::escape(ma.category->toPlainText().
					 toLower()) +
		       "%' AND ");

      if(ma.price->value() > 0)
	{
	  searchstr.append("price = ");
	  searchstr.append(ma.price->text());
	  searchstr.append(" AND ");
	}

      if(ma.language->currentText() != "Any")
	searchstr.append("language = '" +
			 myqstring::escape(ma.language->currentText()) +
			 "' AND ");

      if(ma.monetary_units->currentText() != "Any")
	searchstr.append("monetary_units = '" +
			 myqstring::escape
			 (ma.monetary_units->currentText()) +
			 "' AND ");

      searchstr.append("LOWER(description) LIKE '%" +
		       myqstring::escape
		       (ma.description->toPlainText().toLower()) + "%' ");

      if(ma.quantity->value() != 0)
	searchstr.append("AND quantity = " + ma.quantity->text() + " ");

      if(ma.location->currentText() != "Any")
	searchstr.append("AND location = '" +
			 myqstring::escape
			 (ma.location->currentText()) + "' ");

      if(!ma.url->toPlainText().isEmpty())
	searchstr.append(" AND LOWER(COALESCE(offsystem_url, '')) LIKE '%" +
			 myqstring::escape
			 (ma.url->toPlainText()) + "%' ");

      if(subType == "Journal")
	{
	  close();
	  qmain->removeJournal(qobject_cast<qtbook_journal *>(this));
	}
      else
	slotCancel();

      /*
      ** Search the database.
      */

      (void) qmain->populateTable
	(qtbook::POPULATE_SEARCH, QString("%1s").arg(subType), searchstr);
    }
}

/*
** -- search() --
*/

void qtbook_magazine::search(const QString &field, const QString &value)
{
  ma.coverImages->setVisible(false);
  ma.id->clear();
  ma.lcnum->clear();
  ma.callnum->clear();
  ma.deweynum->clear();
  ma.title->clear();
  ma.publisher->clear();
  ma.description->clear();
  ma.category->clear();
  ma.copiesButton->setVisible(false);
  ma.showUserButton->setVisible(false);
  ma.queryButton->setVisible(false);
  ma.okButton->setText("&Search");
  ma.publication_date->setDate(QDate::fromString("01/01/7999",
						 "MM/dd/yyyy"));
  ma.id->setCursorPosition(0);
  ma.price->setMinimum(0.00);
  ma.price->setValue(0.00);
  ma.quantity->setMinimum(0);
  ma.quantity->setValue(0);
  ma.volume->setMinimum(-1);
  ma.volume->setValue(-1);
  ma.issue->setMinimum(-1);
  ma.issue->setValue(-1);

  /*
  ** Add "any".
  */

  if(ma.language->findText("Any") == -1)
    ma.language->insertItem(0, "Any");

  if(ma.monetary_units->findText("Any") == -1)
    ma.monetary_units->insertItem(0, "Any");

  if(ma.location->findText("Any") == -1)
    ma.location->insertItem(0, "Any");

  ma.location->setCurrentIndex(0);
  ma.language->setCurrentIndex(0);
  ma.monetary_units->setCurrentIndex(0);
  ma.url->clear();

  if(field.isEmpty() && value.isEmpty())
    {
      foreach(QAction *action,
	      ma.resetButton->menu()->findChildren<QAction *>())
	if(action->text().contains("Cover Image"))
	  action->setVisible(false);

      setWindowTitle(QString("BiblioteQ: Database %1 Search").arg(subType));
      ma.id->setFocus();
      misc_functions::center(this, parentWid);
      show();
    }
  else
    {
      if(field == "publisher")
	ma.publisher->setPlainText(value);
      else if(field == "category")
	ma.category->setPlainText(value);

      slotGo();
    }
}

/*
** -- updateWindow() --
*/

void qtbook_magazine::updateWindow(const int state)
{
  QString str = "";

  if(state == qtbook::EDITABLE)
    {
      ma.showUserButton->setEnabled(true);
      ma.copiesButton->setEnabled(true);
      ma.queryButton->setVisible(true);
      ma.okButton->setVisible(true);
      ma.resetButton->setVisible(true);
      ma.frontButton->setVisible(true);
      ma.backButton->setVisible(true);
      str = QString("BiblioteQ: Modify %1 Entry "
		    "(%2)").arg(subType).arg(ma.id->text());
    }
  else
    {
      ma.showUserButton->setEnabled(true);
      ma.copiesButton->setEnabled(false);
      ma.queryButton->setVisible(false);
      ma.okButton->setVisible(false);
      ma.resetButton->setVisible(false);
      ma.frontButton->setVisible(false);
      ma.backButton->setVisible(false);
      str = QString("BiblioteQ: View %1 Details "
		    "(%2)").arg(subType).arg(ma.id->text());
    }

  ma.coverImages->setVisible(true);
  setWindowTitle(str);
}

/*
** -- modify() --
*/

void qtbook_magazine::modify(const int state)
{
  int i = 0;
  QString str = "";
  QString fieldname = "";
  QString searchstr = "";
  QVariant var;
  QSqlQuery query(qmain->getDB());

  if(state == qtbook::EDITABLE)
    {
      setWindowTitle(QString("BiblioteQ: Modify %1 Entry").arg(subType));
      ma.showUserButton->setEnabled(true);
      ma.copiesButton->setEnabled(true);
      ma.queryButton->setVisible(true);
      ma.okButton->setVisible(true);
      ma.resetButton->setVisible(true);
      ma.frontButton->setVisible(true);
      ma.backButton->setVisible(true);
      misc_functions::highlightWidget
	(ma.id, QColor(255, 248, 220));
      misc_functions::highlightWidget
	(ma.title, QColor(255, 248, 220));
      misc_functions::highlightWidget
	(ma.publisher->viewport(), QColor(255, 248, 220));
      misc_functions::highlightWidget
	(ma.description->viewport(), QColor(255, 248, 220));
      misc_functions::highlightWidget
	(ma.category->viewport(), QColor(255, 248, 220));
      te_orig_pal = ma.description->viewport()->palette();
    }
  else
    {
      setWindowTitle(QString("BiblioteQ: View %1 Details").arg(subType));
      ma.showUserButton->setEnabled(true);
      ma.copiesButton->setEnabled(false);
      ma.queryButton->setVisible(false);
      ma.okButton->setVisible(false);
      ma.resetButton->setVisible(false);
      ma.frontButton->setVisible(false);
      ma.backButton->setVisible(false);

      foreach(QAction *action,
	      ma.resetButton->menu()->findChildren<QAction *>())
	if(action->text().contains("Cover Image"))
	  action->setVisible(false);
    }

  ma.quantity->setMinimum(1);
  ma.price->setMinimum(0.01);
  ma.okButton->setText("&Save");
  ma.volume->setMinimum(0);
  ma.issue->setMinimum(0);
  str = oid;
  searchstr = QString("SELECT title, "
		      "publisher, pdate, issuevolume, "
		      "category, language, id, "
		      "price, monetary_units, quantity, "
		      "issueno, "
		      "location, lccontrolnumber, callnumber, "
		      "deweynumber, description, "
		      "front_cover, "
		      "back_cover, "
		      "offsystem_url "
		      "FROM "
		      "%1 "
		      "WHERE myoid = ").arg(subType);
  searchstr.append(str);
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(searchstr) || !query.next())
    {
      qapp->restoreOverrideCursor();
      qmain->addError
	(QString("Database Error"),
	 QString("Unable to retrieve the selected %1's data.").arg
	 (subType.toLower()),
	 query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical
	(this, "BiblioteQ: Database Error",
	 QString("Unable to retrieve the selected %1's "
		 "data.").arg(subType.toLower()));
      return;
    }
  else
    {
      qapp->restoreOverrideCursor();
      showNormal();

      for(i = 0; i < query.record().count(); i++)
	{
	  var = query.record().field(i).value();
	  fieldname = query.record().fieldName(i);

	  if(fieldname == "title")
	    ma.title->setText(var.toString());
	  else if(fieldname == "publisher")
	    {
	      if(subType == "Journal")
		ma.publisher->setText
		  (QString("<a href=\"journal_search?publisher?%1\">" +
			   var.toString() + "</a>").arg(var.toString()));
	      else
		ma.publisher->setText
		  (QString("<a href=\"magazine_search?publisher?%1\">" +
			   var.toString() + "</a>").arg(var.toString()));
	    }
	  else if(fieldname == "pdate")
	    ma.publication_date->setDate
	      (QDate::fromString(var.toString(), "MM/dd/yyyy"));
	  else if(fieldname == "price")
	    ma.price->setValue(var.toDouble());
	  else if(fieldname == "category")
	    {
	      if(subType == "Journal")
		ma.category->setMultipleLinks("journal_search", "category",
					      var.toString());
	      else
		ma.category->setMultipleLinks("magazine_search", "category",
					      var.toString());
	    }
	  else if(fieldname == "language")
	    {
	      if(ma.language->findText(var.toString()) > -1)
		ma.language->setCurrentIndex
		  (ma.language->findText(var.toString()));
	      else
		ma.language->setCurrentIndex
		  (ma.language->findText("UNKNOWN"));
	    }
	  else if(fieldname == "quantity")
	    ma.quantity->setValue(var.toInt());
	  else if(fieldname == "monetary_units")
	    {
	      if(ma.monetary_units->findText(var.toString()) > -1)
		ma.monetary_units->setCurrentIndex
		  (ma.monetary_units->findText(var.toString()));
	      else
		ma.monetary_units->setCurrentIndex
		  (ma.monetary_units->findText("UNKNOWN"));
	    }
	  else if(fieldname == "issuevolume")
	    ma.volume->setValue(var.toInt());
	  else if(fieldname == "issueno")
	    ma.issue->setValue(var.toInt());
	  else if(fieldname == "location")
	    {
	      if(ma.location->findText(var.toString()) > -1)
		ma.location->setCurrentIndex
		  (ma.location->findText(var.toString()));
	      else
		ma.location->setCurrentIndex
		  (ma.location->findText("UNKNOWN"));
	    }
	  else if(fieldname == "id")
	    {
	      if(state == qtbook::EDITABLE)
		str = QString("BiblioteQ: Modify %1 Entry (%2)").arg
		  (subType).arg(var.toString());
	      else
		str = QString("BiblioteQ: View %1 Details (%2)").arg
		  (subType).arg(var.toString());

	      setWindowTitle(str);
	      ma.id->setText(var.toString());
	    }
	  else if(fieldname == "description")
	    ma.description->setPlainText(var.toString());
	  else if(fieldname == "lccontrolnumber")
	    ma.lcnum->setText(var.toString());
	  else if(fieldname == "callnumber")
	    ma.callnum->setText(var.toString());
	  else if(fieldname == "deweynumber")
	    ma.deweynum->setText(var.toString());
	  else if(fieldname == "front_cover")
	    {
	      if(!query.record().field(i).isNull())
		ma.front_image->loadFromData(var.toByteArray());
	    }
	  else if(fieldname == "back_cover")
	    {
	      if(!query.record().field(i).isNull())
		ma.back_image->loadFromData(var.toByteArray());
	    }
	  else if(fieldname == "offsystem_url")
	    {
	      if(!query.record().field(i).isNull())
		ma.url->setText(QString("<a href=\"%1\">%1</a>").arg
				(var.toString()));
	    }
	}

      foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
	textfield->setCursorPosition(0);

      storeData(this);
    }

  ma.id->setFocus();
  raise();
}

/*
** -- insert() --
*/

void qtbook_magazine::insert(void)
{
  slotReset();
  ma.id->clear();
  ma.lcnum->clear();
  ma.callnum->clear();
  ma.deweynum->clear();
  ma.title->clear();
  ma.publisher->clear();
  ma.description->clear();
  ma.category->clear();
  ma.copiesButton->setEnabled(false);
  ma.queryButton->setVisible(true);
  ma.okButton->setText("&Save");
  ma.publication_date->setDate(QDate::fromString("01/01/2000",
						 "MM/dd/yyyy"));
  ma.id->setCursorPosition(0);
  ma.price->setMinimum(0.01);
  ma.price->setValue(0.01);
  ma.quantity->setMinimum(1);
  ma.quantity->setValue(1);
  ma.volume->setMinimum(0);
  ma.volume->setValue(0);
  ma.issue->setMinimum(0);
  ma.issue->setValue(0);
  ma.showUserButton->setEnabled(false);
  ma.location->setCurrentIndex(0);
  ma.language->setCurrentIndex(0);
  ma.monetary_units->setCurrentIndex(0);
  ma.url->clear();
  misc_functions::highlightWidget
    (ma.id, QColor(255, 248, 220));
  misc_functions::highlightWidget
    (ma.title, QColor(255, 248, 220));
  misc_functions::highlightWidget
    (ma.publisher->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (ma.description->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (ma.category->viewport(), QColor(255, 248, 220));
  te_orig_pal = ma.description->viewport()->palette();
  setWindowTitle(QString("BiblioteQ: Create %1 Entry").arg(subType));
  ma.id->setFocus();
  misc_functions::center(this, parentWid);
  show();
}

/*
** -- slotReset() --
*/

void qtbook_magazine::slotReset(void)
{
  QAction *action = qobject_cast<QAction *>(sender());
  QString name = "";

  if(action != NULL)
    {
      name = action->text();

      if(name.contains("Front Cover Image"))
	ma.front_image->clear();
      else if(name.contains("Back Cover Image"))
	ma.back_image->clear();
      else if(name.contains("ISSN"))
	{
	  ma.id->clear();
	  ma.id->setCursorPosition(0);
	  ma.id->setPalette(te_orig_pal);
	  ma.id->setFocus();
	}
      else if(name.contains("Title"))
	{
	  ma.title->clear();
	  ma.title->setPalette(te_orig_pal);
	  ma.title->setFocus();
	}
      else if(name.contains("Volume"))
	{
	  ma.volume->setValue(ma.volume->minimum());
	  ma.volume->setFocus();
	}
      else if(name.contains("Issue"))
	{
	  ma.issue->setValue(ma.issue->minimum());
	  ma.issue->setFocus();
	}
      else if(name.contains("Publication Date"))
	{
	  if(windowTitle().contains("Search"))
	    ma.publication_date->setDate
	      (QDate::fromString("01/01/7999", "MM/dd/yyyy"));
	  else
	    ma.publication_date->setDate
	      (QDate::fromString("01/01/2000", "MM/dd/yyyy"));

	  ma.publication_date->setPalette(dt_orig_pal);
	  ma.publication_date->setFocus();
	}
      else if(name.contains("Publisher"))
	{
	  ma.publisher->clear();
	  ma.publisher->viewport()->setPalette(te_orig_pal);
	  ma.publisher->setFocus();
	}
      else if(name.contains("Categories"))
	{
	  ma.category->clear();
	  ma.category->viewport()->setPalette(te_orig_pal);
	  ma.category->setFocus();
	}
      else if(name.contains("Price"))
	{
	  ma.price->setValue(ma.price->minimum());
	  ma.price->setFocus();
	}
      else if(name.contains("Language"))
	{
	  ma.language->setCurrentIndex(0);
	  ma.language->setFocus();
	}
      else if(name.contains("Monetary Units"))
	{
	  ma.monetary_units->setCurrentIndex(0);
	  ma.monetary_units->setFocus();
	}
      else if(name.contains("Abstract"))
	{
	  ma.description->clear();
	  ma.description->viewport()->setPalette(te_orig_pal);
	  ma.description->setFocus();
	}
      else if(name.contains("Copies"))
	{
	  ma.quantity->setValue(ma.quantity->minimum());
	  ma.quantity->setFocus();
	}
      else if(name.contains("Location"))
	{
	  ma.location->setCurrentIndex(0);
	  ma.location->setFocus();
	}
      else if(name.contains("LC Control Number"))
	{
	  ma.lcnum->clear();
	  ma.lcnum->setPalette(ma.url->viewport()->palette());
	  ma.lcnum->setFocus();
	}
      else if(name.contains("Call Number"))
	{
	  ma.callnum->clear();
	  ma.callnum->setPalette(ma.url->viewport()->palette());
	  ma.callnum->setFocus();
	}
      else if(name.contains("Dewey Number"))
	{
	  ma.deweynum->clear();
	  ma.deweynum->setPalette(ma.url->viewport()->palette());
	  ma.deweynum->setFocus();
	}
      else if(name.contains("OFFSYSTEM URL"))
	{
	  ma.url->clear();
	  ma.url->setFocus();
	}
    }
  else
    {
      /*
      ** Reset all.
      */

      ma.id->clear();
      ma.id->setCursorPosition(0);
      ma.title->clear();
      ma.category->clear();
      ma.volume->setValue(ma.volume->minimum());
      ma.issue->setValue(ma.issue->minimum());
      ma.price->setValue(ma.price->minimum());

      if(windowTitle().contains("Search"))
	ma.publication_date->setDate
	  (QDate::fromString("01/01/7999", "MM/dd/yyyy"));
      else
	ma.publication_date->setDate
	  (QDate::fromString("01/01/2000", "MM/dd/yyyy"));

      ma.publisher->clear();
      ma.language->setCurrentIndex(0);
      ma.monetary_units->setCurrentIndex(0);
      ma.description->clear();
      ma.quantity->setValue(ma.quantity->minimum());
      ma.location->setCurrentIndex(0);
      ma.lcnum->clear();
      ma.callnum->clear();
      ma.deweynum->clear();
      ma.front_image->clear();
      ma.back_image->clear();
      ma.url->clear();
      ma.id->setPalette(te_orig_pal);
      ma.category->viewport()->setPalette(te_orig_pal);
      ma.lcnum->setPalette(ma.url->viewport()->palette());
      ma.callnum->setPalette(ma.url->viewport()->palette());
      ma.deweynum->setPalette(ma.url->viewport()->palette());
      ma.title->setPalette(te_orig_pal);
      ma.publication_date->setPalette(dt_orig_pal);
      ma.description->viewport()->setPalette(te_orig_pal);
      ma.publisher->viewport()->setPalette(te_orig_pal);
      ma.id->setFocus();
    }
}

/*
** -- closeEvent() --
*/

void qtbook_magazine::closeEvent(QCloseEvent *e)
{
  if(windowTitle().contains("Modify"))
    if(hasDataChanged(this))
      if(QMessageBox::question(this, "BiblioteQ: Question",
			       "You have unsaved data. Continue closing?",
			       QMessageBox::Yes | QMessageBox::No,
			       QMessageBox::No) == QMessageBox::No)
	{
	  e->ignore();
	  return;
	}

  qmain->removeMagazine(this);
}

/*
** -- slotCancel() --
*/

void qtbook_magazine::slotCancel(void)
{
  close();
}

/*
** -- slotPopulateCopiesEditor() --
*/

void qtbook_magazine::slotPopulateCopiesEditor(void)
{
  copy_editor *copyeditor = NULL;

  if((copyeditor = new copy_editor
      (qobject_cast<QWidget *>(this),
       (qtbook_item *) this,
       false,
       ma.quantity->value(), oid,
       ma.id->text(),
       ma.quantity, font(), subType)) != NULL)
    copyeditor->populateCopiesEditor();
}

/*
** -- slotShowUsers() --
*/

void qtbook_magazine::slotShowUsers(void)
{
  int state = 0;
  borrowers_editor *borrowerseditor = NULL;

  if(!ma.okButton->isHidden())
    state = qtbook::EDITABLE;
  else
    state = qtbook::VIEW_ONLY;

  if((borrowerseditor = new borrowers_editor
      (qobject_cast<QWidget *>(this), (qtbook_item *) this,
       ma.quantity->value(), oid, ma.id->text(), font(), subType,
       state)) != NULL)
    borrowerseditor->showUsers();
}

/*
** -- slotQuery() --
*/

void qtbook_magazine::slotQuery(void)
{
  int i = 0;
  QString etype = "";
  QString errorstr = "";
  QString searchstr = "";
  locresults *dialog = NULL;
  QStringList list;

  if(thread != NULL)
    {
      QMessageBox::critical
	(this, "BiblioteQ: User Error", 
	 "A query is already in progress. Please allow it to complete.");
      return;
    }

  if(ma.id->text().trimmed().length() != 9)
    {
      QMessageBox::critical
	(this, "BiblioteQ: User Error", 
	 "In order to query the Library of Congress, the ISSN "
	 "must be provided.");
      ma.id->setFocus();
      return;
    }

  if((thread = new generic_thread()) != NULL)
    {
      searchstr = QString("@attr 1=8 %1").arg(ma.id->text());
      thread->setType(generic_thread::QUERY_LIBRARY_OF_CONGRESS);
      thread->setLOCSearchString(searchstr);
      thread->start();
      qapp->setOverrideCursor(Qt::WaitCursor);

      while(thread->isRunning())
	{
	  qapp->processEvents();
	  thread->wait(100);
	}

      qapp->restoreOverrideCursor();

      if((errorstr = thread->getErrorStr()).isEmpty() && isVisible())
	{
	  if(thread->getLOCResults().size() == 1)
	    {
	      if(QMessageBox::question
		 (this, "BiblioteQ: Question",
		  "Replace existing values with those retrieved "
		  "from the Library of Congress?",
		  QMessageBox::Yes | QMessageBox::No,
		  QMessageBox::No) == QMessageBox::Yes)
		{
		  list = QString(thread->getLOCResults()[0]).split("\n");
		  populateDisplayAfterLOC(list);
		  list.clear();
		}
	    }
	  else if(thread->getLOCResults().size() > 1)
	    {
	      for(i = 0; i < thread->getLOCResults().size(); i++)
		list.append(thread->getLOCResults()[i]);

	      /*
	      ** Display a selection dialog.
	      */

	      if((dialog = new locresults((QWidget *) this, list,
					  this, font())) == NULL)
		{
		  qmain->addError
		    (QString("Memory Error"),
		     QString("Unable to create a \"dialog\" object "
			     "due to insufficient resources."),
		     QString(""),
		     __FILE__, __LINE__);
		  QMessageBox::critical
		    (this, "BiblioteQ: Memory Error",
		     "Unable to create a \"dialog\" object "
		     "due to insufficient resources.");
		}
	    }
	  else
	    QMessageBox::critical
	      (this, "BiblioteQ: Z39.50 Query Error",
	       "A Library of Congress entry may not yet exist for " +
	       ma.id->text() + ".");
	}
      else
	etype = thread->getEType();

      delete thread;
      thread = NULL;
    }
  else
    {
      etype = "Memory Error";
      errorstr = "Unable to create a thread due to insufficient resources.";
    }

  if(!errorstr.isEmpty() && isVisible())
    {
      qmain->addError(QString("Z39.50 Query Error"), etype, errorstr,
		      __FILE__, __LINE__);
      QMessageBox::critical
	(this, "BiblioteQ: Z39.50 Query Error",
	 "The Library of Congress entry could not be retrieved.");
    }
}

/*
** -- slotPrint() --
*/

void qtbook_magazine::slotPrint(void)
{
  html = "";
  html += "<b>ISSN:</b> " + ma.id->text().trimmed() + "<br>";
  html += "<b>Volume:</b> " + ma.volume->text() + "<br>";
  html += "<b>Issue (Number):</b> " + ma.issue->text() + "<br>";
  html += "<b>LC Control Number:</b> " + ma.lcnum->text().trimmed() + "<br>";
  html += "<b>Call Number:</b> " + ma.callnum->text().trimmed() + "<br>";
  html += "<b>Dewey Class Number:</b> " + ma.deweynum->text().trimmed() +
    "<br>";

  /*
  ** General information.
  */

  html += "<b>Title:</b> " + ma.title->text().trimmed() + "<br>";
  html += "<b>Publication Date:</b> " + ma.publication_date->date().
    toString("MM/dd/yyyy") + "<br>";
  html += "<b>Publisher:</b> " + ma.publisher->toPlainText().trimmed() +
    "<br>";
  html += "<b>Category:</b> " + ma.category->toPlainText().trimmed() + "<br>";
  html += "<b>Price:</b> " + ma.price->text() + "<br>";
  html += "<b>Language:</b> " + ma.language->currentText() + "<br>";
  html += "<b>Monetary Units:</b> " + ma.monetary_units->currentText() +
    "<br>";
  html += "<b>Copies:</b> " + ma.quantity->text() + "<br>";
  html += "<b>Location:</b> " + ma.location->currentText() + "<br>";
  html += "<b>Abstract:</b> " + ma.description->toPlainText().trimmed() +
    "<br>";
  html += "<b>OFFSYSTEM URL:</b>" + ma.url->toPlainText().trimmed();
  print(this);
}

/*
** -- populateDisplayAfterLOC() --
*/

void qtbook_magazine::populateDisplayAfterLOC(const QStringList &list)
{
  int i = 0;
  int j = 0;
  QString str = "";
  QStringList tmplist;

  for(i = 0; i < list.size(); i++)
    {
      str = list[i];

      if(str.startsWith("010"))
	{
	  str = str.mid(str.indexOf("$a") + 2).trimmed();
	  ma.lcnum->setText(str);
	  misc_functions::highlightWidget
	    (ma.lcnum, QColor(162, 205, 90));
	}
      else if(str.startsWith("050"))
	{
	  str = str.mid(str.indexOf("$a") + 2).trimmed();
	  str = str.remove(" $b").trimmed();
	  ma.callnum->setText(str);
	  misc_functions::highlightWidget
	    (ma.callnum, QColor(162, 205, 90));
	}
      else if(str.startsWith("082"))
	{
	  str = str.mid(str.indexOf("$a") + 2).trimmed();
	  str = str.remove(" $2").trimmed();
	  ma.deweynum->setText(str);
	  misc_functions::highlightWidget
	    (ma.deweynum, QColor(162, 205, 90));
	}
      else if(str.startsWith("245"))
	{
	  str = str.mid(str.indexOf("$a") + 2,
			str.indexOf("/") - str.indexOf("$a") - 2).trimmed();

	  /*
	  ** Let's perform some additional massaging.
	  */

	  if(str.contains(" : "))
	    str.replace(" : ", ": ");
	  else if(str.contains(" ; "))
	    str.replace(" ; ", "; ");

	  tmplist = str.split(" ");
	  str = "";

	  for(j = 0; j < tmplist.size(); j++)
	    if(j == 0 || j == tmplist.size() - 1)
	      str += tmplist[j].mid(0, 1).toUpper() +
		tmplist[j].mid(1) + " ";
	    else if(tmplist[j] == "a" || tmplist[j] == "an" ||
		    tmplist[j] == "and" || tmplist[j] == "but" ||
		    tmplist[j] == "of" || tmplist[j] == "or" ||
		    tmplist[j] == "the")
	      {
		if(tmplist[j - 1].contains(":"))
		  str += tmplist[j].mid(0, 1).toUpper() +
		    tmplist[j].mid(1) + " ";
		else
		  str += tmplist[j] + " ";
	      }
	    else
	      str += tmplist[j].mid(0, 1).toUpper() + tmplist[j].mid(1) + " ";

	  tmplist.clear();
	  ma.title->setText(str.trimmed());
	  misc_functions::highlightWidget
	    (ma.title, QColor(162, 205, 90));
	}
      else if(str.startsWith("260"))
	{
	  ma.publication_date->setDate
	    (QDate::fromString("01/01/" +
			       str.mid(str.trimmed().length() - 5, 4),
			       "MM/dd/yyyy"));
	  ma.publication_date->setStyleSheet
	    ("background-color: rgb(162, 205, 90)");
	  str = str.mid(str.indexOf("$b") + 2).trimmed();
	  str = str.mid(0, str.indexOf(",")).trimmed();
	  ma.publisher->setPlainText(str);
	  misc_functions::highlightWidget
	    (ma.publisher->viewport(), QColor(162, 205, 90));
	}
      else if(str.startsWith("300"))
	{
	  str = str.mid(str.indexOf("$a") + 2).trimmed();
	  str = str.remove(" $b").trimmed();
	  str = str.remove(" $c").trimmed();
	  ma.description->setPlainText(str);
	  misc_functions::highlightWidget
	    (ma.description->viewport(), QColor(162, 205, 90));
	}
      else if(str.startsWith("650"))
	{
	  str = str.mid(str.indexOf("$a") + 2).trimmed();
	  str = str.mid(0, str.indexOf("$v")).trimmed();

	  if(ma.category->toPlainText() == "N/A")
	    ma.category->clear();

	  if(!ma.category->toPlainText().contains(str))
	    {
	      if(!ma.category->toPlainText().isEmpty())
		ma.category->setPlainText
		  (ma.category->toPlainText() + "\n" + str);
	      else
		ma.category->setPlainText(str);

	      misc_functions::highlightWidget
		(ma.category->viewport(), QColor(162, 205, 90));
	    }
	}
    }

  foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
    textfield->setCursorPosition(0);
}

/*
** -- isBusy() --
*/

bool qtbook_magazine::isBusy(void)
{
  if(thread != NULL)
    return true;
  else
    return false;
}

/*
** -- slotSelectImage() --
*/

void qtbook_magazine::slotSelectImage(void)
{
  QFileDialog dialog(this);
  QPushButton *button = qobject_cast<QPushButton *> (sender());

  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setFilter("Image Files (*.bmp *.jpg *.jpeg *.png)");

  if(button == ma.frontButton)
    dialog.setWindowTitle("Front Cover Image Selection");
  else
    dialog.setWindowTitle("Back Cover Image Selection");

  dialog.exec();

  if(dialog.result() == QDialog::Accepted)
    if(button == ma.frontButton)
      {
	ma.front_image->clear();
	ma.front_image->image = QImage(dialog.selectedFiles().at(0));
	ma.front_image->imageFormat = dialog.selectedFiles().at(0).mid
	  (dialog.selectedFiles().at(0).lastIndexOf(".") + 1).toUpper();
	ma.front_image->scene()->addPixmap
	  (QPixmap().fromImage(ma.front_image->image));
	ma.front_image->scene()->items().at(0)->setFlags
	  (QGraphicsItem::ItemIsSelectable);
      }
    else
      {
	ma.back_image->clear();
	ma.back_image->image = QImage(dialog.selectedFiles().at(0));
	ma.back_image->imageFormat = dialog.selectedFiles().at(0).mid
	  (dialog.selectedFiles().at(0).lastIndexOf(".") + 1).toUpper();
	ma.back_image->scene()->addPixmap
	  (QPixmap().fromImage(ma.back_image->image));
	ma.back_image->scene()->items().at(0)->setFlags
	  (QGraphicsItem::ItemIsSelectable);
      }
}
