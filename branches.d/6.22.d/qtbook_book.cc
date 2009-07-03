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
			 const QStringList &languages,
			 const QStringList &monetary_units,
			 const QStringList &locations, const QString &oidArg,
			 const int rowArg):
  QMainWindow()
{
  QMenu *menu = 0;
  QRegExp rx1("[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9X]");
  QRegExp rx2("[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]"
	      "[0-9][0-9][0-9]");
  QValidator *validator1 = 0;
  QValidator *validator2 = 0;
  QGraphicsScene *scene1 = 0;
  QGraphicsScene *scene2 = 0;

  if((menu = new(std::nothrow) QMenu()) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((validator1 = new(std::nothrow) QRegExpValidator(rx1, this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((validator2 = new(std::nothrow) QRegExpValidator(rx2, this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene1 = new(std::nothrow) QGraphicsScene()) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene2 = new(std::nothrow) QGraphicsScene()) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((http1 = new(std::nothrow) QHttp(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((http2 = new(std::nothrow) QHttp(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((imgbuffer1 = new(std::nothrow) QBuffer(&imgbytes1)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((imgbuffer2 = new(std::nothrow) QBuffer(&imgbytes2)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((httpProgress = new(std::nothrow) qtbook_item_working_dialog(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  requestid1 = requestid2 = 0;
  parentWid = parentArg;
  oid = oidArg;
  row = rowArg;
  httpRequestAborted = false;
  oldq = misc_functions::getColumnString
    (qmain->getUI().table, row, tr("Quantity")).toInt();
  http1->setHost(qmain->getAmazonHash()["front_cover_host"]);
  http2->setHost(qmain->getAmazonHash()["back_cover_host"]);
  id.setupUi(this);
  updateFont(qapp->font(), static_cast<QWidget *> (this));
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
  connect(menu->addAction(tr("Reset &Front Cover Image")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Back Cover Image")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &ISBN-10")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &ISBN-13")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Edition")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Author(s)")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Book Binding Type")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &LC Control Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Call Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Dewey Class Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Title")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Publication Date")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Publisher")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Place of Publication")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Categories")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Price")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Language")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Monetary Units")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Copies")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Location")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Abstract")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &OFFSYSTEM URL")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(id.frontButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(id.backButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(id.generate, SIGNAL(clicked(void)), this,
	  SLOT(slotGenerateISBN(void)));
  connect(id.dwnldFront, SIGNAL(clicked(void)), this,
	  SLOT(slotDownloadImage(void)));
  connect(id.dwnldBack, SIGNAL(clicked(void)), this,
	  SLOT(slotDownloadImage(void)));
  connect(http1, SIGNAL(requestFinished(int, bool)),
	  this, SLOT(slotHttpRequestFinished(int, bool)));
  connect(http2, SIGNAL(requestFinished(int, bool)),
	  this, SLOT(slotHttpRequestFinished(int, bool)));
  connect(http1, SIGNAL(dataReadProgress(int, int)),
	  this, SLOT(slotUpdateDataReadProgress(int, int)));
  connect(http2, SIGNAL(dataReadProgress(int, int)),
	  this, SLOT(slotUpdateDataReadProgress(int, int)));
  connect(httpProgress, SIGNAL(canceled(void)), this,
	  SLOT(slotCancelImageDownload(void)));
  id.id->setValidator(validator1);
  id.isbn13->setValidator(validator2);
  id.resetButton->setMenu(menu);
  id.language->addItems(languages);
  id.monetary_units->addItems(monetary_units);
  id.location->addItems(locations);
  id.front_image->setScene(scene1);
  id.back_image->setScene(scene2);
  httpProgress->setModal(true);

  if(id.language->count() == 0)
    id.language->addItem(tr("UNKNOWN"));

  if(id.monetary_units->count() == 0)
    id.monetary_units->addItem(tr("UNKNOWN"));

  if(id.location->count() == 0)
    id.location->addItem(tr("UNKNOWN"));

  /*
  ** Save some palettes and style sheets.
  */

  cb_orig_ss = id.edition->styleSheet();
  dt_orig_ss = id.publication_date->styleSheet();
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
  (void) http1->close();
  (void) http2->close();
  imgbuffer1->close();
  imgbuffer2->close();
  imgbuffer1->deleteLater();
  imgbuffer2->deleteLater();
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
  QTableWidgetItem *column = 0;

  if(engWindowTitle.contains("Create") ||
     engWindowTitle.contains("Modify"))
    {
      if(engWindowTitle.contains("Modify") && row > -1)
	{
	  newq = id.quantity->value();
	  qapp->setOverrideCursor(Qt::WaitCursor);
	  maxcopynumber = misc_functions::getMaxCopyNumber
	    (qmain->getDB(), oid, "Book", errorstr);

	  if(maxcopynumber < 0)
	    {
	      qapp->restoreOverrideCursor();
	      qmain->addError
		(QString(tr("Database Error")),
		 QString(tr("Unable to determine the maximum copy number of "
			    "the item.")),
		 errorstr, __FILE__, __LINE__);
	      QMessageBox::critical
		(this, tr("BiblioteQ: Database Error"),
		 tr("Unable to determine the maximum copy number of "
		    "the item."));
	      return;
	    }

	  qapp->restoreOverrideCursor();

	  if(newq < maxcopynumber)
	    {
	      QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				    tr("It appears that you are attempting to "
				       "decrease the "
				       "number of copies while there are "
				       "copies "
				       "that have been reserved."));
	      id.quantity->setValue(oldq);
	      return;
	    }
	  else if(newq > oldq)
	    if(QMessageBox::question
	       (this, tr("BiblioteQ: Question"),
		tr("Would you like to modify copy information?"),
		QMessageBox::Yes | QMessageBox::No,
		QMessageBox::No) == QMessageBox::Yes)
	      slotPopulateCopiesEditor();
	}

      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!qmain->getDB().transaction())
	{
	  qapp->restoreOverrideCursor();
	  qmain->addError
	    (QString(tr("Database Error")),
	     QString(tr("Unable to create a database transaction.")),
	     qmain->getDB().lastError().text(), __FILE__, __LINE__);
	  QMessageBox::critical
	    (this, tr("BiblioteQ: Database Error"),
	     tr("Unable to create a database transaction."));
	  return;
	}

      qapp->restoreOverrideCursor();
      str = id.id->text().trimmed();
      id.id->setText(str);
      slotConvertISBN10to13();
      str = id.isbn13->text().trimmed();
      id.isbn13->setText(str);

      if(id.id->text().length() != 10 ||
	 id.isbn13->text().length() != 13)
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete both the "
				   "ISBN-10 and ISBN-13 fields."));

	  if(id.id->text().length() != 10)
	    id.id->setFocus();
	  else
	    id.isbn13->setFocus();

	  goto db_rollback;
	}

      str = id.author->toPlainText().trimmed();
      id.author->setPlainText(str);

      if(id.author->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Author(s) field."));
	  id.author->setFocus();
	  goto db_rollback;
	}

      str = id.title->text().trimmed();
      id.title->setText(str);

      if(id.title->text().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Title field."));
	  id.title->setFocus();
	  goto db_rollback;
	}

      str = id.publisher->toPlainText().trimmed();
      id.publisher->setPlainText(str);

      if(id.publisher->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Publisher field."));
	  id.publisher->setFocus();
	  goto db_rollback;
	}

      str = id.place->toPlainText().trimmed();
      id.place->setPlainText(str);

      if(id.place->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Place of Publication "
				   "field."));
	  id.place->setFocus();
	  goto db_rollback;
	}

      str = id.category->toPlainText().trimmed();
      id.category->setPlainText(str);

      if(id.category->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Categories field."));
	  id.category->setFocus();
	  goto db_rollback;
	}

      str = id.description->toPlainText().trimmed();
      id.description->setPlainText(str);

      if(id.description->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Abstract field."));
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

      if(engWindowTitle.contains("Modify"))
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
			      "offsystem_url = ?, "
			      "place = ? "
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
			      "offsystem_url, place) "
			      "VALUES (?, ?, "
			      "?, ?, ?, ?, "
			      "?, ?, ?, "
			      "?, ?, "
			      "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"));
      else
	query.prepare(QString("INSERT INTO book (id, title, "
			      "edition, author, pdate, publisher, "
			      "category, price, description, language, "
			      "monetary_units, quantity, "
			      "binding_type, location, "
			      "isbn13, lccontrolnumber, callnumber, "
			      "deweynumber, front_cover, "
			      "back_cover, "
			      "offsystem_url, place, myoid) "
			      "VALUES (?, ?, "
			      "?, ?, ?, ?, "
			      "?, ?, ?, "
			      "?, ?, "
			      "?, ?, ?, ?, ?, ?, ?, ?, ?, "
			      "?, ?, ?)"));

      query.bindValue(0, id.id->text());
      query.bindValue(1, id.title->text().trimmed());
      query.bindValue(2, id.edition->currentText().trimmed());
      query.bindValue(3, id.author->toPlainText().trimmed());
      query.bindValue(4, id.publication_date->date().toString("MM/dd/yyyy"));
      query.bindValue(5, id.publisher->toPlainText().trimmed());
      query.bindValue(6, id.category->toPlainText().trimmed());
      query.bindValue(7, id.price->value());
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

      query.bindValue(21, id.place->toPlainText().trimmed());

      if(engWindowTitle.contains("Modify"))
	query.bindValue(22, oid);
      else if(qmain->getDB().driverName() == "QSQLITE")
	query.bindValue(22, id.id->text().replace("X", "10"));

      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!query.exec())
	{
	  qapp->restoreOverrideCursor();
	  qmain->addError(QString(tr("Database Error")),
			  QString(tr("Unable to create or update the entry.")),
			  query.lastError().text(), __FILE__, __LINE__);
	  QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
				tr("Unable to create or update the entry. "
				   "Please verify that "
				   "the entry does not already exist."));
	  goto db_rollback;
	}
      else
	{
	  /*
	  ** Remove copies if the quantity has been decreased.
	  */

	  if(engWindowTitle.contains("Modify"))
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
		  qmain->addError
		    (QString(tr("Database Error")),
		     QString(tr("Unable to purge unnecessary copy "
				"data.")),
		     query.lastError().text(), __FILE__,
		     __LINE__);
		  QMessageBox::critical(this,
					tr("BiblioteQ: Database Error"),
					tr("Unable to purge unnecessary "
					   "copy data."));
		  goto db_rollback;
		}

	      if(!qmain->getDB().commit())
		{
		  qapp->restoreOverrideCursor();
		  qmain->addError
		    (QString(tr("Database Error")),
		     QString(tr("Unable to commit the current database "
				"transaction.")),
		     qmain->getDB().lastError().text(), __FILE__,
		     __LINE__);
		  QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
					tr("Unable to commit the current "
					   "database transaction."));
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
		    (QString(tr("Database Error")),
		     QString(tr("Unable to create initial copies.")),
		     errorstr, __FILE__, __LINE__);
		  QMessageBox::critical
		    (this,
		     tr("BiblioteQ: Database Error"),
		     tr("Unable to create initial copies."));
		  goto db_rollback;
		}

	      if(!qmain->getDB().commit())
		{
		  qapp->restoreOverrideCursor();
		  qmain->addError
		    (QString(tr("Database Error")),
		     QString(tr("Unable to commit the current database "
				"transaction.")),
		     qmain->getDB().lastError().text(), __FILE__,
		     __LINE__);
		  QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
					tr("Unable to commit the current "
					   "database transaction."));
		  goto db_rollback;
		}
	    }

	  id.id->setPalette(te_orig_pal);
	  id.isbn13->setPalette(te_orig_pal);
	  id.edition->setStyleSheet(cb_orig_ss);
	  id.category->viewport()->setPalette(te_orig_pal);
	  id.publication_date->setStyleSheet(dt_orig_ss);
	  id.author->viewport()->setPalette(te_orig_pal);
	  id.title->setPalette(te_orig_pal);
	  id.description->viewport()->setPalette(te_orig_pal);
	  id.publisher->viewport()->setPalette(te_orig_pal);
	  id.place->viewport()->setPalette(te_orig_pal);
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
	  id.category->setMultipleLinks("book_search", "category",
					id.category->toPlainText());
	  id.publisher->setText
	    (QString("<a href=\"book_search?publisher?%1\">" +
		     id.publisher->toPlainText() + "</a>").arg
	     (id.publisher->toPlainText()));
	  id.place->setMultipleLinks("book_search", "place",
				     id.place->toPlainText());

	  if(!id.url->toPlainText().isEmpty())
	    id.url->setText(QString("<a href=\"%1\">%1</a>").arg
			    (id.url->toPlainText()));

	  qapp->restoreOverrideCursor();

	  if(engWindowTitle.contains("Modify"))
	    {
	      str = QString(tr("BiblioteQ: Modify Book Entry (")) +
		id.id->text() + tr(")");
	      engWindowTitle = "Modify";
	      setWindowTitle(str);

	      if((qmain->getUI().typefilter->currentText() == tr("All") ||
		  qmain->getUI().typefilter->currentText() ==
		  tr("All Overdue") ||
		  qmain->getUI().typefilter->currentText() ==
		  tr("All Requested") ||
		  qmain->getUI().typefilter->currentText() ==
		  tr("All Reserved") ||
		  qmain->getUI().typefilter->currentText() == tr("Books")) &&
		 oid == misc_functions::getColumnString(qmain->getUI().table,
							row, "MYOID"))
		{
		  for(i = 0; i < qmain->getUI().table->columnCount(); i++)
		    {
		      column = qmain->getUI().table->horizontalHeaderItem(i);

		      if(column == 0)
			continue;

		      if(column->text() == tr("ISBN-10") ||
			 column->text() == tr("ID Number"))
			qmain->getUI().table->item(row, i)->setText
			  (id.id->text());
		      else if(column->text() == tr("Title"))
			qmain->getUI().table->item(row, i)->setText
			  (id.title->text());
		      else if(column->text() == tr("Edition"))
			qmain->getUI().table->item(row, i)->setText
			  (id.edition->currentText().trimmed());
		      else if(column->text() == tr("Author(s)"))
			qmain->getUI().table->item(row, i)->setText
			  (id.author->toPlainText());
		      else if(column->text() == tr("Publication Date"))
			qmain->getUI().table->item(row, i)->setText
			  (id.publication_date->date().toString("MM/dd/yyyy"));
		      else if(column->text() == tr("Publisher"))
			qmain->getUI().table->item(row, i)->setText
			  (id.publisher->toPlainText());
		      else if(column->text() == tr("Place of Publication"))
			qmain->getUI().table->item(row, i)->setText
			  (id.place->toPlainText());
		      else if(column->text() == tr("Categories"))
			qmain->getUI().table->item(row, i)->setText
			  (id.category->toPlainText());
		      else if(column->text() == tr("Price"))
			qmain->getUI().table->item(row, i)->setText
			  (id.price->text());
		      else if(column->text() == tr("Language"))
			qmain->getUI().table->item(row, i)->setText
			  (id.language->currentText().trimmed());
		      else if(column->text() == tr("Monetary Units"))
			qmain->getUI().table->item(row, i)->setText
			  (id.monetary_units->currentText().trimmed());
		      else if(column->text() == tr("Quantity"))
			qmain->getUI().table->item(row, i)->setText
			  (id.quantity->text());
		      else if(column->text() == tr("Book Binding Type"))
			qmain->getUI().table->item(row, i)->setText
			  (id.binding->currentText().trimmed());
		      else if(column->text() == tr("Location"))
			qmain->getUI().table->item(row, i)->setText
			  (id.location->currentText().trimmed());
		      else if(column->text() == tr("ISBN-13"))
			qmain->getUI().table->item(row, i)->setText
			  (id.isbn13->text());
		      else if(column->text() == tr("LC Control Number"))
			qmain->getUI().table->item(row, i)->setText
			  (id.lcnum->text());
		      else if(column->text() == tr("Call Number"))
			qmain->getUI().table->item(row, i)->setText
			  (id.callnum->text());
		      else if(column->text() == tr("Dewey Class Number"))
			qmain->getUI().table->item(row, i)->setText
			  (id.deweynum->text());
		      else if(column->text() == tr("Availability"))
			{
			  qmain->getUI().table->item(row, i)->setText
			    (misc_functions::getAvailability
			     (oid, qmain->getDB(), "Book",
			      errorstr));

			  if(!errorstr.isEmpty())
			    qmain->addError
			      (QString(tr("Database Error")),
			       QString(tr("Retrieving availability.")),
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
		  qmain->addError(QString(tr("Database Error")),
				  QString(tr("Unable to retrieve the book's "
					     "OID.")),
				  errorstr, __FILE__, __LINE__);
		  QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
					tr("Unable to retrieve the book's "
					   "OID."));
		}
	      else
		qmain->replaceBook("insert", this);

	      updateWindow(qtbook::EDITABLE);

	      if(qmain->getUI().actionAutoPopulateOnCreation->isChecked())
		(void) qmain->populateTable
		  (qtbook::POPULATE_ALL, tr("Books"), QString(""));

	      raise();
	    }

	  storeData(this);
	}

      return;

    db_rollback:

      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!qmain->getDB().rollback())
	qmain->addError
	  (QString(tr("Database Error")), QString(tr("Rollback failure.")),
	   qmain->getDB().lastError().text(), __FILE__, __LINE__);

      qapp->restoreOverrideCursor();
    }
  else
    {
      searchstr = "SELECT book.title, "
	"book.author, "
	"book.publisher, book.pdate, book.place, book.edition, "
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

      if(id.edition->currentText() != tr("Any"))
	searchstr.append("edition = '" + id.edition->currentText() +
			 "' AND ");

      searchstr.append("LOWER(author) LIKE '%" +
		       myqstring::escape(id.author->toPlainText().
					 toLower()) + "%' AND ");

      if(id.publication_date->date().toString("MM/yyyy") != "01/7999")
	searchstr.append("SUBSTR(pdate, 1, 3) || SUBSTR(pdate, 7) = '" +
			 id.publication_date->date().toString
			 ("MM/yyyy") +
			 "' AND ");

      searchstr.append("LOWER(publisher) LIKE '%" +
		       myqstring::escape
		       (id.publisher->toPlainText().toLower()) +
		       "%' AND ");
      searchstr.append("LOWER(place) LIKE '%" +
		       myqstring::escape
		       (id.place->toPlainText().toLower()) +
		       "%' AND ");
      searchstr.append("LOWER(category) LIKE '%" +
		       myqstring::escape
		       (id.category->toPlainText().toLower()) +
		       "%' AND ");

      if(id.price->value() > -0.01)
	{
	  searchstr.append("price = ");
	  searchstr.append(QString::number(id.price->value()));
	  searchstr.append(" AND ");
	}

      if(id.language->currentText() != tr("Any"))
	searchstr.append("language = '" +
			 myqstring::escape(id.language->currentText()) +
			 "' AND ");

      if(id.monetary_units->currentText() != tr("Any"))
	searchstr.append("monetary_units = '" +
			 myqstring::escape
			 (id.monetary_units->currentText()) +
			 "' AND ");

      if(id.binding->currentText() != tr("Any"))
	searchstr.append("binding_type = '" +
			 myqstring::escape(id.binding->currentText()) +
			 "' AND ");

      searchstr.append("LOWER(description) LIKE '%" +
		       myqstring::escape
		       (id.description->toPlainText().toLower()) +
		       "%' ");

      if(id.quantity->value() != 0)
	searchstr.append("AND quantity = " + id.quantity->text() + " ");

      if(id.location->currentText() != tr("Any"))
	searchstr.append("AND location = '" +
			 myqstring::escape
			 (id.location->currentText()) + "' ");

      if(!id.url->toPlainText().isEmpty())
	searchstr.append("AND LOWER(COALESCE(offsystem_url, '')) LIKE '%" +
			 myqstring::escape
			 (id.url->toPlainText().toLower()) + "%' ");

      hide();

      /*
      ** Search the database.
      */

      (void) qmain->populateTable
	(qtbook::POPULATE_SEARCH, tr("Books"), searchstr);
      slotCancel();
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
  id.category->clear();
  id.isbn13->clear();
  id.author->clear();
  id.lcnum->clear();
  id.callnum->clear();
  id.deweynum->clear();
  id.title->clear();
  id.publisher->clear();
  id.place->clear();
  id.description->clear();
  id.copiesButton->setVisible(false);
  id.showUserButton->setVisible(false);
  id.queryButton->setVisible(false);
  id.okButton->setText(tr("&Search"));
  id.publication_date->setDate(QDate::fromString("01/7999",
						 "MM/yyyy"));
  id.publication_date->setDisplayFormat("MM/yyyy");
  id.price->setMinimum(-0.01);
  id.price->setValue(-0.01);
  id.quantity->setMinimum(0);
  id.quantity->setValue(0);

  /*
  ** Add "any".
  */

  if(id.edition->findText(tr("Any")) == -1)
    id.edition->insertItem(0, tr("Any"));

  if(id.language->findText(tr("Any")) == -1)
    id.language->insertItem(0, tr("Any"));

  if(id.monetary_units->findText(tr("Any")) == -1)
    id.monetary_units->insertItem(0, tr("Any"));

  if(id.binding->findText(tr("Any")) == -1)
    id.binding->insertItem(0, tr("Any"));

  if(id.location->findText(tr("Any")) == -1)
    id.location->insertItem(0, tr("Any"));

  id.location->setCurrentIndex(0);
  id.edition->setCurrentIndex(0);
  id.language->setCurrentIndex(0);
  id.monetary_units->setCurrentIndex(0);
  id.binding->setCurrentIndex(0);
  id.url->clear();

  if(field.isEmpty() && value.isEmpty())
    {
      QList<QAction *> actions = id.resetButton->menu()->actions();

      actions[0]->setVisible(false);
      actions[1]->setVisible(false);
      actions.clear();
      setWindowTitle(tr("BiblioteQ: Database Book Search"));
      engWindowTitle = "Search";
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
      else if(field == "category")
	id.category->setPlainText(value);
      else if(field == "place")
	id.place->setPlainText(value);

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
      id.dwnldFront->setVisible(true);
      id.dwnldBack->setVisible(true);
      id.generate->setVisible(true);
      id.isbn10to13->setVisible(true);
      str = QString(tr("BiblioteQ: Modify Book Entry (")) +
	id.id->text() + tr(")");
      engWindowTitle = "Modify";
    }
  else
    {
      id.copiesButton->setVisible(false);
      id.showUserButton->setEnabled(true);
      id.okButton->setVisible(false);
      id.queryButton->setVisible(false);
      id.resetButton->setVisible(false);
      id.frontButton->setVisible(false);
      id.backButton->setVisible(false);
      id.dwnldFront->setVisible(false);
      id.dwnldBack->setVisible(false);
      id.generate->setVisible(false);
      id.isbn10to13->setVisible(false);
      str = QString(tr("BiblioteQ: View Book Details (")) +
	id.id->text() + tr(")");
      engWindowTitle = "View";
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
      setWindowTitle(tr("BiblioteQ: Modify Book Entry"));
      engWindowTitle = "Modify";
      id.copiesButton->setEnabled(true);
      id.showUserButton->setEnabled(true);
      id.okButton->setVisible(true);
      id.queryButton->setVisible(true);
      id.resetButton->setVisible(true);
      id.frontButton->setVisible(true);
      id.backButton->setVisible(true);
      id.dwnldFront->setVisible(true);
      id.dwnldBack->setVisible(true);
      id.generate->setVisible(true);
      id.isbn10to13->setVisible(true);
      misc_functions::highlightWidget
	(id.id, QColor(255, 248, 220));
      misc_functions::highlightWidget
	(id.isbn13, QColor(255, 248, 220));
      misc_functions::highlightWidget
	(id.title, QColor(255, 248, 220));
      misc_functions::highlightWidget
	(id.publisher->viewport(), QColor(255, 248, 220));
      misc_functions::highlightWidget
	(id.place->viewport(), QColor(255, 248, 220));
      misc_functions::highlightWidget
	(id.author->viewport(), QColor(255, 248, 220));
      misc_functions::highlightWidget
	(id.description->viewport(), QColor(255, 248, 220));
      misc_functions::highlightWidget
	(id.category->viewport(), QColor(255, 248, 220));
      te_orig_pal = id.id->palette();
    }
  else
    {
      setWindowTitle(tr("BiblioteQ: View Book Details"));
      engWindowTitle = "View";
      id.copiesButton->setVisible(false);
      id.showUserButton->setEnabled(true);
      id.okButton->setVisible(false);
      id.queryButton->setVisible(false);
      id.resetButton->setVisible(false);
      id.frontButton->setVisible(false);
      id.backButton->setVisible(false);
      id.dwnldFront->setVisible(false);
      id.dwnldBack->setVisible(false);
      id.generate->setVisible(false);
      id.isbn10to13->setVisible(false);

      QList<QAction *> actions = id.resetButton->menu()->actions();

      actions[0]->setVisible(false);
      actions[1]->setVisible(false);
      actions.clear();
    }

  id.quantity->setMinimum(1);
  id.price->setMinimum(0.00);
  id.okButton->setText(tr("&Save"));
  str = oid;
  searchstr = "SELECT title, "
    "author, "
    "publisher, pdate, place, edition, "
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
      qmain->addError
	(QString(tr("Database Error")),
	 QString(tr("Unable to retrieve the selected book's data.")),
	 query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical
	(this, tr("BiblioteQ: Database Error"),
	 tr("Unable to retrieve the selected book's data."));
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
	    id.title->setText(var.toString());
	  else if(fieldname == "author")
	    id.author->setMultipleLinks("book_search", "author",
					var.toString());
	  else if(fieldname == "publisher")
	    id.publisher->setText
	      (QString
	       ("<a href=\"book_search?publisher?%1\">" +
		var.toString() + "</a>").arg(var.toString()));
	  else if(fieldname == "place")
	    id.place->setMultipleLinks("book_search", "place",
				       var.toString());
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
	    id.category->setMultipleLinks("book_search", "category",
					  var.toString());
	  else if(fieldname == "language")
	    {
	      if(id.language->findText(var.toString()) > -1)
		id.language->setCurrentIndex
		  (id.language->findText(var.toString()));
	      else
		id.language->setCurrentIndex
		  (id.language->findText(tr("UNKNOWN")));
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
		  (id.monetary_units->findText(tr("UNKNOWN")));
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
		  (id.location->findText(tr("UNKNOWN")));
	    }
	  else if(fieldname == "id")
	    {
	      if(state == qtbook::EDITABLE)
		{
		  str = QString(tr("BiblioteQ: Modify Book Entry (")) +
		    var.toString() + tr(")");
		  engWindowTitle = "Modify";
		}
	      else
		{
		  str = QString(tr("BiblioteQ: View Book Details (")) +
		    tr(")") + var.toString();
		  engWindowTitle = "View";
		}

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
		id.front_image->loadFromData(var.toByteArray());
	    }
	  else if(fieldname == "back_cover")
	    {
	      if(!query.record().field(i).isNull())
		id.back_image->loadFromData(var.toByteArray());
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
  id.category->setPlainText("N/A");
  id.author->setPlainText("N/A");
  id.lcnum->clear();
  id.callnum->clear();
  id.deweynum->clear();
  id.title->clear();
  id.publisher->setPlainText("N/A");
  id.place->setPlainText("N/A");
  id.description->setPlainText("N/A");
  id.copiesButton->setEnabled(false);
  id.queryButton->setVisible(true);
  id.okButton->setText(tr("&Save"));
  id.publication_date->setDate(QDate::fromString("01/01/2000",
						 "MM/dd/yyyy"));
  id.price->setMinimum(0.00);
  id.price->setValue(0.00);
  id.quantity->setMinimum(1);
  id.quantity->setValue(1);
  id.showUserButton->setEnabled(false);
  id.location->setCurrentIndex(0);
  id.edition->setCurrentIndex(0);
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
    (id.place->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (id.author->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (id.description->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (id.category->viewport(), QColor(255, 248, 220));
  te_orig_pal = id.id->palette();
  setWindowTitle(tr("BiblioteQ: Create Book Entry"));
  engWindowTitle = "Create";
  id.id->setFocus();
  storeData(this);
  misc_functions::center(this, parentWid);
  show();
}

/*
** -- slotReset() --
*/

void qtbook_book::slotReset(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(action != 0)
    {
      QList<QAction *> actions = id.resetButton->menu()->actions();

      if(action == actions[0])
	id.front_image->clear();
      else if(action == actions[1])
	id.back_image->clear();
      else if(action == actions[2])
	{
	  id.id->clear();
	  id.id->setPalette(te_orig_pal);
	  id.id->setFocus();
	}
      else if(action == actions[10])
	{
	  id.title->clear();
	  id.title->setPalette(te_orig_pal);
	  id.title->setFocus();
	}
      else if(action == actions[4])
	{
	  id.edition->setCurrentIndex(0);
	  id.edition->setStyleSheet(cb_orig_ss);
	  id.edition->setFocus();
	}
      else if(action == actions[5])
	{
	  if(!engWindowTitle.contains("Search"))
	    id.author->setPlainText("N/A");
	  else
	    id.author->clear();

	  id.author->viewport()->setPalette(te_orig_pal);
	  id.author->setFocus();
	}
      else if(action == actions[11])
	{
	  if(engWindowTitle.contains("Search"))
	    id.publication_date->setDate
	      (QDate::fromString("01/7999", "MM/yyyy"));
	  else
	    id.publication_date->setDate
	      (QDate::fromString("01/01/2000", "MM/dd/yyyy"));

	  id.publication_date->setStyleSheet(dt_orig_ss);
	  id.publication_date->setFocus();
	}
      else if(action == actions[12])
	{
	  if(!engWindowTitle.contains("Search"))
	    id.publisher->setPlainText("N/A");
	  else
	    id.publisher->clear();

	  id.publisher->viewport()->setPalette(te_orig_pal);
	  id.publisher->setFocus();
	}
      else if(action == actions[13])
	{
	  if(!engWindowTitle.contains("Search"))
	    id.place->setPlainText("N/A");
	  else
	    id.place->clear();

	  id.place->viewport()->setPalette(te_orig_pal);
	  id.place->setFocus();
	}
      else if(action == actions[14])
	{
	  if(!engWindowTitle.contains("Search"))
	    id.category->setPlainText("N/A");
	  else
	    id.category->clear();

	  id.category->viewport()->setPalette(te_orig_pal);
	  id.category->setFocus();
	}
      else if(action == actions[15])
	{
	  id.price->setValue(id.price->minimum());
	  id.price->setFocus();
	}
      else if(action == actions[16])
	{
	  id.language->setCurrentIndex(0);
	  id.language->setFocus();
	}
      else if(action == actions[17])
	{
	  id.monetary_units->setCurrentIndex(0);
	  id.monetary_units->setFocus();
	}
      else if(action == actions[6])
	{
	  id.binding->setCurrentIndex(0);
	  id.binding->setFocus();
	}
      else if(action == actions[20])
	{
	  if(!engWindowTitle.contains("Search"))
	    id.description->setPlainText("N/A");
	  else
	    id.description->clear();

	  id.description->viewport()->setPalette(te_orig_pal);
	  id.description->setFocus();
	}
      else if(action == actions[18])
	{
	  id.quantity->setValue(id.quantity->minimum());
	  id.quantity->setFocus();
	}
      else if(action == actions[19])
	{
	  id.location->setCurrentIndex(0);
	  id.location->setFocus();
	}
      else if(action == actions[3])
	{
	  id.isbn13->clear();
	  id.isbn13->setPalette(te_orig_pal);
	  id.isbn13->setFocus();
	}
      else if(action == actions[7])
	{
	  id.lcnum->clear();
	  id.lcnum->setPalette(id.url->viewport()->palette());
	  id.lcnum->setFocus();
	}
      else if(action == actions[8])
	{
	  id.callnum->clear();
	  id.callnum->setPalette(id.url->viewport()->palette());
	  id.callnum->setFocus();
	}
      else if(action == actions[9])
	{
	  id.deweynum->clear();
	  id.deweynum->setPalette(id.url->viewport()->palette());
	  id.deweynum->setFocus();
	}
      else if(action == actions[21])
	{
	  id.url->clear();
	  id.url->setFocus();
	}

      actions.clear();
    }
  else
    {
      /*
      ** Reset all.
      */

      id.id->clear();
      id.title->clear();

      if(!engWindowTitle.contains("Search"))
	id.author->setPlainText("N/A");
      else
	id.author->clear();

      if(!engWindowTitle.contains("Search"))
	id.publisher->setPlainText("N/A");
      else
	id.publisher->clear();

      if(!engWindowTitle.contains("Search"))
	id.place->setPlainText("N/A");
      else
	id.place->clear();

      if(!engWindowTitle.contains("Search"))
	id.category->setPlainText("N/A");
      else
	id.category->clear();

      if(engWindowTitle.contains("Search"))
	id.publication_date->setDate(QDate::fromString("01/7999",
						       "MM/yyyy"));
      else
	id.publication_date->setDate(QDate::fromString("01/01/2000",
						       "MM/dd/yyyy"));

      id.quantity->setValue(id.quantity->minimum());

      if(!engWindowTitle.contains("Search"))
	id.description->setPlainText("N/A");
      else
	id.description->clear();

      id.isbn13->clear();
      id.lcnum->clear();
      id.callnum->clear();
      id.deweynum->clear();
      id.location->setCurrentIndex(0);
      id.edition->setCurrentIndex(0);
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
      id.edition->setStyleSheet(cb_orig_ss);
      id.category->viewport()->setPalette(te_orig_pal);
      id.publication_date->setStyleSheet(dt_orig_ss);
      id.author->viewport()->setPalette(te_orig_pal);
      id.description->viewport()->setPalette(te_orig_pal);
      id.publisher->viewport()->setPalette(te_orig_pal);
      id.place->viewport()->setPalette(te_orig_pal);
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
  if(engWindowTitle.contains("Create") ||
     engWindowTitle.contains("Modify"))
    if(hasDataChanged(this))
      if(QMessageBox::question(this, tr("BiblioteQ: Question"),
			       tr("You have unsaved data. Continue closing?"),
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
  copy_editor *copyeditor = 0;

  if((copyeditor = new(std::nothrow) copy_editor
      (static_cast<QWidget *> (this),
       static_cast<qtbook_item *> (this),
       false,
       id.quantity->value(), oid,
       id.id->text(),
       id.quantity, font(),
       "Book")) != 0)
    copyeditor->populateCopiesEditor();
}

/*
** -- slotShowUsers() --
*/

void qtbook_book::slotShowUsers(void)
{
  int state = 0;
  borrowers_editor *borrowerseditor = 0;

  if(!id.okButton->isHidden())
    state = qtbook::EDITABLE;
  else
    state = qtbook::VIEW_ONLY;

  if((borrowerseditor = new(std::nothrow) borrowers_editor
      (static_cast<QWidget *> (this), static_cast<qtbook_item *> (this),
       id.quantity->value(), oid, id.id->text(), font(), "Book",
       state)) != 0)
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
  qtbook_item_working_dialog working(static_cast<QMainWindow *> (this));

  if(!(id.id->text().trimmed().length() == 10 ||
       id.isbn13->text().trimmed().length() == 13))
    {
      QMessageBox::critical
	(this, tr("BiblioteQ: User Error"),
	 tr("In order to query a Z39.50 system, either the ISBN-10 "
	    "or ISBN-13 must be provided."));
      id.id->setFocus();
      return;
    }

  if((thread = new(std::nothrow) generic_thread()) != 0)
    {
      working.setModal(true);
      working.setWindowTitle(tr("BiblioteQ: Z39.50 Data Retrieval"));
      working.setLabelText(tr("Downloading information from the Z39.50 "
			      "system. Please be patient."));
      working.setMaximum(0);
      working.setMinimum(0);
      working.setCancelButton(0);
      working.show();
      working.update();

      if(!id.id->text().isEmpty())
	searchstr = QString("@attr 1=7 %1").arg(id.id->text());
      else
	searchstr = QString("@attr 1=7 %1").arg(id.isbn13->text());

      thread->setType(generic_thread::QUERY_LIBRARY_OF_CONGRESS);
      thread->setLOCSearchString(searchstr);
      thread->start();

      while(thread->isRunning())
	{
	  qapp->processEvents();
	  thread->msleep(10);
	}

      working.hide();

      if((errorstr = thread->getErrorStr()).isEmpty() &&
	 !thread->getLOCResults().isEmpty())
	{
	  if(QMessageBox::question
	     (this, tr("BiblioteQ: Question"),
	      tr("Replace existing values with those retrieved "
		 "from the Z39.50 system?"),
	      QMessageBox::Yes | QMessageBox::No,
	      QMessageBox::No) == QMessageBox::Yes)
	    {
	      list = QString(thread->getLOCResults()[0]).split("\n");
	      id.edition->setCurrentIndex(0);
	      id.edition->setStyleSheet("background-color: rgb(162, 205, 90)");

	      for(i = 0; i < list.size(); i++)
		if(list[i].startsWith("100") ||
		   list[i].startsWith("700"))
		  id.author->clear();
		else if(list[i].startsWith("260"))
		  id.place->clear();
		else if(list[i].startsWith("650"))
		  id.category->clear();

	      for(i = 0; i < list.size(); i++)
		{
		  str = list[i];

		  if(str.startsWith("010"))
		    {
		      /*
		      ** $a - LC Control Number
		      ** $b - NUCMC Control Number
		      ** $z - Canceled/Invalid LC Control Number
		      ** $8 - Field Link and Sequence Number
		      */

		      str = str.mid(str.indexOf("$a") + 2).trimmed();
		      str = str.remove(" $b").remove(" $z").remove(" $8").
			trimmed();
		      id.lcnum->setText(str);
		      misc_functions::highlightWidget
			(id.lcnum, QColor(162, 205, 90));
		    }
		  else if(str.startsWith("020"))
		    {
		      /*
		      ** $a - International Standard Book Number
		      ** $c - Terms of Availability
		      ** $z - Canceled/Invalid ISBN
		      ** $6 - Linkage
		      ** $8 - Field Link and Sequence Number
		      */

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
		      /*
		      ** $a - Classification Number
		      ** $b - Item Number
		      ** $d - Supplementary Class Number (Obsolete)
		      ** $3 - Materials Specified
		      ** $6 - Linkage
		      ** $8 - Field Link and Sequence Number
		      */

		      str = str.mid(str.indexOf("$a") + 2).trimmed();
		      str = str.remove(" $b").remove(" $d").remove(" $3").
			remove(" $6").remove(" $8").trimmed();
		      id.callnum->setText(str);
		      misc_functions::highlightWidget
			(id.callnum, QColor(162, 205, 90));
		    }
		  else if(str.startsWith("082"))
		    {
		      /*
		      ** $a - Classification Number
		      ** $b - Item Number
		      ** $b - DDC Number--abridged NST Version (Obsolete)
		      ** $2 - Edition Number
		      ** $6 - Linkage
		      ** $8 - Field Link and Sequence Number
		      */

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
		      /*
		      ** $a - Personal Name
		      ** $b - Numeration
		      ** $c - Titles and other Words associated with a Name
		      ** $d - Dates associated with a Name
		      ** $e - Relator Term
		      ** $f - Date of a Work
		      ** $g - Miscellaneous Information
		      ** $j - Attribution Qualifier
		      ** $k - Form Subheading
		      ** $l - Language of a Work
		      ** $n - Number of Part/Section of a Work
		      ** $p - Name of Part/Section of a Work
		      ** $q - Fuller Form of Name
		      ** $t - Title of a Work
		      ** $u - Affiliation
		      ** $0 - Authority Record Control Number
		      ** $4 - Relator Code
		      ** $6 - Linkage
		      ** $8 - Field Link and Sequence Number
		      */

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
		      removeList.append(" $0");
		      removeList.append(" $4");
		      removeList.append(" $6");
		      removeList.append(" $8");

		      while(!removeList.isEmpty())
			str = str.remove(removeList.takeFirst()).trimmed();

		      if(str.count(",") > 1)
			str = str.mid(0, str.lastIndexOf(","));

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

		      if(!id.author->toPlainText().contains(str))
			{
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
		    }
		  else if(str.startsWith("245"))
		    {
		      /*
		      ** $a - Title
		      ** $b - Remainder of Title
		      ** $c - Statement of Responsibility
		      ** $d - Designation of section (Obsolete)
		      ** $e - Name of Part/Section
		      ** $f - Inclusive Dates
		      ** $g - Bulk Dates
		      ** $h - Medium
		      ** $k - Form
		      ** $n - Number of Part/Section of a Work
		      ** $p - Name of Part/Section of a Work
		      ** $s - Version (NR)
		      ** $6 - Linkage (NR)
		      ** $8 - Field Link and Sequence Number
		      */

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

		      if(!str.isEmpty() && str[str.length() - 1].isPunct())
			str.remove(str.length() - 1, 1);

		      id.title->setText(str);
		      misc_functions::highlightWidget
			(id.title, QColor(162, 205, 90));
		    }
		  else if(str.startsWith("250"))
		    {
		      /*
		      ** $a - Edition Statement
		      ** $b - Remainder of Edition Statement
		      ** $6 - Linkage
		      ** $8 - Field Link and Sequence Number
		      */

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
		      /*
		      ** $a - Place of Publication, Distribution, etc.
		      ** $b - Name of Publisher, Distributor, etc.
		      ** $c - Date of Publication, Distribution, etc.
		      ** $d - Plate or Publisher's Number for Music
		      ** $e - Place of Manufacture
		      ** $f - Manufacturer
		      ** $g - Date of Manufacture
		      ** $3 - Materials Specified
		      ** $6 - Linkage
		      ** $8 - Field Link and Sequence Number
		      */

		      QString tmpstr = str.mid(str.indexOf("$a") + 2).
			trimmed();

		      if(tmpstr.contains("$b"))
			tmpstr = tmpstr.mid(0, tmpstr.indexOf("$b")).trimmed();
		      else
			tmpstr = tmpstr.mid(0, tmpstr.indexOf("$c")).trimmed();

		      tmplist = tmpstr.split("$a");

		      for(j = 0; j < tmplist.size(); j++)
			{
			  tmpstr = tmplist.at(j).trimmed();
			  tmpstr = tmpstr.mid(0, tmpstr.lastIndexOf(" ")).
			    trimmed();

			  if(tmpstr.isEmpty())
			    continue;

			  if(!tmpstr[0].isLetterOrNumber())
			    tmpstr = tmpstr.mid(1).trimmed();

			  if(tmpstr.isEmpty())
			    continue;

			  if(tmpstr[tmpstr.length() - 1] == ',')
			    tmpstr.remove(tmpstr.length() - 1, 1);

			  if(id.place->toPlainText().isEmpty())
			    id.place->setPlainText(tmpstr);
			  else
			    id.place->setPlainText(id.place->toPlainText() +
						   "\n" + tmpstr);
			}

		      misc_functions::highlightWidget
			(id.place->viewport(), QColor(162, 205, 90));

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

		      if(str.contains("$a"))
			{
			  str = str.mid(0, str.indexOf("$a")).trimmed();
			  str = str.mid(0, str.lastIndexOf(" ")).trimmed();
			}
		      else
			str = str.mid(0, str.indexOf("$c")).trimmed();

		      if(str.endsWith(","))
			str = str.mid(0, str.length() - 1);

		      id.publisher->setPlainText(str);
		      misc_functions::highlightWidget
			(id.publisher->viewport(), QColor(162, 205, 90));
		    }
		  else if(str.startsWith("300"))
		    {
		      /*
		      ** $a - Extent
		      ** $b - Other Physical Details
		      ** $c - Dimensions
		      ** $e - Accompanying Material
		      ** $f - Type of Unit
		      ** $g - Size of Unit
		      ** $3 - Materials Specified
		      ** $6 - Linkage
		      ** $8 - Field Link and Sequence Number
		      */

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
		      /*
		      ** $a - Topical Term or Geographic Name as Entry Element
		      ** $b - Topical Term following Geographic Name as Entry
		      **      Element
		      ** $c - Location of Event
		      ** $d - Active Dates
		      ** $e - Relator Term
		      ** $v - Form Subdivision
		      ** $x - General Subdivision
		      ** $y - Chronological Subdivision
		      ** $z - Geographic Subdivision
		      ** $0 - Authority Record Control Number
		      ** $2 - Source of Heading or Term
		      ** $3 - Materials Specified
		      ** $4 - Relator Code
		      ** $6 - Linkage
		      ** $8 - Field Link and Sequence Number
		      */

		      str = str.mid(str.indexOf("$a") + 2).trimmed();

		      if(str.contains("$v"))
			str = str.mid(0, str.indexOf("$v")).trimmed();

		      if(str.contains("$x"))
			str = str.mid(0, str.indexOf("$x")).trimmed();

		      if(str.contains("$y"))
			str = str.mid(0, str.indexOf("$y")).trimmed();

		      if(str.contains("$z"))
			str = str.mid(0, str.indexOf("$z")).trimmed();

		      if(!str.isEmpty())
			{
			  if(!str[str.length() - 1].isPunct())
			    str += ".";

			  if(!id.category->toPlainText().contains(str))
			    {
			      if(!id.category->toPlainText().isEmpty())
				id.category->setPlainText
				  (id.category->toPlainText() + "\n" + str);
			      else
				id.category->setPlainText(str);

			      misc_functions::highlightWidget
				(id.category->viewport(),
				 QColor(162, 205, 90));
			    }
			}
		    }
		}

	      foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
		textfield->setCursorPosition(0);
	    }
	}
      else if(errorstr.isEmpty() && thread->getLOCResults().isEmpty())
	QMessageBox::critical
	  (this, tr("BiblioteQ: Z39.50 Query Error"),
	   tr("A Z39.50 entry may not yet exist for ") +
	   id.id->text() + tr("."));
      else
	etype = thread->getEType();

      thread->deleteLater();
    }
  else
    {
      etype = tr("Memory Error");
      errorstr = tr("Unable to create a thread due to "
		    "insufficient resources.");
    }

  if(!errorstr.isEmpty())
    {
      qmain->addError(QString(tr("Z39.50 Query Error")), etype, errorstr,
		      __FILE__, __LINE__);
      QMessageBox::critical
	(this, tr("BiblioteQ: Z39.50 Query Error"),
	 tr("The Z39.50 entry could not be retrieved."));
    }
}

/*
** -- slotPrint() --
*/

void qtbook_book::slotPrint(void)
{
  html = "";
  html += "<b>" + tr("ISBN-10:") + "</b> " +
    id.id->text().trimmed() + "<br>";
  html += "<b>" + tr("ISBN-13:") + "</b> " +
    id.isbn13->text().trimmed() + "<br>";
  html += "<b>" + tr("Edition:") + "</b> " +
    id.edition->currentText() + "<br>";
  html += "<b>" + tr("Author(s):") + "</b> " +
    id.author->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Book Binding Type:") + "</b> " +
    id.binding->currentText() + "<br>";
  html += "<b>" + tr("LC Control Number:") + "</b> " +
    id.lcnum->text().trimmed() + "<br>";
  html += "<b>" + tr("Call Number:") + "</b> " +
    id.callnum->text().trimmed() + "<br>";
  html += "<b>" + tr("Dewey Class Number:") + "</b> " +
    id.deweynum->text().trimmed() + "<br>";

  /*
  ** General information.
  */

  html += "<b>" + tr("Title:") + "</b> " +
    id.title->text().trimmed() + "<br>";
  html += "<b>" + tr("Publication Date:") + "</b> " +
    id.publication_date->date().toString("MM/dd/yyyy") + "<br>";
  html += "<b>" + tr("Publisher:") + "</b> " +
    id.publisher->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Place of Publication:") + "</b> " +
    id.place->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Categories:") + "</b> " +
    id.category->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Price:") + "</b> " + id.price->text() + "<br>";
  html += "<b>" + tr("Language:") + "</b> " +
    id.language->currentText() + "<br>";
  html += "<b>" + tr("Monetary Units:") + "</b> " +
    id.monetary_units->currentText() + "<br>";
  html += "<b>" + tr("Copies:") + "</b> " + id.quantity->text() + "<br>";
  html += "<b>" + tr("Location:") + "</b> " +
    id.location->currentText() + "<br>";
  html += "<b>" + tr("Abstract:") + "</b> " +
    id.description->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("OFFSYSTEM URL:") + "</b> " +
    id.url->toPlainText().trimmed();
  print(this);
}

/*
** -- slotSelectImage() --
*/

void qtbook_book::slotSelectImage(void)
{
  QFileDialog dialog(this);
  QPushButton *button = qobject_cast<QPushButton *> (sender());

  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter("Image Files (*.bmp *.jpg *.jpeg *.png)");

  if(button == id.frontButton)
    dialog.setWindowTitle(tr("BiblioteQ: Front Cover Image Selection"));
  else
    dialog.setWindowTitle(tr("BiblioteQ: Back Cover Image Selection"));

  dialog.exec();

  if(dialog.result() == QDialog::Accepted)
    {
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
}

/*
** -- slotGenerateISBN() --
*/

void qtbook_book::slotGenerateISBN(void)
{
  id.id->setText(QString::number(QDateTime::currentDateTime().toTime_t()));
}

/*
** -- slotDownloadImage() --
*/

void qtbook_book::slotDownloadImage(void)
{
  QString url = "";
  QPushButton *pb = qobject_cast<QPushButton *> (sender());

  if(httpProgress->isVisible())
    return;

  if(requestid1 > 0 && pb == id.dwnldFront)
    return;

  if(requestid2 > 0 && pb == id.dwnldBack)
    return;

  if(id.id->text().trimmed().length() != 10)
    {
      QMessageBox::critical
	(this, tr("BiblioteQ: User Error"),
	 tr("In order to download a cover image from Amazon, the ISBN-10 "
	    "must be provided."));
      id.id->setFocus();
      return;
    }

  httpRequestAborted = false;
  httpProgress->setMaximum(0);
  httpProgress->setMinimum(0);

  if(pb == id.dwnldFront)
    {
      httpProgress->setWindowTitle
	(tr("BiblioteQ: Front Cover Image Download"));
      httpProgress->setLabelText(tr("Downloading the front cover image. "
				    "Please be patient..."));
      url = qmain->getAmazonHash()["front_cover_path"].replace
	("%", id.id->text().trimmed());
      imgbuffer1->open(QIODevice::WriteOnly);
    }
  else
    {
      httpProgress->setWindowTitle(tr("BiblioteQ: Back Cover Image Download"));
      httpProgress->setLabelText(tr("Downloading the back cover image. "
				    "Please be patient..."));
      url = qmain->getAmazonHash()["back_cover_path"].replace
	("%", id.id->text().trimmed());
      imgbuffer2->open(QIODevice::WriteOnly);
    }

  httpProgress->show();
  QByteArray path = QUrl::toPercentEncoding(url, "!$&'()*+,;=:@/");

  if(pb == id.dwnldFront)
    requestid1 = http1->get(path, imgbuffer1);
  else
    requestid2 = http2->get(path, imgbuffer2);
}

/*
** -- slotHttpRequestFinished() --
*/

void qtbook_book::slotHttpRequestFinished(int rqid, bool error)
{
  if(httpRequestAborted)
    {
      httpProgress->hide();
      return;
    }

  httpProgress->hide();

  if(!error)
    {
      if(rqid == requestid1)
	{
	  if(imgbytes1.size() > 1000)
	    {
	      id.front_image->clear();
	      id.front_image->loadFromData(imgbytes1);
	    }

	  imgbuffer1->close();
	  (void) http1->close();

	  if(imgbytes1.size() < 1000)
	    QMessageBox::warning
	      (this, tr("BiblioteQ: HTTP Warning"),
	       tr("The front cover image for the specified "
		  "ISBN may not exist."));
	}
      else if(rqid == requestid2)
	{
	  if(imgbytes2.size() > 1000)
	    {
	      id.back_image->clear();
	      id.back_image->loadFromData(imgbytes2);
	    }

	  imgbuffer2->close();
	  (void) http2->close();

	  if(imgbytes2.size() < 1000)
	    QMessageBox::warning
	      (this, tr("BiblioteQ: HTTP Warning"),
	       tr("The back cover image for the specified ISBN "
		  "may not exist."));
	}
    }

  if(error)
    {
      if(rqid == requestid1)
	QMessageBox::critical
	  (this, tr("BiblioteQ: HTTP Error"),
	   QString(tr("Front cover image download failed: ")) +
	   http1->errorString() + tr("."));
      else if(rqid == requestid2)
	QMessageBox::critical
	  (this, tr("BiblioteQ: HTTP Error"),
	   QString(tr("Back cover image download failed: ")) +
	   http2->errorString() + tr("."));
    }

  requestid1 = requestid2 = 0;
}

/*
** -- slotUpdateDataReadProgress() --
*/

void qtbook_book::slotUpdateDataReadProgress(int bytesread, int totalbytes)
{
  if(httpRequestAborted)
    return;

  httpProgress->setMaximum(totalbytes);
  httpProgress->setValue(bytesread);
}

/*
** -- slotCancelImageDownload() --
*/

void qtbook_book::slotCancelImageDownload(void)
{
  httpRequestAborted = true;
  imgbuffer1->close();
  imgbuffer2->close();
  http1->abort();
  http2->abort();
  (void) http1->close();
  (void) http2->close();
  requestid1 = requestid2 = 0;
}
