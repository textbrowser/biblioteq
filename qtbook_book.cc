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
			 const QString &oidArg,
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

  if((httpProgress = new(std::nothrow) qtbook_item_working_dialog(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  parentWid = parentArg;
  oid = oidArg;
  row = rowArg;
  oldq = misc_functions::getColumnString
    (qmain->getUI().table, row, tr("Quantity")).toInt();
  id.setupUi(this);
#ifdef Q_WS_MAC
  setAttribute(Qt::WA_MacMetalStyle, true);
#endif
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
  connect(id.isbn13to10, SIGNAL(clicked(void)), this,
	  SLOT(slotConvertISBN13to10(void)));
  connect(id.printButton, SIGNAL(clicked(void)), this, SLOT(slotPrint(void)));
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  id.dwnldFront,
	  SLOT(setEnabled(bool)));
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  id.dwnldBack,
	  SLOT(setEnabled(bool)));
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  id.queryButton,
	  SLOT(setEnabled(bool)));
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  id.id,
	  SLOT(setEnabled(bool)));
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  id.isbn13,
	  SLOT(setEnabled(bool)));
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  id.isbn10to13,
	  SLOT(setEnabled(bool)));
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  id.isbn13to10,
	  SLOT(setEnabled(bool)));
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
  connect(menu->addAction(tr("Reset &MARC Tags")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Keywords")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(id.frontButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(id.backButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(id.dwnldFront, SIGNAL(clicked(void)), this,
	  SLOT(slotDownloadImage(void)));
  connect(id.dwnldBack, SIGNAL(clicked(void)), this,
	  SLOT(slotDownloadImage(void)));
  connect(httpProgress, SIGNAL(canceled(void)), this,
	  SLOT(slotCancelImageDownload(void)));
  connect(httpProgress, SIGNAL(rejected(void)), this,
	  SLOT(slotCancelImageDownload(void)));
  id.id->setValidator(validator1);
  id.isbn13->setValidator(validator2);
  id.resetButton->setMenu(menu);
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  menu->actions()[2],
	  SLOT(setEnabled(bool)));
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  menu->actions()[3],
	  SLOT(setEnabled(bool)));

  QString errorstr("");

  qapp->setOverrideCursor(Qt::WaitCursor);
  id.language->addItems
    (misc_functions::getLanguages(qmain->getDB(),
				  errorstr));
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the languages.")),
       errorstr, __FILE__, __LINE__);

  qapp->setOverrideCursor(Qt::WaitCursor);
  id.monetary_units->addItems
    (misc_functions::getMonetaryUnits(qmain->getDB(),
				      errorstr));
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the monetary units.")),
       errorstr, __FILE__, __LINE__);

  qapp->setOverrideCursor(Qt::WaitCursor);
  id.location->addItems
    (misc_functions::getLocations(qmain->getDB(),
				  "Book",
				  errorstr));
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the book locations.")),
       errorstr, __FILE__, __LINE__);

  id.front_image->setScene(scene1);
  id.back_image->setScene(scene2);
  httpProgress->setModal(true);

  if(id.language->findText(tr("UNKNOWN")) == -1)
    id.language->addItem(tr("UNKNOWN"));

  if(id.monetary_units->findText(tr("UNKNOWN")) == -1)
    id.monetary_units->addItem(tr("UNKNOWN"));

  if(id.location->findText(tr("UNKNOWN")) == -1)
    id.location->addItem(tr("UNKNOWN"));

  QActionGroup *actionGroup = 0;

  if((actionGroup = new(std::nothrow) QActionGroup(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  bool found = false;
  QMap<QString, QHash<QString, QString> > hashes(qmain->getZ3950Maps());

  for(int i = 0; i < hashes.size(); i++)
    {
      QAction *action = actionGroup->addAction(hashes.keys().at(i));

      if(!action)
	continue;

      action->setCheckable(true);
      id.queryButton->addAction(action);

      if(qmain->getPreferredZ3950Site() == action->text())
	{
	  found = true;
	  action->setChecked(true);
	}
    }

  if(id.queryButton->actions().isEmpty())
    id.queryButton->setPopupMode(QToolButton::DelayedPopup);
  else if(!found)
    id.queryButton->actions()[0]->setChecked(true);

  hashes.clear();

  for(int i = 1; i <= 1000; i++)
    id.edition->addItem(QString::number(i));

  /*
  ** Save some palettes and style sheets.
  */

  cb_orig_ss = id.edition->styleSheet();
  dt_orig_ss = id.publication_date->styleSheet();
  white_pal = te_orig_pal = id.author->viewport()->palette();

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
	      QMessageBox::critical
		(this, tr("BiblioteQ: User Error"),
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

      str = id.id->text().trimmed();
      id.id->setText(str);

      str = id.isbn13->text().trimmed();
      id.isbn13->setText(str);

      if(id.isbnAvailableCheckBox->isChecked())
	if(id.id->text().length() == 10)
	  slotConvertISBN10to13();

      if(id.isbnAvailableCheckBox->isChecked())
	if(id.isbn13->text().length() == 13)
	  slotConvertISBN13to10();

      if(id.isbnAvailableCheckBox->isChecked())
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

	    return;
	  }

      str = id.author->toPlainText().trimmed();
      id.author->setPlainText(str);

      if(id.author->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Author(s) field."));
	  id.author->setFocus();
	  return;
	}

      str = id.title->text().trimmed();
      id.title->setText(str);

      if(id.title->text().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Title field."));
	  id.title->setFocus();
	  return;
	}

      str = id.publisher->toPlainText().trimmed();
      id.publisher->setPlainText(str);

      if(id.publisher->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Publisher field."));
	  id.publisher->setFocus();
	  return;
	}

      str = id.place->toPlainText().trimmed();
      id.place->setPlainText(str);

      if(id.place->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Place of Publication "
				   "field."));
	  id.place->setFocus();
	  return;
	}

      str = id.category->toPlainText().trimmed();
      id.category->setPlainText(str);

      if(id.category->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Categories field."));
	  id.category->setFocus();
	  return;
	}

      str = id.description->toPlainText().trimmed();
      id.description->setPlainText(str);

      if(id.description->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Abstract field."));
	  id.description->setFocus();
	  return;
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
      str = id.lcnum->text().trimmed();
      id.lcnum->setText(str);
      str = id.callnum->text().trimmed();
      id.callnum->setText(str);
      str = id.deweynum->text().trimmed();
      id.deweynum->setText(str);
      str = id.marc_tags->toPlainText().trimmed();
      id.marc_tags->setPlainText(str);
      str = id.keyword->toPlainText().trimmed();
      id.keyword->setPlainText(str);

      if(engWindowTitle.contains("Modify"))
	query.prepare("UPDATE book SET id = ?, "
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
		      "place = ?, "
		      "marc_tags = ?, "
		      "keyword = ? "
		      "WHERE "
		      "myoid = ?");
      else if(qmain->getDB().driverName() != "QSQLITE")
	query.prepare("INSERT INTO book (id, title, "
		      "edition, author, pdate, publisher, "
		      "category, price, description, language, "
		      "monetary_units, quantity, "
		      "binding_type, location, "
		      "isbn13, lccontrolnumber, callnumber, "
		      "deweynumber, front_cover, "
		      "back_cover, "
		      "place, marc_tags, keyword) "
		      "VALUES (?, ?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) RETURNING myoid");
      else
	query.prepare("INSERT INTO book (id, title, "
		      "edition, author, pdate, publisher, "
		      "category, price, description, language, "
		      "monetary_units, quantity, "
		      "binding_type, location, "
		      "isbn13, lccontrolnumber, callnumber, "
		      "deweynumber, front_cover, "
		      "back_cover, "
		      "place, marc_tags, keyword, myoid) "
		      "VALUES (?, ?, ?, "
		      "?, ?, ?, ?, "
		      "?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, ?, ?, ?, ?, ?, ?, "
		      "?, ?, ?)");

      if(id.isbnAvailableCheckBox->isChecked() &&
	 !id.id->text().isEmpty())
	query.bindValue(0, id.id->text());
      else
	query.bindValue(0, QVariant(QVariant::String));

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

      if(id.isbnAvailableCheckBox->isChecked() &&
	 !id.isbn13->text().isEmpty())
	query.bindValue(14, id.isbn13->text());
      else
	query.bindValue(14, QVariant(QVariant::String));

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
	  query.bindValue(18, bytes.toBase64());
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
	  query.bindValue(19, bytes.toBase64());
	}
      else
	{
	  id.back_image->imageFormat = "";
	  query.bindValue(19, QVariant());
	}

      query.bindValue(20, id.place->toPlainText().trimmed());
      query.bindValue(21, id.marc_tags->toPlainText().trimmed());
      query.bindValue(22, id.keyword->toPlainText().trimmed());

      if(engWindowTitle.contains("Modify"))
	query.bindValue(23, oid);
      else if(qmain->getDB().driverName() == "QSQLITE")
	{
	  qint64 value = misc_functions::getSqliteUniqueId(qmain->getDB(),
							   errorstr);

	  if(errorstr.isEmpty())
	    {
	      query.bindValue(23, value);
	      oid = QString::number(value);
	    }
	  else
	    qmain->addError(QString(tr("Database Error")),
			    QString(tr("Unable to generate a unique "
				       "integer.")),
			    errorstr);
	}

      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!query.exec())
	{
	  qapp->restoreOverrideCursor();
	  qmain->addError
	    (QString(tr("Database Error")),
	     QString(tr("Unable to create or update the entry.")),
	     query.lastError().text(), __FILE__, __LINE__);
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
		  goto db_rollback;
		}
	    }
	  else
	    {
	      /*
	      ** Create initial copies.
	      */

	      if(qmain->getDB().driverName() != "QSQLITE")
		{
		  oid = query.next();
		  oid = query.value(0).toString();
		}

	      if(id.id->text().isEmpty())
		misc_functions::createInitialCopies(oid,
						    id.quantity->value(),
						    qmain->getDB(),
						    "Book", errorstr);
	      else
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
		  goto db_rollback;
		}
	    }

	  if(!id.isbnAvailableCheckBox->isChecked())
	    {
	      id.id->clear();
	      id.isbn13->clear();
	    }

	  id.id->setPalette(te_orig_pal);
	  id.isbn13->setPalette(te_orig_pal);
	  id.edition->setStyleSheet(cb_orig_ss);
	  id.binding->setStyleSheet(cb_orig_ss);
	  id.category->viewport()->setPalette(te_orig_pal);
	  id.publication_date->setStyleSheet(dt_orig_ss);
	  id.author->viewport()->setPalette(te_orig_pal);
	  id.title->setPalette(te_orig_pal);
	  id.description->viewport()->setPalette(te_orig_pal);
	  id.marc_tags->viewport()->setPalette(white_pal);
	  id.keyword->viewport()->setPalette(white_pal);
	  id.publisher->viewport()->setPalette(te_orig_pal);
	  id.place->viewport()->setPalette(te_orig_pal);
	  id.lcnum->setPalette(white_pal);
	  id.callnum->setPalette(white_pal);
	  id.deweynum->setPalette(white_pal);
	  oldq = id.quantity->value();

	  if(id.front_image->image.isNull())
	    id.front_image->imageFormat = "";

	  if(id.back_image->image.isNull())
	    id.back_image->imageFormat = "";

	  id.author->setMultipleLinks("book_search", "author",
				      id.author->toPlainText());
	  id.category->setMultipleLinks("book_search", "category",
					id.category->toPlainText());
	  id.publisher->setMultipleLinks("book_search", "publisher",
					 id.publisher->toPlainText());
	  id.place->setMultipleLinks("book_search", "place",
				     id.place->toPlainText());
	  id.keyword->setMultipleLinks("book_search", "keyword",
				       id.keyword->toPlainText());
	  qapp->restoreOverrideCursor();

	  if(engWindowTitle.contains("Modify"))
	    {
	      if(!id.id->text().isEmpty())
		str = QString(tr("BiblioteQ: Modify Book Entry (")) +
		  id.id->text() + tr(")");
	      else
		str = tr("BiblioteQ: Modify Book Entry");

	      engWindowTitle = "Modify";
	      setWindowTitle(str);

	      if((qmain->getTypeFilterString() == "All" ||
		  qmain->getTypeFilterString() == "All Overdue" ||
		  qmain->getTypeFilterString() == "All Requested" ||
		  qmain->getTypeFilterString() == "All Reserved" ||
		  qmain->getTypeFilterString() == "Books") &&
		 oid == misc_functions::getColumnString(qmain->getUI().table,
							row, "MYOID") &&
		 misc_functions::getColumnString(qmain->getUI().table,
						 row, tr("Type")) == "Book")
		{
		  qmain->getUI().table->setSortingEnabled(false);

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
			  (id.publication_date->date().toString
			   (Qt::SystemLocaleShortDate));
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

		  qmain->getUI().table->setSortingEnabled(true);

		  foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
		    textfield->setCursorPosition(0);

		  qmain->slotResizeColumns();
		  qmain->slotDisplaySummary();
		}
	    }
	  else
	    {
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
	  (QString(tr("Database Error")), QString(tr("Rollback failure.")),
	   qmain->getDB().lastError().text(), __FILE__, __LINE__);

      qapp->restoreOverrideCursor();
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to create or update the entry. "
			       "Please verify that "
			       "the entry does not already exist."));
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
	// "book.front_cover "
	"FROM "
	"book LEFT JOIN item_borrower_vw ON "
	"book.myoid = item_borrower_vw.item_oid "
	"AND item_borrower_vw.type = 'Book' "
	"WHERE ";

      if(id.isbnAvailableCheckBox->isChecked())
	searchstr.append("id LIKE '%" + id.id->text().trimmed() +
			 "%' AND ");
      else
	searchstr.append("id IS NULL AND ");

      searchstr.append("title LIKE '%" +
		       myqstring::escape(id.title->text().trimmed()) +
		       "%' AND ");

      if(id.isbnAvailableCheckBox->isChecked())
	searchstr.append("isbn13 LIKE '%" +
			 id.isbn13->text().trimmed() + "%' AND ");
      else
	searchstr.append("isbn13 IS NULL AND ");

      searchstr.append("COALESCE(lccontrolnumber, '') LIKE '%" +
		       myqstring::escape(id.lcnum->text().trimmed()) +
		       "%' AND ");
      searchstr.append("COALESCE(callnumber, '') LIKE '%" +
		       myqstring::escape(id.callnum->text().trimmed()) +
		       "%' AND ");
      searchstr.append("COALESCE(deweynumber, '') LIKE '%" +
		       myqstring::escape(id.deweynum->text().trimmed()) +
		       "%' AND ");

      if(id.edition->currentText() != tr("Any"))
	searchstr.append("edition = '" +
			 id.edition->currentText().trimmed() +
			 "' AND ");

      searchstr.append("author LIKE '%" +
		       myqstring::escape(id.author->toPlainText().
					 trimmed()) + "%' AND ");

      if(id.publication_date->date().toString("MM/yyyy") != "01/7999")
	searchstr.append("SUBSTR(pdate, 1, 3) || SUBSTR(pdate, 7) = '" +
			 id.publication_date->date().toString
			 ("MM/yyyy") +
			 "' AND ");

      searchstr.append("publisher LIKE '%" +
		       myqstring::escape
		       (id.publisher->toPlainText().trimmed()) +
		       "%' AND ");
      searchstr.append("place LIKE '%" +
		       myqstring::escape
		       (id.place->toPlainText().trimmed()) +
		       "%' AND ");
      searchstr.append("category LIKE '%" +
		       myqstring::escape
		       (id.category->toPlainText().trimmed()) +
		       "%' AND ");

      if(id.price->value() > -0.01)
	{
	  searchstr.append("price = ");
	  searchstr.append(QString::number(id.price->value()));
	  searchstr.append(" AND ");
	}

      if(id.language->currentText() != tr("Any"))
	searchstr.append("language = '" +
			 myqstring::escape(id.language->currentText().
					   trimmed()) +
			 "' AND ");

      if(id.monetary_units->currentText() != tr("Any"))
	searchstr.append("monetary_units = '" +
			 myqstring::escape
			 (id.monetary_units->currentText().trimmed()) +
			 "' AND ");

      if(id.binding->currentText() != tr("Any"))
	searchstr.append("binding_type = '" +
			 myqstring::escape(id.binding->currentText().
					   trimmed()) +
			 "' AND ");

      searchstr.append("description LIKE '%" +
		       myqstring::escape
		       (id.description->toPlainText().trimmed()) +
		       "%' ");

      if(id.quantity->value() != 0)
	searchstr.append("AND quantity = " + id.quantity->text() + " ");

      if(id.location->currentText() != tr("Any"))
	searchstr.append("AND location = '" +
			 myqstring::escape
			 (id.location->currentText().trimmed()) + "' ");

      searchstr.append("AND COALESCE(marc_tags, '') LIKE '%" +
		       myqstring::escape
		       (id.marc_tags->toPlainText().trimmed()) +
		       "%' ");
      searchstr.append("AND COALESCE(keyword, '') LIKE '%" +
		       myqstring::escape
		       (id.keyword->toPlainText().trimmed()) +
		       "%' ");
      hide();

      /*
      ** Search the database.
      */

      (void) qmain->populateTable
	(qtbook::POPULATE_SEARCH, "Books", searchstr);
      slotCancel();
    }
}

/*
** -- search() --
*/

void qtbook_book::search(const QString &field, const QString &value)
{
  id.coverImages->setVisible(false);
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
  id.marc_tags->clear();
  id.keyword->clear();
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
      else if(field == "keyword")
	id.keyword->setPlainText(value);

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
      id.isbnAvailableCheckBox->setCheckable(true);
      id.copiesButton->setEnabled(true);
      id.showUserButton->setEnabled(true);
      id.okButton->setVisible(true);
      id.queryButton->setVisible(true);
      id.resetButton->setVisible(true);
      id.frontButton->setVisible(true);
      id.backButton->setVisible(true);
      id.dwnldFront->setVisible(true);
      id.dwnldBack->setVisible(true);
      id.isbn10to13->setVisible(true);
      id.isbn13to10->setVisible(true);

      if(!id.id->text().trimmed().isEmpty())
	str = QString(tr("BiblioteQ: Modify Book Entry (")) +
	  id.id->text() + tr(")");
      else
	str = tr("BiblioteQ: Modify Book Entry");

      engWindowTitle = "Modify";
    }
  else
    {
      id.isbnAvailableCheckBox->setCheckable(false);
      id.copiesButton->setVisible(false);
      id.showUserButton->setEnabled(true);
      id.okButton->setVisible(false);
      id.queryButton->setVisible(false);
      id.resetButton->setVisible(false);
      id.frontButton->setVisible(false);
      id.backButton->setVisible(false);
      id.dwnldFront->setVisible(false);
      id.dwnldBack->setVisible(false);
      id.isbn10to13->setVisible(false);
      id.isbn13to10->setVisible(false);

      if(!id.id->text().trimmed().isEmpty())
	str = QString(tr("BiblioteQ: View Book Details (")) +
	  id.id->text() + tr(")");
      else
	str = tr("BiblioteQ: View Book Details");

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
      id.isbnAvailableCheckBox->setCheckable(true);
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
      id.isbn10to13->setVisible(true);
      id.isbn13to10->setVisible(true);
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
      id.isbnAvailableCheckBox->setCheckable(false);
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
      id.isbn10to13->setVisible(false);
      id.isbn13to10->setVisible(false);

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
    "marc_tags, "
    "keyword "
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
	    id.publisher->setMultipleLinks("book_search", "publisher",
					   var.toString());
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
		  if(!var.toString().trimmed().isEmpty())
		    str = QString(tr("BiblioteQ: Modify Book Entry (")) +
		      var.toString() + tr(")");
		  else
		    str = tr("BiblioteQ: Modify Book Entry");

		  engWindowTitle = "Modify";
		}
	      else
		{
		  if(!var.toString().trimmed().isEmpty())
		    str = QString(tr("BiblioteQ: View Book Details (")) +
		      var.toString() + tr(")");
		  else
		    str = tr("BiblioteQ: View Book Details");

		  engWindowTitle = "View";
		}

	      id.id->setText(var.toString());
	      setWindowTitle(str);

	      if(query.record().isNull(i))
		id.isbnAvailableCheckBox->setChecked(false);
	      else
		id.isbnAvailableCheckBox->setChecked(true);
	    }
	  else if(fieldname == "description")
	    id.description->setPlainText(var.toString());
	  else if(fieldname == "marc_tags")
	    id.marc_tags->setPlainText(var.toString());
	  else if(fieldname == "keyword")
	    id.keyword->setMultipleLinks("book_search", "keyword",
					 var.toString());
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
		  id.front_image->loadFromData
		    (QByteArray::fromBase64(var.toByteArray()));

		  if(id.front_image->image.isNull())
		    id.front_image->loadFromData(var.toByteArray());
		}
	    }
	  else if(fieldname == "back_cover")
	    {
	      if(!query.record().field(i).isNull())
		{
		  id.back_image->loadFromData
		    (QByteArray::fromBase64(var.toByteArray()));

		  if(id.back_image->image.isNull())
		    id.back_image->loadFromData(var.toByteArray());
		}
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
  id.marc_tags->clear();
  id.keyword->clear();
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
      else if(action == actions[3])
	{
	  id.isbn13->clear();
	  id.isbn13->setPalette(te_orig_pal);
	  id.isbn13->setFocus();
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
      else if(action == actions[6])
	{
	  id.binding->setCurrentIndex(0);
	  id.binding->setStyleSheet(cb_orig_ss);
	  id.binding->setFocus();
	}
      else if(action == actions[7])
	{
	  id.lcnum->clear();
	  id.lcnum->setPalette(white_pal);
	  id.lcnum->setFocus();
	}
      else if(action == actions[8])
	{
	  id.callnum->clear();
	  id.callnum->setPalette(white_pal);
	  id.callnum->setFocus();
	}
      else if(action == actions[9])
	{
	  id.deweynum->clear();
	  id.deweynum->setPalette(white_pal);
	  id.deweynum->setFocus();
	}
      else if(action == actions[10])
	{
	  id.title->clear();
	  id.title->setPalette(te_orig_pal);
	  id.title->setFocus();
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
      else if(action == actions[20])
	{
	  if(!engWindowTitle.contains("Search"))
	    id.description->setPlainText("N/A");
	  else
	    id.description->clear();

	  id.description->viewport()->setPalette(te_orig_pal);
	  id.description->setFocus();
	}
      else if(action == actions[21])
	{
	  id.marc_tags->clear();
	  id.marc_tags->viewport()->setPalette(white_pal);
	  id.marc_tags->setFocus();
	}
      else if(action == actions[22])
	{
	  id.keyword->clear();
	  id.keyword->setFocus();
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

      id.marc_tags->clear();
      id.keyword->clear();
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
      id.title->setPalette(te_orig_pal);
      id.lcnum->setPalette(white_pal);
      id.deweynum->setPalette(white_pal);
      id.callnum->setPalette(white_pal);
      id.id->setPalette(te_orig_pal);
      id.isbn13->setPalette(te_orig_pal);
      id.edition->setStyleSheet(cb_orig_ss);
      id.binding->setStyleSheet(cb_orig_ss);
      id.category->viewport()->setPalette(te_orig_pal);
      id.publication_date->setStyleSheet(dt_orig_ss);
      id.author->viewport()->setPalette(te_orig_pal);
      id.description->viewport()->setPalette(te_orig_pal);
      id.marc_tags->viewport()->setPalette(white_pal);
      id.keyword->viewport()->setPalette(white_pal);
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
** -- slotConvertISBN13to10() --
*/

void qtbook_book::slotConvertISBN13to10(void)
{
  int total = 0;
  QString z("");
  QString isbnnum(id.isbn13->text().trimmed().mid(3, 9));

  for(int i = 0; i < 9; i++)
    if(i < isbnnum.length())
      total += isbnnum[i].digitValue() * (10 - i);
    else
      break;

  z = QString::number((11 - (total % 11)) % 11);

  if(z == "10")
    z = "X";

  id.id->setText(isbnnum + z);
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
	  if(e)
	    e->ignore();

	  return;
	}

  qmain->removeBook(this);
  QMainWindow::closeEvent(e);
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
  if(findChild<generic_thread *> ())
    return;

  int i = 0;
  int j = 0;
  QRegExp reg("[A-Z]{1}");
  QString str = "";
  QString etype = "";
  QString errorstr = "";
  QString searchstr = "";
  QStringList list;
  QStringList tmplist;
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

  if((thread = new(std::nothrow) generic_thread(this)) != 0)
    {
      working.setModal(true);
      working.setWindowTitle(tr("BiblioteQ: Z39.50 Data Retrieval"));
      working.setLabelText(tr("Downloading information from the Z39.50 "
			      "system. Please be patient..."));
      working.setMaximum(0);
      working.setMinimum(0);
      working.setCancelButton(0);
      working.show();
      working.update();

      if(!id.id->text().isEmpty())
	searchstr = QString("@attr 1=7 %1").arg(id.id->text());
      else
	searchstr = QString("@attr 1=7 %1").arg(id.isbn13->text());

      bool found = false;

      for(i = 0; i < id.queryButton->actions().size(); i++)
	if(id.queryButton->actions().at(i)->isChecked())
	  {
	    found = true;
	    thread->setZ3950Name
	      (id.queryButton->actions().at(i)->text());
	    break;
	  }

      if(!found)
	thread->setZ3950Name(qmain->getPreferredZ3950Site());

      thread->setType(generic_thread::Z3950_QUERY);
      thread->setZ3950SearchString(searchstr);
      thread->start();

      while(thread->isRunning())
	{
#ifndef Q_WS_MAC
	  qapp->processEvents();
#endif
	  thread->msleep(10);
	}

      working.hide();

      if((errorstr = thread->getErrorStr()).isEmpty() &&
	 !thread->getZ3950Results().isEmpty())
	{
	  if(QMessageBox::question
	     (this, tr("BiblioteQ: Question"),
	      tr("Replace existing values with those retrieved "
		 "from the Z39.50 system?"),
	      QMessageBox::Yes | QMessageBox::No,
	      QMessageBox::No) == QMessageBox::Yes)
	    {
	      list = QString(thread->getZ3950Results()[0]).split("\n");
	      id.edition->setCurrentIndex(0);
	      id.edition->setStyleSheet
		("background-color: rgb(162, 205, 90)");
	      id.marc_tags->setPlainText
		(thread->getZ3950Results()[0].trimmed());
	      misc_functions::highlightWidget
		(id.marc_tags->viewport(), QColor(162, 205, 90));

	      for(i = 0; i < list.size(); i++)
		if(list[i].startsWith("100 ") ||
		   list[i].startsWith("700 "))
		  id.author->clear();
		else if(list[i].startsWith("260 "))
		  id.place->clear();
		else if(list[i].startsWith("650 "))
		  id.category->clear();

	      for(i = 0; i < list.size(); i++)
		{
		  str = list[i];

		  if(str.startsWith("010 "))
		    {
		      /*
		      ** $a - LC control number (NR)
		      ** $b - NUCMC control number (R)
		      ** $z - Canceled/invalid LC control number (R)
		      ** $8 - Field link and sequence number (R)
		      */

		      str = str.mid(str.indexOf("$a") + 2).trimmed();

		      QStringList subfields;

		      subfields << "$b"
				<< "$z"
				<< "$8";

		      while(!subfields.isEmpty())
			if(str.contains(subfields.first()))
			  {
			    str = str.mid
			      (0, str.indexOf(subfields.first())).trimmed();
			    break;
			  }
			else
			  subfields.takeFirst();

		      id.lcnum->setText(str);
		      misc_functions::highlightWidget
			(id.lcnum, QColor(162, 205, 90));
		    }
		  else if(str.startsWith("020 "))
		    {
		      /*
		      ** $a - International Standard Book Number (NR)
		      ** $c - Terms of availability (NR)
		      ** $z - Canceled/invalid ISBN (R)
		      ** $6 - Linkage (NR)
		      ** $8 - Field link and sequence number (R)
		      */

		      str = str.mid(str.indexOf("$a") + 2).trimmed();

		      if(str.toLower().contains("hardcover"))
			{
			  id.binding->setCurrentIndex(0);
			  id.binding->setStyleSheet
			    ("background-color: rgb(162, 205, 90)");
			}
		      else if(str.toLower().contains("pbk."))
			{
			  id.binding->setCurrentIndex(1);
			  id.binding->setStyleSheet
			    ("background-color: rgb(162, 205, 90)");
			}

		      if(str.contains(" ") && str.indexOf(" ") == 10)
			{
			  str = str.mid(0, 10).trimmed();
			  id.id->setText(str);
			  misc_functions::highlightWidget
			    (id.id, QColor(162, 205, 90));
			}
		      else if(str.contains(" ") && str.indexOf(" ") == 13)
			{
			  str = str.mid(0, 13).trimmed();
			  id.isbn13->setText(str);
			  misc_functions::highlightWidget
			    (id.isbn13, QColor(162, 205, 90));
			}
		      else if(str.length() == 10)
			{
			  id.id->setText(str);
			  misc_functions::highlightWidget
			    (id.id, QColor(162, 205, 90));
			}
		    }
		  else if(str.startsWith("050 "))
		    {
		      /*
		      ** $a - Classification number (R)
		      ** $b - Item number (NR)
		      ** $3 - Materials specified (NR)
		      ** $6 - Linkage (NR)
		      ** $8 - Field link and sequence number (R)
		      */

		      str = str.mid(str.indexOf("$a") + 2).trimmed();
		      str = str.remove(" $b").trimmed();

		      QStringList subfields;

		      subfields << "$3"
				<< "$6"
				<< "$8";

		      while(!subfields.isEmpty())
			if(str.contains(subfields.first()))
			  {
			    str = str.mid
			      (0, str.indexOf(subfields.first())).trimmed();
			    break;
			  }
			else
			  subfields.takeFirst();

		      id.callnum->setText(str);
		      misc_functions::highlightWidget
			(id.callnum, QColor(162, 205, 90));
		    }
		  else if(str.startsWith("082 "))
		    {
		      /*
		      ** $a - Classification number (R)
		      ** $b - Item number (NR)
		      ** $m - Standard or optional designation (NR)
		      ** $q - Assigning agency (NR)
		      ** $2 - Edition number (NR)
		      ** $6 - Linkage (NR)
		      ** $8 - Field link and sequence number (R)
		      */

		      str = str.mid(str.indexOf("$a") + 2).trimmed();
		      str = str.remove(" $b").remove
			(" $m").remove(" $q").trimmed();

		      QStringList subfields;

		      subfields << "$2"
				<< "$6"
				<< "$8";

		      while(!subfields.isEmpty())
			if(str.contains(subfields.first()))
			  {
			    str = str.mid
			      (0, str.indexOf(subfields.first())).trimmed();
			    break;
			  }
			else
			  subfields.takeFirst();

		      id.deweynum->setText(str);
		      misc_functions::highlightWidget
			(id.deweynum, QColor(162, 205, 90));
		    }
		  else if(str.startsWith("100 ") ||
			  str.startsWith("700 "))
		    {
		      /*
		      ** $a - Personal name (NR)
		      ** $b - Numeration (NR)
		      ** $c - Titles and words associated with a name (R)
		      ** $d - Dates associated with a name (NR)
		      ** $e - Relator term (R)
		      ** $f - Date of a work (NR)
		      ** $g - Miscellaneous information (NR)
		      ** $j - Attribution qualifier (R)
		      ** $k - Form subheading (R)
		      ** $l - Language of a work (NR)
		      ** $n - Number of part/section of a work (R)
		      ** $p - Name of part/section of a work (R)
		      ** $q - Fuller form of name (NR)
		      ** $t - Title of a work (NR)
		      ** $u - Affiliation (NR)
		      ** $0 - Authority record control number (R)
		      ** $4 - Relator code (R)
		      ** $6 - Linkage (NR)
		      ** $8 - Field link and sequence number (R)
		      */

		      str = str.mid(str.indexOf("$a") + 2).trimmed();

		      QStringList subfields;

		      subfields << "$b"
				<< "$c"
				<< "$d"
				<< "$e"
				<< "$f"
				<< "$g"
				<< "$j"
				<< "$k"
				<< "$l"
				<< "$n"
				<< "$p"
				<< "$q"
				<< "$t"
				<< "$u"
				<< "$0"
				<< "$4"
				<< "$6"
				<< "$8";

		      while(!subfields.isEmpty())
			if(str.contains(subfields.first()))
			  {
			    str = str.mid
			      (0, str.indexOf(subfields.first())).trimmed();
			    break;
			  }
			else
			  subfields.takeFirst();

		      if(str.endsWith(","))
			str = str.mid(0, str.length() - 1).trimmed();

		      if(!id.author->toPlainText().contains(str))
			{
			  if(list[i].startsWith("100 "))
			    id.author->setPlainText(str);
			  else if(!id.author->toPlainText().isEmpty())
			    id.author->setPlainText
			      (id.author->toPlainText() + "\n" +
			       str);
			  else
			    id.author->setPlainText(str);

			  misc_functions::highlightWidget
			    (id.author->viewport(), QColor(162, 205, 90));
			}
		    }
		  else if(str.startsWith("245 "))
		    {
		      /*
		      ** $a - Title (NR)
		      ** $b - Remainder of title (NR)
		      ** $c - Statement of responsibility, etc. (NR)
		      ** $f - Inclusive dates (NR)
		      ** $g - Bulk dates (NR)
		      ** $h - Medium (NR)
		      ** $k - Form (R)
		      ** $n - Number of part/section of a work (R)
		      ** $p - Name of part/section of a work (R)
		      ** $s - Version (NR)
		      ** $6 - Linkage (NR)
		      ** $8 - Field link and sequence number (R)
		      */

		      str = str.mid(str.indexOf("$a") + 2).trimmed();
		      str = str.remove(" $b").trimmed();

		      QStringList subfields;

		      subfields << "$c"
				<< "$f"
				<< "$g"
				<< "$h"
				<< "$k"
				<< "$n"
				<< "$p"
				<< "$s"
				<< "$6"
				<< "$8";

		      while(!subfields.isEmpty())
			if(str.contains(subfields.first()))
			  {
			    str = str.mid
			      (0, str.indexOf(subfields.first())).trimmed();
			    break;
			  }
			else
			  subfields.takeFirst();

		      str = str.mid(0, str.lastIndexOf('/')).trimmed();

		      if(!str.isEmpty() && str[str.length() - 1].isPunct())
			str = str.remove(str.length() - 1, 1).trimmed();

		      id.title->setText(str);
		      misc_functions::highlightWidget
			(id.title, QColor(162, 205, 90));
		    }
		  else if(str.startsWith("250 "))
		    {
		      /*
		      ** $a - Edition statement (NR)
		      ** $b - Remainder of edition statement (NR)
		      ** $6 - Linkage (NR)
		      ** $8 - Field link and sequence number (R)
		      */

		      str = str.mid(str.indexOf("$a") + 2).trimmed();

		      QStringList subfields;

		      subfields << "$b"
				<< "$6"
				<< "$8";

		      while(!subfields.isEmpty())
			if(str.contains(subfields.first()))
			  {
			    str = str.mid
			      (0, str.indexOf(subfields.first())).trimmed();
			    break;
			  }
			else
			  subfields.takeFirst();

		      str = str.mid(0, str.indexOf(" ")).trimmed();

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
		  else if(str.startsWith("260 "))
		    {
		      /*
		      ** $a - Place of publication, distribution, etc. (R)
		      ** $b - Name of publisher, distributor, etc. (R)
		      ** $c - Date of publication, distribution, etc. (R)
		      ** $e - Place of manufacture (R)
		      ** $f - Manufacturer (R)
		      ** $g - Date of manufacture (R)
		      ** $3 - Materials specified (NR)
		      ** $6 - Linkage (NR)
		      ** $8 - Field link and sequence number (R)
		      */

		      QString tmpstr = str.mid(str.indexOf("$a") + 2).
			trimmed();

		      QStringList subfields;

		      subfields << "$b"
				<< "$c"
				<< "$e"
				<< "$f"
				<< "$g"
				<< "$3"
				<< "$6"
				<< "$8";

		      while(!subfields.isEmpty())
			if(tmpstr.contains(subfields.first()))
			  {
			    tmpstr = tmpstr.mid
			      (0, tmpstr.indexOf(subfields.first())).trimmed();
			    break;
			  }
			else
			  subfields.takeFirst();

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

			  if(!tmpstr[tmpstr.length() - 1].isLetter())
			    tmpstr = tmpstr.remove(tmpstr.length() - 1, 1).
			      trimmed();

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

		      if(str.contains("$b"))
			str = str.mid(str.indexOf("$b") + 2).trimmed();

		      if(str.contains("$a"))
			{
			  str = str.mid(0, str.indexOf("$a")).trimmed();
			  str = str.mid(0, str.lastIndexOf(" ")).trimmed();
			}
		      else
			str = str.mid(0, str.indexOf("$c")).trimmed();

		      if(str.endsWith(","))
			str = str.mid(0, str.length() - 1).trimmed();

		      id.publisher->setPlainText(str);
		      misc_functions::highlightWidget
			(id.publisher->viewport(), QColor(162, 205, 90));
		    }
		  else if(str.startsWith("300 "))
		    {
		      /*
		      ** $a - Extent (R)
		      ** $b - Other physical details (NR)
		      ** $c - Dimensions (R)
		      ** $e - Accompanying material (NR)
		      ** $f - Type of unit (R)
		      ** $g - Size of unit (R)
		      ** $3 - Materials specified (NR)
		      ** $6 - Linkage (NR)
		      ** $8 - Field link and sequence number (R)
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
		  else if(str.startsWith("650 "))
		    {
		      /*
		      ** $a - Topical term or geographic name entry
		      **      element (NR)
		      ** $b - Topical term following geographic name entry
		      **      element (NR)
		      ** $c - Location of event (NR)
		      ** $d - Active dates (NR)
		      ** $e - Relator term (R)
		      ** $4 - Relator code (R)
		      ** $v - Form subdivision (R)
		      ** $x - General subdivision (R)
		      ** $y - Chronological subdivision (R)
		      ** $z - Geographic subdivision (R)
		      ** $0 - Authority record control number (R)
		      ** $2 - Source of heading or term (NR)
		      ** $3 - Materials specified (NR)
		      ** $6 - Linkage (NR)
		      ** $8 - Field link and sequence number (R) 
		      */

		      str = str.mid(str.indexOf("$a") + 2).trimmed();

		      QStringList subfields;

		      subfields << "$b"
				<< "$c"
				<< "$d"
				<< "$e"
				<< "$4"
				<< "$v"
				<< "$x"
				<< "$y"
				<< "$z"
				<< "$0"
				<< "$2"
				<< "$3"
				<< "$6"
				<< "$8";

		      while(!subfields.isEmpty())
			if(str.contains(subfields.first()))
			  {
			    str = str.mid
			      (0, str.indexOf(subfields.first())).trimmed();
			    break;
			  }
			else
			  subfields.takeFirst();

		      if(!str.isEmpty())
			{
			  if(!str[str.length() - 1].isPunct())
			    str += ".";

			  if(!id.category->toPlainText().contains(str))
			    {
			      if(!id.category->toPlainText().isEmpty())
				id.category->setPlainText
				  (id.category->toPlainText() + "\n" +
				   str);
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
      else if(errorstr.isEmpty() && thread->getZ3950Results().isEmpty())
	QMessageBox::critical
	  (this, tr("BiblioteQ: Z39.50 Query Error"),
	   tr("A Z39.50 entry may not yet exist for ") +
	   (id.id->text().isEmpty() ? id.isbn13->text() : id.id->text()) +
	   tr("."));
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
    id.publication_date->date().toString(Qt::SystemLocaleShortDate) + "<br>";
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
  html += "<b>" + tr("MARC Tags:") + "</b> " +
    id.marc_tags->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Keywords:") + "</b> " +
    id.keyword->toPlainText().trimmed() + "<br>";
  print(this);
}

/*
** -- slotSelectImage() --
*/

void qtbook_book::slotSelectImage(void)
{
  QFileDialog dialog(this);
  QPushButton *button = qobject_cast<QPushButton *> (sender());

#ifdef Q_WS_MAC
  dialog.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setDirectory(QDir::homePath());
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
** -- slotDownloadImage() --
*/

void qtbook_book::slotDownloadImage(void)
{
  if(httpProgress->isVisible())
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

  QBuffer *imgbuffer = findChild<QBuffer *> ();

  if(imgbuffer)
    return;

  QNetworkAccessManager *manager = findChild<QNetworkAccessManager *> ();

  if(manager)
    return;

  if(manager->findChild<QNetworkReply *> ())
    return;

  if((manager = new(std::nothrow) QNetworkAccessManager(this)) == 0)
    return;

  if((imgbuffer = new(std::nothrow) QBuffer(this)) == 0)
    {
      manager->deleteLater();
      return;
    }

  QPushButton *pb = qobject_cast<QPushButton *> (sender());

  if(pb == id.dwnldFront)
    imgbuffer->setProperty("which", "front");
  else
    imgbuffer->setProperty("which", "back");

  imgbuffer->open(QIODevice::WriteOnly);

  QUrl url;

  if(pb == id.dwnldFront)
    url = QUrl::fromUserInput
      (qmain->getAmazonHash()["front_cover_host"] +
       qmain->getAmazonHash()["front_cover_path"].replace
       ("%", id.id->text().trimmed()));
  else
    url = QUrl::fromUserInput
      (qmain->getAmazonHash()["back_cover_host"] +
       qmain->getAmazonHash()["back_cover_path"].replace
       ("%", id.id->text().trimmed()));

  QNetworkReply *reply = manager->get(QNetworkRequest(url));

  if(!reply)
    {
      manager->deleteLater();
      imgbuffer->deleteLater();
    }

  connect(reply, SIGNAL(readyRead(void)),
	  this, SLOT(slotReadyRead(void)));
  connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
	  this, SLOT(slotDataTransferProgress(qint64, qint64)));
  connect(reply, SIGNAL(finished(void)),
	  this, SLOT(slotDownloadFinished(void)));
  httpProgress->setMaximum(0);
  httpProgress->setMinimum(0);
  httpProgress->show();

  if(pb == id.dwnldFront)
    {
      httpProgress->setWindowTitle
	(tr("BiblioteQ: Front Cover Image Download"));
      httpProgress->setLabelText(tr("Downloading the front cover image. "
				    "Please be patient..."));
    }
  else
    {
      httpProgress->setWindowTitle
	(tr("BiblioteQ: Back Cover Image Download"));
      httpProgress->setLabelText(tr("Downloading the back cover image. "
				    "Please be patient..."));
    }
}

/*
** -- slotDownloadFinished() --
*/

void qtbook_book::slotDownloadFinished(void)
{
  if(httpProgress->isVisible())
    httpProgress->hide();

  QNetworkAccessManager *manager = findChild<QNetworkAccessManager *> ();

  if(manager)
    manager->deleteLater();

  QBuffer *imgbuffer = findChild<QBuffer *> ();

  if(imgbuffer)
    {
      if(imgbuffer->property("which") == "front")
	{
	  if(imgbuffer->data().size() > 1000)
	    {
	      id.front_image->clear();
	      id.front_image->loadFromData(imgbuffer->data());
	    }

	  if(imgbuffer->data().size() < 1000)
	    {
	      imgbuffer->deleteLater();
	      QMessageBox::warning
		(this, tr("BiblioteQ: HTTP Warning"),
		 tr("The front cover image for the specified "
		    "ISBN may not exist."));
	    }
	  else
	    imgbuffer->deleteLater();
	}
      else
	{
	  if(imgbuffer->data().size() > 1000)
	    {
	      id.back_image->clear();
	      id.back_image->loadFromData(imgbuffer->data());
	    }

	  if(imgbuffer->data().size() < 1000)
	    {
	      imgbuffer->deleteLater();
	      QMessageBox::warning
		(this, tr("BiblioteQ: HTTP Warning"),
		 tr("The back cover image for the specified ISBN "
		    "may not exist."));
	    }
	  else
	    imgbuffer->deleteLater();
	}
    }
}

/*
** -- slotDataTransferProgress() --
*/

void qtbook_book::slotDataTransferProgress(qint64 bytesread,
					   qint64 totalbytes)
{
  if(httpProgress->isVisible())
    {
      httpProgress->setMaximum(totalbytes);
      httpProgress->setValue(bytesread);
    }
}

/*
** -- slotCancelImageDownload() --
*/

void qtbook_book::slotCancelImageDownload(void)
{
  QBuffer *imgbuffer = findChild<QBuffer *> ();

  if(imgbuffer)
    imgbuffer->deleteLater();

  QNetworkAccessManager *manager = findChild<QNetworkAccessManager *> ();

  if(manager)
    manager->deleteLater();
}

/*
** -- slotReadyRead() --
*/

void qtbook_book::slotReadyRead(void)
{
  QBuffer *imgbuffer = findChild<QBuffer *> ();
  QNetworkAccessManager *manager = findChild<QNetworkAccessManager *> ();

  if(manager)
    {
      QNetworkReply *reply = manager->findChild<QNetworkReply *> ();

      if(reply && imgbuffer)
	imgbuffer->write(reply->readAll());
    }
}
