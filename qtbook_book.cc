/*
** Includes book-specific methods.
*/

/*
** -- Local Includes --
*/

#include "qtbook_book.h"

extern qtbook *qmain;
extern QApplication *qapp;

/*
** -- qtbook_book() --
*/

qtbook_book::qtbook_book(QMainWindow *parentArg,
			 const QStringList &categories,
			 const QStringList &languages,
			 const QStringList &monetary_units,
			 const QStringList &locations, const QString &oidArg,
			 const int rowArg):
  QMainWindow()
{
  QMenu *menu = NULL;
  QRegExp rx1("[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9X]");
  QRegExp rx2("[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]"
	      "[0-9][0-9][0-9]");
  QValidator *validator1 = NULL;
  QValidator *validator2 = NULL;
  QGraphicsScene *scene1 = NULL;
  QGraphicsScene *scene2 = NULL;

  if((menu = new QMenu()) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((validator1 = new QRegExpValidator(rx1, this)) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((validator2 = new QRegExpValidator(rx2, this)) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene1 = new QGraphicsScene()) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene2 = new QGraphicsScene()) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  thread = NULL;
  parentWid = parentArg;
  oid = oidArg;
  row = rowArg;
  oldq = misc_functions::getColumnString
    (qmain->getUI().table, row, "Quantity").toInt();
  id.setupUi(this);
  updateFont(qapp->font(), (QWidget *) this);
  connect(id.okButton, SIGNAL(clicked(void)), this, SLOT(slotGo(void)));
  connect(id.showUserButton, SIGNAL(clicked(void)), this,
	  SLOT(slotShowUsers(void)));
  connect(id.queryButton, SIGNAL(clicked(void)), this,
	  SLOT(slotQuery(void)));
  connect(id.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCancel(void)));
  connect(id.copiesButton, SIGNAL(clicked()), this,
	  SLOT(slotPopulateCopiesEditor(void)));
  connect(id.resetButton, SIGNAL(clicked(void)), this,
	  SLOT(slotReset(void)));
  connect(id.isbn10to13, SIGNAL(clicked(void)), this,
	  SLOT(slotConvertISBN10to13(void)));
  connect(id.printButton, SIGNAL(clicked(void)), this, SLOT(slotPrint(void)));
  connect(menu->addAction("Reset &Front Cover Image"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Back Cover Image"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &ISBN-10"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &ISBN-13"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Edition"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Author(s)"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Book Binding Type"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &LC Control Number"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Call Number"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Dewey Class Number"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Title"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Publication Date"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Publisher"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Category"),
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
  connect(id.frontButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(id.backButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(id.generate, SIGNAL(clicked(void)), this,
	  SLOT(slotGenerateISBN(void)));
  id.id->setValidator(validator1);
  id.isbn13->setValidator(validator2);
  id.resetButton->setMenu(menu);
  id.category->addItems(categories);
  id.language->addItems(languages);
  id.monetary_units->addItems(monetary_units);
  id.location->addItems(locations);
  id.front_image->setScene(scene1);
  id.back_image->setScene(scene2);

  if(id.category->count() == 0)
    id.category->addItem("UNKNOWN");

  if(id.language->count() == 0)
    id.language->addItem("UNKNOWN");

  if(id.monetary_units->count() == 0)
    id.monetary_units->addItem("UNKNOWN");

  if(id.location->count() == 0)
    id.location->addItem("UNKNOWN");

  /*
  ** Save some palettes.
  */

  cb_orig_pal = id.edition->palette();
  dt_orig_pal = id.publication_date->palette();
  te_orig_pal = id.author->viewport()->palette();

  /*
  ** Prepare the form.
  */

  resize(baseSize());
  misc_functions::center(this, parentWid);
  misc_functions::hideAdminFields(this, qmain->getRoles());
}

/*
** -- ~qtbook_book() --
*/

qtbook_book::~qtbook_book()
{
  if(thread != NULL && thread->isRunning())
    qapp->restoreOverrideCursor();
}

/*
** -- slotGo() --
*/

void qtbook_book::slotGo(void)
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
	  newq = id.quantity->value();
	  qapp->setOverrideCursor(Qt::WaitCursor);
	  maxcopynumber = misc_functions::getMaxCopyNumber
	    (qmain->getDB(), oid, "Book", errorstr);

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
	      QMessageBox::critical(this, "BiblioteQ: User Error",
				    "It appears that you are attempting to "
				    "decrease the "
				    "number of copies while there are copies "
				    "that have been reserved.");
	      id.quantity->setValue(oldq);
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
      str = id.id->text().trimmed();
      id.id->setText(str);
      slotConvertISBN10to13();
      str = id.isbn13->text().trimmed();
      id.isbn13->setText(str);

      if(id.id->text().isEmpty() || id.isbn13->text().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete both the "
				"ISBN-10 and ISBN-13 fields.");

	  if(id.id->text().isEmpty())
	    id.id->setFocus();
	  else
	    id.isbn13->setFocus();

	  goto db_rollback;
	}

      str = id.author->toPlainText().trimmed();
      id.author->setPlainText(str);

      if(id.author->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Author(s) field.");
	  id.author->setFocus();
	  goto db_rollback;
	}

      str = id.title->text().trimmed();
      id.title->setText(str);

      if(id.title->text().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Title field.");
	  id.title->setFocus();
	  goto db_rollback;
	}

      str = id.publisher->toPlainText().trimmed();
      id.publisher->setPlainText(str);

      if(id.publisher->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Publisher field.");
	  id.publisher->setFocus();
	  goto db_rollback;
	}

      str = id.description->toPlainText().trimmed();
      id.description->setPlainText(str);

      if(id.description->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Abstract field.");
	  id.description->setFocus();
	  goto db_rollback;
	}

      str = id.lcnum->text().trimmed();
      id.lcnum->setText(str);
      str = id.callnum->text().trimmed();
      id.callnum->setText(str);
      str = id.deweynum->text().trimmed();
      id.deweynum->setText(str);
      str = id.url->toPlainText().trimmed();
      id.url->setPlainText(str);

      if(windowTitle().contains("Modify"))
	query.prepare(QString("UPDATE book SET id = ?, "
			      "title = ?, "
			      "edition = ?, author = ?, "
			      "pdate = ?, "
			      "publisher = ?, "
			      "category = ?, price = ?, "
			      "description = ?, "
			      "language = ?, "
			      "monetary_units = ?, "
			      "quantity = ?, "
			      "binding_type = ?, "
			      "location = ?, "
			      "isbn13 = ?, "
			      "lccontrolnumber = ?, "
			      "callnumber = ?, "
			      "deweynumber = ?, "
			      "front_cover = ?, "
			      "back_cover = ?, "
			      "offsystem_url = ? "
			      "WHERE "
			      "myoid = ?"));
      else if(qmain->getDB().driverName() != "QSQLITE")
	query.prepare(QString("INSERT INTO book (id, title, "
			      "edition, author, pdate, publisher, "
			      "category, price, description, language, "
			      "monetary_units, quantity, "
			      "binding_type, location, "
			      "isbn13, lccontrolnumber, callnumber, "
			      "deweynumber, front_cover, "
			      "back_cover, "
			      "offsystem_url) "
			      "VALUES (?, ?, "
			      "?, ?, ?, ?, "
			      "?, ?, ?, "
			      "?, ?, "
			      "?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"));
      else
	query.prepare(QString("INSERT INTO book (id, title, "
			      "edition, author, pdate, publisher, "
			      "category, price, description, language, "
			      "monetary_units, quantity, "
			      "binding_type, location, "
			      "isbn13, lccontrolnumber, callnumber, "
			      "deweynumber, front_cover, "
			      "back_cover, "
			      "offsystem_url, myoid) "
			      "VALUES (?, ?, "
			      "?, ?, ?, ?, "
			      "?, ?, ?, "
			      "?, ?, "
			      "?, ?, ?, ?, ?, ?, ?, ?, ?, "
			      "?, ?)"));

      query.bindValue(0, id.id->text());
      query.bindValue(1, id.title->text());
      query.bindValue(2, id.edition->currentText().trimmed());
      query.bindValue(3, id.author->toPlainText());
      query.bindValue(4, id.publication_date->date().toString("MM/dd/yyyy"));
      query.bindValue(5, id.publisher->toPlainText());
      query.bindValue(6, id.category->currentText().trimmed());
      query.bindValue(7, id.price->text());
      query.bindValue(8, id.description->toPlainText());
      query.bindValue(9, id.language->currentText().trimmed());
      query.bindValue(10, id.monetary_units->currentText().trimmed());
      query.bindValue(11, id.quantity->text());
      query.bindValue(12, id.binding->currentText().trimmed());
      query.bindValue(13, id.location->currentText().trimmed());
      query.bindValue(14, id.isbn13->text());

      if(!id.lcnum->text().isEmpty())
	query.bindValue(15, id.lcnum->text());
      else
	query.bindValue(15, QVariant(QVariant::String));

      if(!id.callnum->text().isEmpty())
	query.bindValue(16, id.callnum->text());
      else
	query.bindValue(16, QVariant(QVariant::String));

      if(!id.deweynum->text().isEmpty())
	query.bindValue(17, id.deweynum->text());
      else
	query.bindValue(17, QVariant(QVariant::String));

      if(!id.front_image->image.isNull())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);
	  buffer.open(QIODevice::WriteOnly);
	  id.front_image->image.save
	    (&buffer, id.front_image->imageFormat.toAscii(), 100);
	  query.bindValue(18, bytes);
	}
      else
	{
	  id.front_image->imageFormat = "";
	  query.bindValue(18, QVariant());
	}

      if(!id.back_image->image.isNull())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);
	  buffer.open(QIODevice::WriteOnly);
	  id.back_image->image.save
	    (&buffer, id.back_image->imageFormat.toAscii(), 100);
	  query.bindValue(19, bytes);
	}
      else
	{
	  id.back_image->imageFormat = "";
	  query.bindValue(19, QVariant());
	}

      if(!id.url->toPlainText().isEmpty())
	query.bindValue(20, id.url->toPlainText().trimmed());
      else
	query.bindValue(20, QVariant(QVariant::String));

      if(windowTitle().contains("Modify"))
	query.bindValue(21, oid);
      else if(qmain->getDB().driverName() == "QSQLITE")
	query.bindValue(21, id.id->text().replace("X", "10"));

      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!query.exec())
	{
	  qapp->restoreOverrideCursor();
	  qmain->addError(QString("Database Error"),
			  QString("Unable to create or update the entry."),
			  query.lastError().text(), __FILE__, __LINE__);
	  QMessageBox::critical(this, "BiblioteQ: Database Error",
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
	      query.prepare(QString("DELETE FROM book_copy_info WHERE "
				    "copy_number > ? AND "
				    "item_oid = "
				    "?"));
	      query.bindValue(0, id.quantity->text());
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
		     qmain->getDB().lastError().text(), __FILE__,
		     __LINE__);
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

	      misc_functions::createInitialCopies(id.id->text(),
						  id.quantity->value(),
						  qmain->getDB(),
						  "Book", errorstr);

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

	  id.id->setPalette(te_orig_pal);
	  id.isbn13->setPalette(te_orig_pal);
	  id.edition->setPalette(cb_orig_pal);
	  id.category->setPalette(cb_orig_pal);
	  id.publication_date->setPalette(dt_orig_pal);
	  id.author->viewport()->setPalette(te_orig_pal);
	  id.title->setPalette(te_orig_pal);
	  id.description->viewport()->setPalette(te_orig_pal);
	  id.publisher->viewport()->setPalette(te_orig_pal);
	  id.lcnum->setPalette(id.url->viewport()->palette());
	  id.callnum->setPalette(id.url->viewport()->palette());
	  id.deweynum->setPalette(id.url->viewport()->palette());
	  oldq = id.quantity->value();

	  if(id.front_image->image.isNull())
	    id.front_image->imageFormat = "";

	  if(id.back_image->image.isNull())
	    id.back_image->imageFormat = "";

	  id.author->setMultipleLinks("book_search", "author",
				      id.author->toPlainText());
	  id.publisher->setText
	    (QString("<a href=\"book_search?publisher?%1\">" +
		     id.publisher->toPlainText() + "</a>").arg
	     (id.publisher->toPlainText()));

	  if(!id.url->toPlainText().isEmpty())
	    id.url->setText(QString("<a href=\"%1\">%1</a>").arg
			    (id.url->toPlainText()));

	  qapp->restoreOverrideCursor();

	  if(windowTitle().contains("Modify"))
	    {
	      str = QString("BiblioteQ: Modify Book Entry (%1)").arg
		(id.id->text());
	      setWindowTitle(str);

	      if((qmain->getUI().typefilter->currentText() == "All" ||
		  qmain->getUI().typefilter->currentText() == "Books") &&
		 oid == misc_functions::getColumnString(qmain->getUI().table,
							row, "OID"))
		{
		  for(i = 0; i < qmain->getUI().table->columnCount(); i++)
		    {
		      column = qmain->getUI().table->horizontalHeaderItem(i);

		      if(column == NULL)
			continue;

		      if(column->text() == "ISBN-10" ||
			 column->text() == "ID Number")
			qmain->getUI().table->item(row, i)->setText
			  (id.id->text());
		      else if(column->text() == "Title")
			qmain->getUI().table->item(row, i)->setText
			  (id.title->text());
		      else if(column->text() == "Edition")
			qmain->getUI().table->item(row, i)->setText
			  (id.edition->currentText().trimmed());
		      else if(column->text() == "Author(s)")
			qmain->getUI().table->item(row, i)->setText
			  (id.author->toPlainText());
		      else if(column->text() == "Publication Date")
			qmain->getUI().table->item(row, i)->setText
			  (id.publication_date->date().toString("MM/dd/yyyy"));
		      else if(column->text() == "Publisher")
			qmain->getUI().table->item(row, i)->setText
			  (id.publisher->toPlainText());
		      else if(column->text() == "Category")
			qmain->getUI().table->item(row, i)->setText
			  (id.category->currentText().trimmed());
		      else if(column->text() == "Price")
			qmain->getUI().table->item(row, i)->setText
			  (id.price->text());
		      else if(column->text() == "Language")
			qmain->getUI().table->item(row, i)->setText
			  (id.language->currentText().trimmed());
		      else if(column->text() == "Monetary Units")
			qmain->getUI().table->item(row, i)->setText
			  (id.monetary_units->currentText().trimmed());
		      else if(column->text() == "Quantity")
			qmain->getUI().table->item(row, i)->setText
			  (id.quantity->text());
		      else if(column->text() == "Book Binding Type")
			qmain->getUI().table->item(row, i)->setText
			  (id.binding->currentText().trimmed());
		      else if(column->text() == "Location")
			qmain->getUI().table->item(row, i)->setText
			  (id.location->currentText().trimmed());
		      else if(column->text() == "ISBN-13")
			qmain->getUI().table->item(row, i)->setText
			  (id.isbn13->text());
		      else if(column->text() == "LC Control Number")
			qmain->getUI().table->item(row, i)->setText
			  (id.lcnum->text());
		      else if(column->text() == "Call Number")
			qmain->getUI().table->item(row, i)->setText
			  (id.callnum->text());
		      else if(column->text() == "Dewey Class Number")
			qmain->getUI().table->item(row, i)->setText
			  (id.deweynum->text());
		      else if(column->text() == "Availability")
			{
			  qmain->getUI().table->item(row, i)->setText
			    (misc_functions::getAvailability
			     (oid, qmain->getDB(), "Book",
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
	      oid = misc_functions::getOID(id.id->text(),
					   "Book",
					   qmain->getDB(),
					   errorstr);
	      qapp->restoreOverrideCursor();

	      if(!errorstr.isEmpty())
		{
		  oid = "insert";
		  qmain->addError(QString("Database Error"),
				  QString("Unable to retrieve the book's "
					  "OID."),
				  errorstr, __FILE__, __LINE__);
		  QMessageBox::critical(this, "BiblioteQ: Database Error",
					"Unable to retrieve the book's "
					"OID.");
		}
	      else
		qmain->replaceBook("insert", this);

	      updateWindow(qtbook::EDITABLE);

	      if(qmain->getUI().actionAutoPopulateOnCreation->isChecked())
		(void) qmain->populateTable
		  (qtbook::POPULATE_ALL, "Books", QString(""));

	      raise();
	    }

	  storeData(this);
	}

      return;

    db_rollback:

      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!qmain->getDB().rollback())
	qmain->addError
	  (QString("Database Error"), QString("Rollback failure."),
	   qmain->getDB().lastError().text(), __FILE__, __LINE__);

      qapp->restoreOverrideCursor();
    }
  else
    {
      searchstr = "SELECT book.title, "
	"book.author, "
	"book.publisher, book.pdate, book.edition, "
	"book.category, book.language, book.id, "
	"book.price, book.monetary_units, "
	"book.quantity, "
	"book.binding_type, "
	"book.location, "
	"book.isbn13, "
	"book.lccontrolnumber, "
	"book.callnumber, "
	"book.deweynumber, "
	"book.quantity - COUNT(item_borrower_vw.item_oid) "
	"AS availability, "
	"book.type, "
	"book.myoid "
	"FROM "
	"book LEFT JOIN item_borrower_vw ON "
	"book.myoid = item_borrower_vw.item_oid "
	"AND item_borrower_vw.type = 'Book' "
	"WHERE ";
      searchstr.append("LOWER(id) LIKE '%" + id.id->text().toLower() +
		       "%' AND ");
      searchstr.append("LOWER(title) LIKE '%" +
		       myqstring::escape(id.title->text().toLower()) +
		       "%' AND ");
      searchstr.append("LOWER(isbn13) LIKE '%" +
		       id.isbn13->text().toLower() + "%' AND ");
      searchstr.append("LOWER(COALESCE(lccontrolnumber, '')) LIKE '%" +
		       myqstring::escape(id.lcnum->text().toLower()) +
		       "%' AND ");
      searchstr.append("LOWER(COALESCE(callnumber, '')) LIKE '%" +
		       myqstring::escape(id.callnum->text().toLower()) +
		       "%' AND ");
      searchstr.append("LOWER(COALESCE(deweynumber, '')) LIKE '%" +
		       myqstring::escape(id.deweynum->text().toLower()) +
		       "%' AND ");

      if(id.edition->currentText() != "Any")
	searchstr.append("edition = '" + id.edition->currentText() +
			 "' AND ");

      searchstr.append("LOWER(author) LIKE '%" +
		       myqstring::escape(id.author->toPlainText().
					 toLower()) + "%' AND ");

      if(id.publication_date->date().toString("MM/dd/yyyy") != "01/01/7999")
	searchstr.append("pdate = '" +
			 id.publication_date->date().toString
			 ("MM/dd/yyyy") +
			 "' AND ");

      searchstr.append("LOWER(publisher) LIKE '%" +
		       myqstring::escape
		       (id.publisher->toPlainText().toLower()) +
		       "%' AND ");

      if(id.category->currentText() != "Any")
	searchstr.append("category = '" +
			 myqstring::escape(id.category->currentText()) +
			 "' AND ");

      if(id.price->value() > 0)
	{
	  searchstr.append("price = ");
	  searchstr.append(id.price->text());
	  searchstr.append(" AND ");
	}

      if(id.language->currentText() != "Any")
	searchstr.append("language = '" +
			 myqstring::escape(id.language->currentText()) +
			 "' AND ");

      if(id.monetary_units->currentText() != "Any")
	searchstr.append("monetary_units = '" +
			 myqstring::escape
			 (id.monetary_units->currentText()) +
			 "' AND ");

      if(id.binding->currentText() != "Any")
	searchstr.append("binding_type = '" +
			 myqstring::escape(id.binding->currentText()) +
			 "' AND ");

      searchstr.append("LOWER(description) LIKE '%" +
		       myqstring::escape
		       (id.description->toPlainText().toLower()) +
		       "%' ");

      if(id.quantity->value() != 0)
	searchstr.append("AND quantity = " + id.quantity->text() + " ");

      if(id.location->currentText() != "Any")
	searchstr.append("AND location = '" +
			 myqstring::escape
			 (id.location->currentText()) + "' ");

      if(!id.url->toPlainText().isEmpty())
	searchstr.append("AND LOWER(COALESCE(offsystem_url, '')) LIKE '%" +
			 myqstring::escape
			 (id.url->toPlainText().toLower()) + "%' ");

      slotCancel();

      /*
      ** Search the database.
      */

      (void) qmain->populateTable
	(qtbook::POPULATE_SEARCH, "Books", searchstr);
    }
}

/*
** -- search() --
*/

void qtbook_book::search(const QString &field, const QString &value)
{
  id.coverImages->setVisible(false);
  id.generate->setVisible(false);
  id.id->clear();
  id.isbn13->clear();
  id.author->clear();
  id.lcnum->clear();
  id.callnum->clear();
  id.deweynum->clear();
  id.title->clear();
  id.publisher->clear();
  id.description->clear();
  id.copiesButton->setVisible(false);
  id.showUserButton->setVisible(false);
  id.queryButton->setVisible(false);
  id.okButton->setText("&Search");
  id.publication_date->setDate(QDate::fromString("01/01/7999",
						 "MM/dd/yyyy"));
  id.price->setMinimum(0.00);
  id.price->setValue(0.00);
  id.quantity->setMinimum(0);
  id.quantity->setValue(0);

  /*
  ** Add "any".
  */

  if(id.edition->findText("Any") == -1)
    id.edition->insertItem(0, "Any");

  if(id.language->findText("Any") == -1)
    id.language->insertItem(0, "Any");

  if(id.category->findText("Any") == -1)
    id.category->insertItem(0, "Any");

  if(id.monetary_units->findText("Any") == -1)
    id.monetary_units->insertItem(0, "Any");

  if(id.binding->findText("Any") == -1)
    id.binding->insertItem(0, "Any");

  if(id.location->findText("Any") == -1)
    id.location->insertItem(0, "Any");

  id.location->setCurrentIndex(0);
  id.edition->setCurrentIndex(0);
  id.category->setCurrentIndex(0);
  id.language->setCurrentIndex(0);
  id.monetary_units->setCurrentIndex(0);
  id.binding->setCurrentIndex(0);
  id.url->clear();

  if(field.isEmpty() && value.isEmpty())
    {
      foreach(QAction *action,
	      id.resetButton->menu()->findChildren<QAction *>())
	if(action->text().contains("Cover Image"))
	  action->setVisible(false);

      setWindowTitle("BiblioteQ: Database Book Search");
      id.id->setFocus();
      misc_functions::center(this, parentWid);
      show();
    }
  else
    {
      if(field == "publisher")
	id.publisher->setPlainText(value);
      else if(field == "author")
	id.author->setPlainText(value);

      slotGo();
    }
}

/*
** -- updateWindow() --
*/

void qtbook_book::updateWindow(const int state)
{
  QString str = "";

  if(state == qtbook::EDITABLE)
    {
      id.copiesButton->setEnabled(true);
      id.showUserButton->setEnabled(true);
      id.okButton->setVisible(true);
      id.queryButton->setVisible(true);
      id.resetButton->setVisible(true);
      id.frontButton->setVisible(true);
      id.backButton->setVisible(true);
      str = QString("BiblioteQ: Modify Book Entry (%1)").arg(id.id->text());
    }
  else
    {
      id.copiesButton->setEnabled(false);
      id.showUserButton->setEnabled(true);
      id.okButton->setVisible(false);
      id.queryButton->setVisible(false);
      id.resetButton->setVisible(false);
      id.frontButton->setVisible(false);
      id.backButton->setVisible(false);
      str = QString("BiblioteQ: View Book Details (%1)").arg(id.id->text());
    }

  id.coverImages->setVisible(true);
  setWindowTitle(str);
}

/*
** -- modify() --
*/

void qtbook_book::modify(const int state)
{
  int i = 0;
  QString str = "";
  QString fieldname = "";
  QString searchstr = "";
  QVariant var;
  QSqlQuery query(qmain->getDB());

  if(state == qtbook::EDITABLE)
    {
      setWindowTitle("BiblioteQ: Modify Book Entry");
      id.copiesButton->setEnabled(true);
      id.showUserButton->setEnabled(true);
      id.okButton->setVisible(true);
      id.queryButton->setVisible(true);
      id.resetButton->setVisible(true);
      id.frontButton->setVisible(true);
      id.backButton->setVisible(true);
      misc_functions::highlightWidget
	(id.id, QColor(255, 248, 220));
      misc_functions::highlightWidget
	(id.isbn13, QColor(255, 248, 220));
      misc_functions::highlightWidget
	(id.title, QColor(255, 248, 220));
      misc_functions::highlightWidget
	(id.publisher->viewport(), QColor(255, 248, 220));
      misc_functions::highlightWidget
	(id.author->viewport(), QColor(255, 248, 220));
      misc_functions::highlightWidget
	(id.description->viewport(), QColor(255, 248, 220));
      te_orig_pal = id.id->palette();
    }
  else
    {
      setWindowTitle("BiblioteQ: View Book Details");
      id.copiesButton->setEnabled(false);
      id.showUserButton->setEnabled(true);
      id.okButton->setVisible(false);
      id.queryButton->setVisible(false);
      id.resetButton->setVisible(false);
      id.frontButton->setVisible(false);
      id.backButton->setVisible(false);

      foreach(QAction *action,
	      id.resetButton->menu()->findChildren<QAction *>())
	if(action->text().contains("Cover Image"))
	  action->setVisible(false);
    }

  id.quantity->setMinimum(1);
  id.price->setMinimum(0.01);
  id.okButton->setText("&Save");
  str = oid;
  searchstr = "SELECT title, "
    "author, "
    "publisher, pdate, edition, "
    "category, language, id, "
    "price, monetary_units, quantity, "
    "binding_type, "
    "location, "
    "isbn13, "
    "lccontrolnumber, "
    "callnumber, "
    "deweynumber, "
    "description, "
    "front_cover, "
    "back_cover, "
    "offsystem_url "
    "FROM book WHERE myoid = ";
  searchstr.append(str);
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(searchstr) || !query.next())
    {
      qapp->restoreOverrideCursor();
      qmain->addError(QString("Database Error"),
		      QString("Unable to retrieve the selected book's data."),
		      query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(this, "BiblioteQ: Database Error",
			    "Unable to retrieve the selected book's data.");
      return;
    }
  else
    {
      qapp->restoreOverrideCursor();

      for(i = 0; i < query.record().count(); i++)
	{
	  var = query.record().field(i).value();
	  fieldname = query.record().fieldName(i);

	  if(fieldname == "title")
	    id.title->setText(var.toString());
	  else if(fieldname == "author")
	    id.author->setMultipleLinks("book_search", "author",
					var.toString());
	  else if(fieldname == "publisher")
	    id.publisher->setText
	      (QString
	       ("<a href=\"book_search?publisher?%1\">" +
		var.toString() + "</a>").arg(var.toString()));
	  else if(fieldname == "pdate")
	    id.publication_date->setDate
	      (QDate::fromString(var.toString(), "MM/dd/yyyy"));
	  else if(fieldname == "edition")
	    {
	      if(id.edition->findText(var.toString()) > -1)
		id.edition->setCurrentIndex
		  (id.edition->findText(var.toString()));
	      else
		id.edition->setCurrentIndex(0);
	    }
	  else if(fieldname == "price")
	    id.price->setValue(var.toDouble());
	  else if(fieldname == "category")
	    {
	      if(id.category->findText(var.toString()) > -1)
		id.category->setCurrentIndex
		  (id.category->findText(var.toString()));
	      else
		id.category->setCurrentIndex
		  (id.category->findText("UNKNOWN"));
	    }
	  else if(fieldname == "language")
	    {
	      if(id.language->findText(var.toString()) > -1)
		id.language->setCurrentIndex
		  (id.language->findText(var.toString()));
	      else
		id.language->setCurrentIndex
		  (id.language->findText("UNKNOWN"));
	    }
	  else if(fieldname == "quantity")
	    id.quantity->setValue(var.toInt());
	  else if(fieldname == "monetary_units")
	    {
	      if(id.monetary_units->findText(var.toString()) > -1)
		id.monetary_units->setCurrentIndex
		  (id.monetary_units->findText(var.toString()));
	      else
		id.monetary_units->setCurrentIndex
		  (id.monetary_units->findText("UNKNOWN"));
	    }
	  else if(fieldname == "binding_type")
	    {
	      if(id.binding->findText(var.toString()) > -1)
		id.binding->setCurrentIndex
		  (id.binding->findText(var.toString()));
	      else
		id.binding->setCurrentIndex(0);
	    }
	  else if(fieldname == "location")
	    {
	      if(id.location->findText(var.toString()) > -1)
		id.location->setCurrentIndex
		  (id.location->findText(var.toString()));
	      else
		id.location->setCurrentIndex
		  (id.location->findText("UNKNOWN"));
	    }
	  else if(fieldname == "id")
	    {
	      if(state == qtbook::EDITABLE)
		str = QString("BiblioteQ: Modify Book Entry (%1)").arg
		  (var.toString());
	      else
		str = QString("BiblioteQ: View Book Details (%1)").arg
		  (var.toString());

	      id.id->setText(var.toString());
	      setWindowTitle(str);
	    }
	  else if(fieldname == "description")
	    id.description->setPlainText(var.toString());
	  else if(fieldname == "isbn13")
	    id.isbn13->setText(var.toString());
	  else if(fieldname == "lccontrolnumber")
	    id.lcnum->setText(var.toString());
	  else if(fieldname == "callnumber")
	    id.callnum->setText(var.toString());
	  else if(fieldname == "deweynumber")
	    id.deweynum->setText(var.toString());
	  else if(fieldname == "front_cover")
	    {
	      if(!query.record().field(i).isNull())
		{
		  id.front_image->loadFromData(var.toByteArray());
		  id.front_image->determineFormat(var.toByteArray());
		}
	    }
	  else if(fieldname == "back_cover")
	    {
	      if(!query.record().field(i).isNull())
		{
		  id.back_image->loadFromData(var.toByteArray());
		  id.back_image->determineFormat(var.toByteArray());
		}
	    }
	  else if(fieldname == "offsystem_url")
	    {
	      if(!query.record().field(i).isNull())
		id.url->setText(QString("<a href=\"%1\">%1</a>").arg
				(var.toString()));
	    }
	}

      foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
	textfield->setCursorPosition(0);

      storeData(this);
    }

  id.id->setFocus();
  showNormal();
  raise();
}

/*
** -- insert() --
*/

void qtbook_book::insert(void)
{
  slotReset();
  id.id->clear();
  id.isbn13->clear();
  id.author->setPlainText("N/A");
  id.lcnum->clear();
  id.callnum->clear();
  id.deweynum->clear();
  id.title->clear();
  id.publisher->setPlainText("N/A");
  id.description->setPlainText("N/A");
  id.copiesButton->setEnabled(false);
  id.queryButton->setVisible(true);
  id.okButton->setText("&Save");
  id.publication_date->setDate(QDate::fromString("01/01/2000",
						 "MM/dd/yyyy"));
  id.price->setMinimum(0.01);
  id.price->setValue(0.01);
  id.quantity->setMinimum(1);
  id.quantity->setValue(1);
  id.showUserButton->setEnabled(false);
  id.location->setCurrentIndex(0);
  id.edition->setCurrentIndex(0);
  id.category->setCurrentIndex(0);
  id.language->setCurrentIndex(0);
  id.monetary_units->setCurrentIndex(0);
  id.binding->setCurrentIndex(0);
  id.url->clear();
  misc_functions::highlightWidget
    (id.id, QColor(255, 248, 220));
  misc_functions::highlightWidget
    (id.isbn13, QColor(255, 248, 220));
  misc_functions::highlightWidget
    (id.title, QColor(255, 248, 220));
  misc_functions::highlightWidget
    (id.publisher->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (id.author->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (id.description->viewport(), QColor(255, 248, 220));
  te_orig_pal = id.id->palette();
  setWindowTitle("BiblioteQ: Create Book Entry");
  id.id->setFocus();
  misc_functions::center(this, parentWid);
  show();
}

/*
** -- slotReset() --
*/

void qtbook_book::slotReset(void)
{
  QAction *action = qobject_cast<QAction *>(sender());
  QString name = "";

  if(action != NULL)
    {
      name = action->text();

      if(name.contains("Front Cover Image"))
	id.front_image->clear();
      else if(name.contains("Back Cover Image"))
	id.back_image->clear();
      else if(name.contains("ISBN-10"))
	{
	  id.id->clear();
	  id.id->setPalette(te_orig_pal);
	  id.id->setFocus();
	}
      else if(name.contains("Title"))
	{
	  id.title->clear();
	  id.title->setPalette(te_orig_pal);
	  id.title->setFocus();
	}
      else if(name.contains("Edition"))
	{
	  id.edition->setCurrentIndex(0);
	  id.edition->setPalette(cb_orig_pal);
	  id.edition->setFocus();
	}
      else if(name.contains("Author(s)"))
	{
	  id.author->clear();

	  if(!windowTitle().contains("Search"))
	    id.author->setPlainText("N/A");

	  id.author->viewport()->setPalette(te_orig_pal);
	  id.author->setFocus();
	}
      else if(name.contains("Publication Date"))
	{
	  if(windowTitle().contains("Search"))
	    id.publication_date->setDate
	      (QDate::fromString("01/01/7999", "MM/dd/yyyy"));
	  else
	    id.publication_date->setDate
	      (QDate::fromString("01/01/2000", "MM/dd/yyyy"));

	  id.publication_date->setPalette(dt_orig_pal);
	  id.publication_date->setFocus();
	}
      else if(name.contains("Publisher"))
	{
	  id.publisher->clear();

	  if(!windowTitle().contains("Search"))
	    id.publisher->setPlainText("N/A");

	  id.publisher->viewport()->setPalette(te_orig_pal);
	  id.publisher->setFocus();
	}
      else if(name.contains("Category"))
	{
	  id.category->setCurrentIndex(0);
	  id.category->setPalette(cb_orig_pal);
	  id.category->setFocus();
	}
      else if(name.contains("Price"))
	{
	  id.price->setValue(id.price->minimum());
	  id.price->setFocus();
	}
      else if(name.contains("Language"))
	{
	  id.language->setCurrentIndex(0);
	  id.language->setFocus();
	}
      else if(name.contains("Monetary Units"))
	{
	  id.monetary_units->setCurrentIndex(0);
	  id.monetary_units->setFocus();
	}
      else if(name.contains("Book Binding Type"))
	{
	  id.binding->setCurrentIndex(0);
	  id.binding->setFocus();
	}
      else if(name.contains("Abstract"))
	{
	  id.description->clear();

	  if(!windowTitle().contains("Search"))
	    id.description->setPlainText("N/A");

	  id.description->viewport()->setPalette(te_orig_pal);
	  id.description->setFocus();
	}
      else if(name.contains("Copies"))
	{
	  id.quantity->setValue(id.quantity->minimum());
	  id.quantity->setFocus();
	}
      else if(name.contains("Location"))
	{
	  id.location->setCurrentIndex(0);
	  id.location->setFocus();
	}
      else if(name.contains("ISBN-13"))
	{
	  id.isbn13->clear();
	  id.isbn13->setPalette(te_orig_pal);
	  id.isbn13->setFocus();
	}
      else if(name.contains("LC Control Number"))
	{
	  id.lcnum->clear();
	  id.lcnum->setPalette(id.url->viewport()->palette());
	  id.lcnum->setFocus();
	}
      else if(name.contains("Call Number"))
	{
	  id.callnum->clear();
	  id.callnum->setPalette(id.url->viewport()->palette());
	  id.callnum->setFocus();
	}
      else if(name.contains("Dewey Class Number"))
	{
	  id.deweynum->clear();
	  id.deweynum->setPalette(id.url->viewport()->palette());
	  id.deweynum->setFocus();
	}
      else if(name.contains("OFFSYSTEM URL"))
	{
	  id.url->clear();
	  id.url->setFocus();
	}
    }
  else
    {
      /*
      ** Reset all.
      */

      id.id->clear();
      id.title->clear();
      id.author->clear();
      id.publisher->clear();
      id.description->clear();

      if(!windowTitle().contains("Search"))
	id.author->setPlainText("N/A");

      if(!windowTitle().contains("Search"))
	id.publisher->setPlainText("N/A");

      if(windowTitle().contains("Search"))
	id.publication_date->setDate(QDate::fromString("01/01/7999",
						       "MM/dd/yyyy"));
      else
	id.publication_date->setDate(QDate::fromString("01/01/2000",
						       "MM/dd/yyyy"));

      id.quantity->setValue(id.quantity->minimum());

      if(!windowTitle().contains("Search"))
	id.description->setPlainText("N/A");

      id.isbn13->clear();
      id.lcnum->clear();
      id.callnum->clear();
      id.deweynum->clear();
      id.location->setCurrentIndex(0);
      id.edition->setCurrentIndex(0);
      id.category->setCurrentIndex(0);
      id.price->setValue(id.price->minimum());
      id.language->setCurrentIndex(0);
      id.monetary_units->setCurrentIndex(0);
      id.binding->setCurrentIndex(0);
      id.front_image->clear();
      id.back_image->clear();
      id.url->clear();
      id.title->setPalette(te_orig_pal);
      id.lcnum->setPalette(id.url->viewport()->palette());
      id.deweynum->setPalette(id.url->viewport()->palette());
      id.callnum->setPalette(id.url->viewport()->palette());
      id.id->setPalette(te_orig_pal);
      id.isbn13->setPalette(te_orig_pal);
      id.edition->setPalette(cb_orig_pal);
      id.category->setPalette(cb_orig_pal);
      id.publication_date->setPalette(dt_orig_pal);
      id.author->viewport()->setPalette(te_orig_pal);
      id.description->viewport()->setPalette(te_orig_pal);
      id.publisher->viewport()->setPalette(te_orig_pal);
      id.id->setFocus();
    }
}

/*
** -- slotConvertISBN10to13() --
*/

void qtbook_book::slotConvertISBN10to13(void)
{
  int i = 0;
  int arr[] = {1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3};
  int check = 0;
  int total = 0;
  QString str = "";
  QString numberstr = "";

  str = "978" + id.id->text().trimmed().left(9);

  for(i = 0; i < (int) (sizeof(arr) / sizeof(int)); i++)
    if(i < str.length())
      total += str[i].digitValue() * arr[i];
    else
      break;

  check = 10 - (total % 10);

  if(check == 10)
    check = 0;

  numberstr.setNum(check);
  id.isbn13->setText(str + numberstr);
}

/*
** -- closeEvent() --
*/

void qtbook_book::closeEvent(QCloseEvent *e)
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

  qmain->removeBook(this);
}

/*
** -- slotCancel() --
*/

void qtbook_book::slotCancel(void)
{
  close();
}

/*
** -- slotPopulateCopiesEditor() --
*/

void qtbook_book::slotPopulateCopiesEditor(void)
{
  copy_editor *copyeditor = NULL;

  if((copyeditor = new copy_editor(qobject_cast<QWidget *>(this),
				   (qtbook_item *) this,
				   false,
				   id.quantity->value(), oid,
				   id.id->text(),
				   id.quantity, font(), "Book")) != NULL)
    copyeditor->populateCopiesEditor();
}

/*
** -- slotShowUsers() --
*/

void qtbook_book::slotShowUsers(void)
{
  int state = 0;
  borrowers_editor *borrowerseditor = NULL;

  if(!id.okButton->isHidden())
    state = qtbook::EDITABLE;
  else
    state = qtbook::VIEW_ONLY;

  if((borrowerseditor = new borrowers_editor
      (qobject_cast<QWidget *>(this), (qtbook_item *) this,
       id.quantity->value(), oid, id.id->text(), font(), "Book",
       state)) != NULL)
    borrowerseditor->showUsers();
}

/*
** -- slotQuery() --
*/

void qtbook_book::slotQuery(void)
{
  int i = 0;
  int j = 0;
  QRegExp reg("[A-Z]{1}");
  QString str = "";
  QString etype = "";
  QString errorstr = "";
  QString searchstr = "";
  QStringList list;
  QStringList tmplist;
  QStringList removeList;

  if(thread != NULL)
    {
      QMessageBox::critical
	(this, "BiblioteQ: User Error", 
	 "A query is already in progress. Please allow it to complete.");
      return;
    }

  if(id.id->text().trimmed().isEmpty() &&
     id.isbn13->text().trimmed().isEmpty())
    {
      QMessageBox::critical
	(this, "BiblioteQ: User Error", 
	 "In order to query the Library of Congress, either the ISBN-10 "
	 "or ISBN-13 must be provided.");
      id.id->setFocus();
      return;
    }

  if((thread = new generic_thread()) != NULL)
    {
      if(!id.id->text().isEmpty())
	searchstr = QString("@attr 1=7 %1").arg(id.id->text());
      else
	searchstr = QString("@attr 1=7 %1").arg(id.isbn13->text());

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

      if((errorstr = thread->getErrorStr()).isEmpty() &&
	 !thread->getLOCResults().isEmpty() && isVisible())
	{
	  if(QMessageBox::question
	     (this, "BiblioteQ: Question",
	      "Replace existing values with those retrieved "
	      "from the Library of Congress?",
	      QMessageBox::Yes | QMessageBox::No,
	      QMessageBox::No) == QMessageBox::Yes)
	    {
	      list = QString(thread->getLOCResults()[0]).split("\n");
	      id.edition->setCurrentIndex(0);
	      id.edition->setStyleSheet("background-color: rgb(162, 205, 90)");

	      for(i = 0; i < list.size(); i++)
		{
		  str = list[i];

		  if(str.startsWith("010"))
		    {
		      str = str.mid(str.indexOf("$a") + 2).trimmed();
		      str = str.remove(" $b").remove(" $z").remove(" $8").
			trimmed();
		      id.lcnum->setText(str);
		      misc_functions::highlightWidget
			(id.lcnum, QColor(162, 205, 90));
		    }
		  else if(str.startsWith("020"))
		    {
		      str = str.mid(str.indexOf("$a") + 2).trimmed();
		      str = str.mid(0, 10);

		      if(id.id->text().isEmpty())
			{
			  id.id->setText(str);
			  misc_functions::highlightWidget
			    (id.id, QColor(162, 205, 90));
			}
		    }
		  else if(str.startsWith("050"))
		    {
		      str = str.mid(str.indexOf("$a") + 2).trimmed();
		      str = str.remove(" $b").remove(" $3").remove(" $6").
			remove(" $8").trimmed();
		      id.callnum->setText(str);
		      misc_functions::highlightWidget
			(id.callnum, QColor(162, 205, 90));
		    }
		  else if(str.startsWith("082"))
		    {
		      str = str.mid(str.indexOf("$a") + 2).trimmed();
		      str = str.remove(" $b").remove(" $2").remove(" $6").
			remove(" $8").trimmed();
		      id.deweynum->setText(str);
		      misc_functions::highlightWidget
			(id.deweynum, QColor(162, 205, 90));
		    }
		  else if(str.startsWith("100") ||
			  str.startsWith("700"))
		    {
		      str = str.mid(str.indexOf("$a") + 2).trimmed();
		      str = str.remove(".").trimmed();
		      removeList.append(" $b");
		      removeList.append(" $c");
		      removeList.append(" $d");
		      removeList.append(" $e");
		      removeList.append(" $f");
		      removeList.append(" $g");
		      removeList.append(" $j");
		      removeList.append(" $k");
		      removeList.append(" $l");
		      removeList.append(" $n");
		      removeList.append(" $p");
		      removeList.append(" $q");
		      removeList.append(" $t");
		      removeList.append(" $u");
		      removeList.append(" $4");
		      removeList.append(" $6");
		      removeList.append(" $8");

		      while(!removeList.isEmpty())
			str = str.remove(removeList.takeFirst()).trimmed();

		      if(str.count(",") > 1)
			str = str.mid(0, str.lastIndexOf(","));

		      if(id.author->toPlainText() == "N/A")
			id.author->clear();

		      /*
		      ** Pure regular expressions would have worked just
		      ** as elegantly.
		      */

		      tmplist = str.split(" ");
		      str = "";

		      for(j = 0; j < tmplist.size(); j++)
			if(reg.exactMatch(tmplist[j]))
			  str += tmplist[j] + ". ";
			else
			  str += tmplist[j] + " ";

		      str = str.trimmed();

		      if(list[i].startsWith("100"))
			id.author->setPlainText(str);
		      else if(!id.author->toPlainText().isEmpty())
			id.author->setPlainText
			  (id.author->toPlainText() + "\n" + str);
		      else
			id.author->setPlainText(str);

		      misc_functions::highlightWidget
			(id.author->viewport(), QColor(162, 205, 90));
		    }
		  else if(str.startsWith("245"))
		    {
		      str = str.mid(str.indexOf("$a") + 2).trimmed();
		      str = str.remove(" $b").trimmed();
		      str = str.remove(" $c").trimmed();
		      str = str.remove(" $d").trimmed();
		      str = str.remove(" $f").trimmed();
		      str = str.remove(" $g").trimmed();
		      str = str.remove(" $h").trimmed();
		      str = str.remove(" $k").trimmed();
		      str = str.remove(" $n").trimmed();
		      str = str.remove(" $p").trimmed();
		      str = str.remove(" $s").trimmed();
		      str = str.remove(" $6").trimmed();
		      str = str.remove(" $8").trimmed();

		      if(str.contains("/"))
			str = str.mid(0, str.lastIndexOf('/')).trimmed();

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
			  str += tmplist[j].mid(0, 1).toUpper() +
			    tmplist[j].mid(1) + " ";

		      tmplist.clear();
		      str = str.trimmed();
		      id.title->setText(str);
		      misc_functions::highlightWidget
			(id.title, QColor(162, 205, 90));
		    }
		  else if(str.startsWith("250"))
		    {
		      str = str.mid(str.indexOf("$a") + 2).trimmed();
		      str = str.mid(0, str.indexOf(" ")).trimmed();
		      str = str.remove(" $b").remove(" $6").remove(" $8").
			trimmed();

		      if(id.edition->findText(str) > -1)
			id.edition->setCurrentIndex
			  (id.edition->findText(str));
		      else
			{
			  str = str.remove("d").remove("h").remove("n").
			    remove("r").remove("s").remove("t").trimmed();

			  if(str.toInt() >= 1)
			    id.edition->setCurrentIndex(str.toInt() - 1);
			}
		    }
		  else if(str.startsWith("260"))
		    {
		      if(str.mid(str.indexOf("$c") + 2, 4).contains("c"))
			id.publication_date->setDate
			  (QDate::fromString
			   ("01/01/" +
			    str.mid(str.indexOf("$c") + 4, 4),
			    "MM/dd/yyyy"));
		      else
			id.publication_date->setDate
			  (QDate::fromString
			   ("01/01/" +
			    str.mid(str.indexOf("$c") + 3, 4),
			    "MM/dd/yyyy"));

		      id.publication_date->setStyleSheet
			("background-color: rgb(162, 205, 90)");
		      str = str.mid(str.indexOf("$b") + 2).trimmed();
		      str = str.mid(0, str.indexOf("$c")).trimmed();

		      if(str.endsWith(","))
			str = str.mid(0, str.length() - 1);

		      id.publisher->setPlainText(str);
		      misc_functions::highlightWidget
			(id.publisher->viewport(), QColor(162, 205, 90));
		    }
		  else if(str.startsWith("300"))
		    {
		      str = str.mid(str.indexOf("$a") + 2).trimmed();
		      str = str.remove(" $b").trimmed();
		      str = str.remove(" $c").trimmed();
		      str = str.remove(" $e").trimmed();
		      str = str.remove(" $f").trimmed();
		      str = str.remove(" $g").trimmed();
		      str = str.remove(" $3").trimmed();
		      str = str.remove(" $6").trimmed();
		      str = str.remove(" $8").trimmed();
		      id.description->setPlainText(str);
		      misc_functions::highlightWidget
			(id.description->viewport(), QColor(162, 205, 90));
		    }
		  else if(str.startsWith("650"))
		    {
		      str = str.trimmed().toLower();

		      for(j = 0; j < id.category->count(); j++)
			if(str.contains(id.category->itemText(j).toLower()))
			  {
			    id.category->setCurrentIndex(j);
			    id.category->setStyleSheet
			      ("background-color: rgb(162, 205, 90)");
			    break;
			  }
		    }
		}

	      foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
		textfield->setCursorPosition(0);
	    }
	}
      else if(errorstr.isEmpty() && thread->getLOCResults().isEmpty() &&
	      isVisible())
	QMessageBox::critical
	  (this, "BiblioteQ: Z39.50 Query Error",
	   "A Library of Congress entry may not yet exist for " +
	   id.id->text() + ".");
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

void qtbook_book::slotPrint(void)
{
  html = "";
  html += "<b>ISBN-10:</b> " + id.id->text() + "<br>";
  html += "<b>ISBN-13:</b> " + id.isbn13->text() + "<br>";
  html += "<b>Edition:</b> " + id.edition->currentText() + "<br>";
  html += "<b>Author(s):</b> " + id.author->toPlainText() + "<br>";
  html += "<b>Book Binding Type:</b> " + id.binding->currentText() + "<br>";
  html += "<b>LC Control Number:</b> " + id.lcnum->text() + "<br>";
  html += "<b>Call Number:</b> " + id.callnum->text() + "<br>";
  html += "<b>Dewey Class Number:</b> " + id.deweynum->text() + "<br>";

  /*
  ** General information.
  */

  html += "<b>Title:</b> " + id.title->text() + "<br>";
  html += "<b>Publication Date:</b> " + id.publication_date->date().
    toString("MM/dd/yyyy") + "<br>";
  html += "<b>Publisher:</b> " + id.publisher->toPlainText() + "<br>";
  html += "<b>Category:</b> " + id.category->currentText() + "<br>";
  html += "<b>Price:</b> " + id.price->text() + "<br>";
  html += "<b>Language:</b> " + id.language->currentText() + "<br>";
  html += "<b>Monetary Units:</b> " + id.monetary_units->currentText() +
    "<br>";
  html += "<b>Copies:</b> " + id.quantity->text() + "<br>";
  html += "<b>Location:</b> " + id.location->currentText() + "<br>";
  html += "<b>Abstract:</b> " + id.description->toPlainText() + "<br>";
  html += "<b>OFFSYSTEM URL:</b> " + id.url->toPlainText();
  print(this);
}

/*
** -- isBusy() --
*/

bool qtbook_book::isBusy(void)
{
  if(thread != NULL)
    return true;
  else
    return false;
}

/*
** -- slotSelectImage() --
*/

void qtbook_book::slotSelectImage(void)
{
  QFileDialog dialog(this);
  QPushButton *button = qobject_cast<QPushButton *> (sender());

  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setFilter("Image Files (*.bmp *.jpg *.jpeg *.png)");

  if(button == id.frontButton)
    dialog.setWindowTitle("Front Cover Image Selection");
  else
    dialog.setWindowTitle("Back Cover Image Selection");

  dialog.exec();

  if(dialog.result() == QDialog::Accepted)
    if(button == id.frontButton)
      {
	id.front_image->clear();
	id.front_image->image = QImage(dialog.selectedFiles().at(0));
	id.front_image->imageFormat = dialog.selectedFiles().at(0).mid
	  (dialog.selectedFiles().at(0).lastIndexOf(".") + 1).toUpper();
	id.front_image->scene()->addPixmap
	  (QPixmap().fromImage(id.front_image->image));
	id.front_image->scene()->items().at(0)->setFlags
	  (QGraphicsItem::ItemIsSelectable);
      }
    else
      {
	id.back_image->clear();
	id.back_image->image = QImage(dialog.selectedFiles().at(0));
	id.back_image->imageFormat = dialog.selectedFiles().at(0).mid
	  (dialog.selectedFiles().at(0).lastIndexOf(".") + 1).toUpper();
	id.back_image->scene()->addPixmap
	  (QPixmap().fromImage(id.back_image->image));
	id.back_image->scene()->items().at(0)->setFlags
	  (QGraphicsItem::ItemIsSelectable);
      }
}

/*
** -- slotGenerateISBN() --
*/

void qtbook_book::slotGenerateISBN(void)
{
  id.id->setText(QString::number(QDateTime::currentDateTime().toTime_t()));
}
