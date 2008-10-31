/*
** Includes video game-specific methods.
*/

/*
** -- Local Includes --
*/

#include "qtbook_videogame.h"

extern qtbook *qmain;
extern QApplication *qapp;

/*
** -- qtbook_videogame() --
*/

qtbook_videogame::qtbook_videogame(QMainWindow *parentArg,
				   const QStringList &ratings,
				   const QStringList &platforms,
				   const QStringList &languages,
				   const QStringList &monetary_units,
				   const QStringList &locations,
				   const QString &oidArg,
				   const int rowArg):
  QMainWindow()
{
  QMenu *menu = 0;
  QRegExp rx("[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]"
	     "[0-9][0-9]");
  QValidator *validator1 = 0;
  QGraphicsScene *scene1 = 0;
  QGraphicsScene *scene2 = 0;

  if((menu = new(std::nothrow) QMenu()) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((validator1 = new(std::nothrow) QRegExpValidator(rx, this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene1 = new(std::nothrow) QGraphicsScene()) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene2 = new(std::nothrow) QGraphicsScene()) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  oid = oidArg;
  row = rowArg;
  isQueryEnabled = false;
  parentWid = parentArg;
  oldq = misc_functions::getColumnString
    (qmain->getUI().table, row, "Quantity").toInt();
  vg.setupUi(this);
  updateFont(qapp->font(), static_cast<QWidget *> (this));
  connect(vg.okButton, SIGNAL(clicked(void)), this, SLOT(slotGo(void)));
  connect(vg.showUserButton, SIGNAL(clicked(void)), this,
	  SLOT(slotShowUsers(void)));
  connect(vg.queryButton, SIGNAL(clicked(void)), this,
	  SLOT(slotQuery(void)));
  connect(vg.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCancel(void)));
  connect(vg.copiesButton, SIGNAL(clicked()), this,
	  SLOT(slotPopulateCopiesEditor(void)));
  connect(vg.resetButton, SIGNAL(clicked(void)), this,
	  SLOT(slotReset(void)));
  connect(vg.printButton, SIGNAL(clicked(void)), this, SLOT(slotPrint(void)));
  connect(menu->addAction("Reset &Front Cover Image"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Back Cover Image"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &UPC"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Game Rating"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Developer(s)"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Platform"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Mode"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Title"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Release Date"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Publisher"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Genres"),
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
  connect(vg.frontButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(vg.backButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  vg.id->setValidator(validator1);
  vg.resetButton->setMenu(menu);
  vg.rating->addItems(ratings);
  vg.platform->addItems(platforms);
  vg.language->addItems(languages);
  vg.monetary_units->addItems(monetary_units);
  vg.location->addItems(locations);
  vg.queryButton->setVisible(isQueryEnabled);
  vg.front_image->setScene(scene1);
  vg.back_image->setScene(scene2);

  if(vg.platform->count() == 0)
    vg.platform->addItem("UNKNOWN");

  if(vg.rating->count() == 0)
    vg.rating->addItem("UNKNOWN");

  if(vg.language->count() == 0)
    vg.language->addItem("UNKNOWN");

  if(vg.monetary_units->count() == 0)
    vg.monetary_units->addItem("UNKNOWN");

  if(vg.location->count() == 0)
    vg.location->addItem("UNKNOWN");

  resize(baseSize());
  misc_functions::center(this, parentWid);
  misc_functions::hideAdminFields(this, qmain->getRoles());
}

/*
** -- ~qtbook_videogame() --
*/

qtbook_videogame::~qtbook_videogame()
{
}

/*
** -- slotGo() --
*/

void qtbook_videogame::slotGo(void)
{
  int i = 0;
  int newq = 0;
  int maxcopynumber = 0;
  QString str = "";
  QString errorstr = "";
  QString searchstr = "";
  QSqlQuery query(qmain->getDB());
  QTableWidgetItem *column = 0;

  if(windowTitle().contains("Create") ||
     windowTitle().contains("Modify"))
    {
      if(windowTitle().contains("Modify") && row > -1)
	{
	  newq = vg.quantity->value();
	  qapp->setOverrideCursor(Qt::WaitCursor);
	  maxcopynumber = misc_functions::getMaxCopyNumber
	    (qmain->getDB(), oid, "Video Game", errorstr);

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
	      vg.quantity->setValue(oldq);
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
      str = vg.id->text().trimmed();
      vg.id->setText(str);

      if(vg.id->text().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the UPC field.");
	  vg.id->setFocus();
	  goto db_rollback;
	}

      str = vg.developer->toPlainText().trimmed();
      vg.developer->setMultipleLinks("videogame_search", "developer", str);

      if(vg.developer->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Developer(s) field.");
	  vg.developer->setFocus();
	  goto db_rollback;
	}

      str = vg.title->text().trimmed();
      vg.title->setText(str);

      if(vg.title->text().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Title field.");
	  vg.title->setFocus();
	  goto db_rollback;
	}

      str = vg.publisher->toPlainText().trimmed();
      vg.publisher->setPlainText(str);

      if(vg.publisher->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Publisher field.");
	  vg.publisher->setFocus();
	  goto db_rollback;
	}

      str = vg.genre->toPlainText().trimmed();
      vg.genre->setPlainText(str);

      if(vg.genre->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Genres field.");
	  vg.genre->setFocus();
	  goto db_rollback;
	}

      str = vg.description->toPlainText().trimmed();
      vg.description->setPlainText(str);

      if(vg.description->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Abstract field.");
	  vg.description->setFocus();
	  goto db_rollback;
	}

      str = vg.url->toPlainText().trimmed();
      vg.url->setPlainText(str);

      if(windowTitle().contains("Modify"))
	query.prepare(QString("UPDATE videogame SET id = ?, "
			      "title = ?, "
			      "vgrating = ?, developer = ?, "
			      "rdate = ?, "
			      "publisher = ?, "
			      "genre = ?, price = ?, "
			      "description = ?, "
			      "language = ?, "
			      "monetary_units = ?, "
			      "quantity = ?, "
			      "vgplatform = ?, "
			      "location = ?, "
			      "vgmode = ?, "
			      "front_cover = ?, "
			      "back_cover = ?, "
			      "offsystem_url = ? "
			      "WHERE "
			      "myoid = ?"));
      else if(qmain->getDB().driverName() != "QSQLITE")
	query.prepare(QString("INSERT INTO videogame (id, title, "
			      "vgrating, developer, rdate, publisher, "
			      "genre, price, description, language, "
			      "monetary_units, quantity, "
			      "vgplatform, location, vgmode, "
			      "front_cover, "
			      "back_cover, "
			      "offsystem_url) "
			      "VALUES (?, ?, "
			      "?, ?, ?, ?, "
			      "?, ?, ?, "
			      "?, ?, ?, ?, ?, ?, ?, ?, ?)"));
      else
	query.prepare(QString("INSERT INTO videogame (id, title, "
			      "vgrating, developer, rdate, publisher, "
			      "genre, price, description, language, "
			      "monetary_units, quantity, "
			      "vgplatform, location, vgmode, "
			      "front_cover, "
			      "back_cover, "
			      "offsystem_url, myoid) "
			      "VALUES (?, ?, "
			      "?, ?, ?, ?, "
			      "?, ?, ?, "
			      "?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"));

      query.bindValue(0, vg.id->text());
      query.bindValue(1, vg.title->text());
      query.bindValue(2, vg.rating->currentText().trimmed());
      query.bindValue(3, vg.developer->toPlainText());
      query.bindValue(4, vg.release_date->date().toString("MM/dd/yyyy"));
      query.bindValue(5, vg.publisher->toPlainText());
      query.bindValue(6, vg.genre->toPlainText().trimmed());
      query.bindValue(7, vg.price->text());
      query.bindValue(8, vg.description->toPlainText());
      query.bindValue(9, vg.language->currentText().trimmed());
      query.bindValue(10, vg.monetary_units->currentText().trimmed());
      query.bindValue(11, vg.quantity->text());
      query.bindValue(12, vg.platform->currentText().trimmed());
      query.bindValue(13, vg.location->currentText().trimmed());
      query.bindValue(14, vg.mode->currentText().trimmed());

      if(!vg.front_image->image.isNull())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);
	  buffer.open(QIODevice::WriteOnly);
	  vg.front_image->image.save
	    (&buffer, vg.front_image->imageFormat.toAscii(), 100);
	  query.bindValue(15, bytes);
	}
      else
	{
	  vg.front_image->imageFormat = "";
	  query.bindValue(15, QVariant());
	}

      if(!vg.back_image->image.isNull())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);
	  buffer.open(QIODevice::WriteOnly);
	  vg.back_image->image.save
	    (&buffer, vg.back_image->imageFormat.toAscii(), 100);
	  query.bindValue(16, bytes);
	}
      else
	{
	  vg.back_image->imageFormat = "";
	  query.bindValue(16, QVariant());
	}

      if(!vg.url->toPlainText().isEmpty())
	query.bindValue(17, vg.url->toPlainText());
      else
	query.bindValue(17, QVariant(QVariant::String));

      if(windowTitle().contains("Modify"))
	query.bindValue(18, oid);
      else if(qmain->getDB().driverName() == "QSQLITE")
	query.bindValue(18, vg.id->text());

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
	      query.prepare(QString("DELETE FROM videogame_copy_info WHERE "
				    "copy_number > ? AND "
				    "item_oid = "
				    "?"));
	      query.bindValue(0, vg.quantity->text());
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

	      misc_functions::createInitialCopies(vg.id->text(),
						  vg.quantity->value(),
						  qmain->getDB(),
						  "Video Game",
						  errorstr);

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

	  oldq = vg.quantity->value();

	  if(vg.front_image->image.isNull())
	    vg.front_image->imageFormat = "";

	  if(vg.back_image->image.isNull())
	    vg.back_image->imageFormat = "";

	  vg.publisher->setText
	    (QString("<a href=\"videogame_search?publisher?%1\">" +
		     vg.publisher->toPlainText() + "</a>").arg
	     (vg.publisher->toPlainText()));

	  if(!vg.url->toPlainText().isEmpty())
	    vg.url->setText(QString("<a href=\"%1\">%1</a>").arg
			    (vg.url->toPlainText()));

	  qapp->restoreOverrideCursor();

	  if(windowTitle().contains("Modify"))
	    {
	      str = QString("BiblioteQ: Modify Video Game Entry (%1)").arg
		(vg.id->text());
	      setWindowTitle(str);

	      if((qmain->getUI().typefilter->currentText() == "All" ||
		  qmain->getUI().typefilter->currentText() == "Video Games") &&
		 oid == misc_functions::getColumnString(qmain->getUI().table,
							row, "MYOID"))
		{
		  for(i = 0; i < qmain->getUI().table->columnCount(); i++)
		    {
		      column = qmain->getUI().table->horizontalHeaderItem(i);

		      if(column == 0)
			continue;

		      if(column->text() == "UPC" ||
			 column->text() == "ID Number")
			qmain->getUI().table->item(row, i)->setText
			  (vg.id->text());
		      else if(column->text() == "Title")
			qmain->getUI().table->item(row, i)->setText
			  (vg.title->text());
		      else if(column->text() == "Game Rating")
			qmain->getUI().table->item(row, i)->setText
			  (vg.rating->currentText().trimmed());
		      else if(column->text() == "Release Date" ||
			      column->text() == "Publication Date")
			qmain->getUI().table->item(row, i)->setText
			  (vg.release_date->date().toString("MM/dd/yyyy"));
		      else if(column->text() == "Publisher")
			qmain->getUI().table->item(row, i)->setText
			  (vg.publisher->toPlainText());
		      else if(column->text() == "Genres" ||
			      column->text() == "Categories")
			qmain->getUI().table->item(row, i)->setText
			  (vg.genre->toPlainText().trimmed());
		      else if(column->text() == "Price")
			qmain->getUI().table->item(row, i)->setText
			  (vg.price->text());
		      else if(column->text() == "Language")
			qmain->getUI().table->item(row, i)->setText
			  (vg.language->currentText().trimmed());
		      else if(column->text() == "Monetary Units")
			qmain->getUI().table->item(row, i)->setText
			  (vg.monetary_units->currentText().trimmed());
		      else if(column->text() == "Quantity")
			qmain->getUI().table->item(row, i)->setText
			  (vg.quantity->text());
		      else if(column->text() == "Platform")
			qmain->getUI().table->item(row, i)->setText
			  (vg.platform->currentText().trimmed());
		      else if(column->text() == "Location")
			qmain->getUI().table->item(row, i)->setText
			  (vg.location->currentText().trimmed());
		      else if(column->text() == "Mode")
			qmain->getUI().table->item(row, i)->setText
			  (vg.mode->currentText().trimmed());
		      else if(column->text() == "Availability")
			{
			  qmain->getUI().table->item(row, i)->setText
			    (misc_functions::getAvailability
			     (oid, qmain->getDB(), "Video Game",
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
	      oid = misc_functions::getOID(vg.id->text(),
					   "Video Game",
					   qmain->getDB(),
					   errorstr);
	      qapp->restoreOverrideCursor();

	      if(!errorstr.isEmpty())
		{
		  oid = "insert";
		  qmain->addError(QString("Database Error"),
				  QString("Unable to retrieve the "
					  "video game's OID."),
				  errorstr, __FILE__, __LINE__);
		  QMessageBox::critical(this, "BiblioteQ: Database Error",
					"Unable to retrieve the "
					"video game's OID.");
		}
	      else
		qmain->replaceVideoGame("insert", this);

	      updateWindow(qtbook::EDITABLE);

	      if(qmain->getUI().actionAutoPopulateOnCreation->isChecked())
		(void) qmain->populateTable
		  (qtbook::POPULATE_ALL, "Video Games", QString(""));

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
      searchstr = "SELECT videogame.title, "
	"videogame.vgrating, "
	"videogame.vgplatform, "
	"videogame.vgmode, "
	"videogame.publisher, videogame.rdate, "
	"videogame.genre, videogame.language, videogame.id, "
	"videogame.price, videogame.monetary_units, "
	"videogame.quantity, "
	"videogame.location, "
	"videogame.quantity - COUNT(item_borrower_vw.item_oid) "
	"AS availability, "
	"videogame.type, "
	"videogame.myoid "
	"FROM "
	"videogame LEFT JOIN item_borrower_vw ON "
	"videogame.myoid = item_borrower_vw.item_oid "
	"AND item_borrower_vw.type = 'Video Game' "
	"WHERE ";
      searchstr.append("id LIKE '%" + vg.id->text() + "%' AND ");
      searchstr.append("LOWER(title) LIKE '%" +
		       myqstring::escape(vg.title->text().toLower()) +
		       "%' AND ");

      if(vg.rating->currentText() != "Any")
	searchstr.append("vgrating = '" +
			 myqstring::escape(vg.rating->currentText()) +
			 "' AND ");

      searchstr.append("LOWER(developer) LIKE '%" +
		       myqstring::escape(vg.developer->toPlainText().
					 toLower()) + "%' "
		       "AND ");

      if(vg.release_date->date().toString
	 ("MM/dd/yyyy") != "01/01/7999")
	searchstr.append("rdate = '" +
			 vg.release_date->date().toString
			 ("MM/dd/yyyy") +
			 "' AND ");

      searchstr.append("LOWER(publisher) LIKE '%" +
		       myqstring::escape
		       (vg.publisher->toPlainText().toLower()) +
		       "%' AND ");
      searchstr.append("LOWER(genre) LIKE '%" +
		       myqstring::escape(vg.genre->toPlainText().toLower()) +
		       "%' AND ");

      if(vg.price->value() > -0.01)
	{
	  searchstr.append("price = ");
	  searchstr.append(vg.price->text());
	  searchstr.append(" AND ");
	}

      if(vg.language->currentText() != "Any")
	searchstr.append("language = '" +
			 myqstring::escape(vg.language->currentText()) +
			 "' AND ");

      if(vg.monetary_units->currentText() != "Any")
	searchstr.append("monetary_units = '" +
			 myqstring::escape
			 (vg.monetary_units->currentText()) +
			 "' AND ");

      if(vg.platform->currentText() != "Any")
	searchstr.append("vgplatform = '" +
			 myqstring::escape(vg.rating->currentText()) +
			 "' AND ");

      searchstr.append("LOWER(description) LIKE '%" +
		       myqstring::escape
		       (vg.description->toPlainText().toLower()) + "%' ");

      if(vg.quantity->value() != 0)
	searchstr.append("AND quantity = " + vg.quantity->text() + " ");

      if(vg.location->currentText() != "Any")
	searchstr.append("AND location = '" +
			 myqstring::escape
			 (vg.location->currentText()) + "' ");

      if(vg.mode->currentText() != "Any")
	searchstr.append("AND vgmode = '" +
			 myqstring::escape
			 (vg.mode->currentText()) + "' ");

      if(!vg.url->toPlainText().isEmpty())
	searchstr.append(" AND LOWER(COALESCE(offsystem_url, '')) LIKE '%" +
			 myqstring::escape
			 (vg.url->toPlainText()) + "%' ");

      hide();

      /*
      ** Search the database.
      */

      (void) qmain->populateTable
	(qtbook::POPULATE_SEARCH, "Video Games", searchstr);
      slotCancel();
    }
}

/*
** -- search() --
*/

void qtbook_videogame::search(const QString &field, const QString &value)
{
  vg.coverImages->setVisible(false);
  vg.id->clear();
  vg.developer->clear();
  vg.title->clear();
  vg.publisher->clear();
  vg.description->clear();
  vg.genre->clear();
  vg.copiesButton->setVisible(false);
  vg.showUserButton->setVisible(false);
  vg.queryButton->setVisible(false);
  vg.okButton->setText("&Search");
  vg.release_date->setDate(QDate::fromString("01/01/7999",
					     "MM/dd/yyyy"));
  vg.price->setMinimum(-0.01);
  vg.price->setValue(-0.01);
  vg.quantity->setMinimum(0);
  vg.quantity->setValue(0);

  /*
  ** Add "any".
  */

  if(vg.rating->findText("Any") == -1)
    vg.rating->insertItem(0, "Any");

  if(vg.language->findText("Any") == -1)
    vg.language->insertItem(0, "Any");

  if(vg.monetary_units->findText("Any") == -1)
    vg.monetary_units->insertItem(0, "Any");

  if(vg.platform->findText("Any") == -1)
    vg.platform->insertItem(0, "Any");

  if(vg.location->findText("Any") == -1)
    vg.location->insertItem(0, "Any");

  if(vg.mode->findText("Any") == -1)
    vg.mode->insertItem(0, "Any");

  vg.location->setCurrentIndex(0);
  vg.rating->setCurrentIndex(0);
  vg.language->setCurrentIndex(0);
  vg.monetary_units->setCurrentIndex(0);
  vg.platform->setCurrentIndex(0);
  vg.mode->setCurrentIndex(0);
  vg.url->clear();

  if(field.isEmpty() && value.isEmpty())
    {
      foreach(QAction *action,
	      vg.resetButton->menu()->findChildren<QAction *>())
	if(action->text().contains("Cover Image"))
	  action->setVisible(false);

      setWindowTitle("BiblioteQ: Database Video Game Search");
      vg.id->setFocus();
      misc_functions::center(this, parentWid);
      show();
    }
  else
    {
      if(field == "developer")
	vg.developer->setPlainText(value);
      else if(field == "publisher")
	vg.publisher->setPlainText(value);

      slotGo();
    }
}

/*
** -- updateWindow() --
*/

void qtbook_videogame::updateWindow(const int state)
{
  QString str = "";

  if(state == qtbook::EDITABLE)
    {
      vg.copiesButton->setEnabled(true);
      vg.showUserButton->setEnabled(true);
      vg.okButton->setVisible(true);
      vg.queryButton->setVisible(isQueryEnabled);
      vg.resetButton->setVisible(true);
      vg.frontButton->setVisible(true);
      vg.backButton->setVisible(true);
      str = QString("BiblioteQ: Modify Video Game Entry (%1)").arg
	(vg.id->text());
    }
  else
    {
      vg.copiesButton->setVisible(false);
      vg.showUserButton->setEnabled(true);
      vg.okButton->setVisible(false);
      vg.queryButton->setVisible(false);
      vg.resetButton->setVisible(false);
      vg.frontButton->setVisible(false);
      vg.backButton->setVisible(false);
      str = QString("BiblioteQ: View Video Game Details (%1)").arg
	(vg.id->text());
    }

  vg.coverImages->setVisible(true);
  setWindowTitle(str);
}

/*
** -- modify() --
*/

void qtbook_videogame::modify(const int state)
{
  int i = 0;
  QString str = "";
  QString fieldname = "";
  QString searchstr = "";
  QVariant var;
  QSqlQuery query(qmain->getDB());

  if(state == qtbook::EDITABLE)
    {
      setWindowTitle("BiblioteQ: Modify Video Game Entry");
      vg.copiesButton->setEnabled(true);
      vg.showUserButton->setEnabled(true);
      vg.okButton->setVisible(true);
      vg.queryButton->setVisible(isQueryEnabled);
      vg.resetButton->setVisible(true);
      vg.frontButton->setVisible(true);
      vg.backButton->setVisible(true);
      misc_functions::highlightWidget
	(vg.id, QColor(255, 248, 220));
      misc_functions::highlightWidget
	(vg.title, QColor(255, 248, 220));
      misc_functions::highlightWidget
	(vg.publisher->viewport(), QColor(255, 248, 220));
      misc_functions::highlightWidget
	(vg.developer->viewport(), QColor(255, 248, 220));
      misc_functions::highlightWidget
	(vg.description->viewport(), QColor(255, 248, 220));
      misc_functions::highlightWidget
	(vg.genre->viewport(), QColor(255, 248, 220));
    }
  else
    {
      setWindowTitle("BiblioteQ: View Video Game Details");
      vg.copiesButton->setVisible(false);
      vg.showUserButton->setEnabled(true);
      vg.okButton->setVisible(false);
      vg.queryButton->setVisible(false);
      vg.resetButton->setVisible(false);     
      vg.frontButton->setVisible(false);
      vg.backButton->setVisible(false);

      foreach(QAction *action,
	      vg.resetButton->menu()->findChildren<QAction *>())
	if(action->text().contains("Cover Image"))
	  action->setVisible(false);
    }

  vg.quantity->setMinimum(1);
  vg.queryButton->setEnabled(true);
  vg.price->setMinimum(0.00);
  vg.okButton->setText("&Save");
  str = oid;
  searchstr = "SELECT title, "
    "vgrating, "
    "vgplatform, "
    "vgmode, "
    "developer, "
    "publisher, rdate, "
    "genre, language, id, "
    "price, monetary_units, quantity, "
    "location, description, "
    "front_cover, "
    "back_cover, "
    "offsystem_url "
    "FROM "
    "videogame "
    "WHERE myoid = ";
  searchstr.append(str);
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(searchstr) || !query.next())
    {
      qapp->restoreOverrideCursor();
      qmain->addError(QString("Database Error"),
		      QString("Unable to retrieve the selected video "
			      "game's data."),
		      query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(this, "BiblioteQ: Database Error",
			    "Unable to retrieve the selected video "
			    "game's data.");
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
	    vg.title->setText(var.toString());
	  else if(fieldname == "developer")
	    vg.developer->setMultipleLinks("videogame_search", "developer",
					   var.toString());
	  else if(fieldname == "publisher")
	    vg.publisher->setText
	      (QString("<a href=\"videogame_search?publisher?%1\">" +
		       var.toString() + "</a>").arg(var.toString()));
	  else if(fieldname == "rdate")
	    vg.release_date->setDate
	      (QDate::fromString(var.toString(), "MM/dd/yyyy"));
	  else if(fieldname == "vgrating")
	    {
	      if(vg.rating->findText(var.toString()) > -1)
		vg.rating->setCurrentIndex
		  (vg.rating->findText(var.toString()));
	      else
		vg.rating->setCurrentIndex(0);
	    }
	  else if(fieldname == "price")
	    vg.price->setValue(var.toDouble());
	  else if(fieldname == "genre")
	    vg.genre->setMultipleLinks("videogame_search", "genre",
				       var.toString());
	  else if(fieldname == "language")
	    {
	      if(vg.language->findText(var.toString()) > -1)
		vg.language->setCurrentIndex
		  (vg.language->findText(var.toString()));
	      else
		vg.language->setCurrentIndex
		  (vg.language->findText("UNKNOWN"));
	    }
	  else if(fieldname == "quantity")
	    vg.quantity->setValue(var.toInt());
	  else if(fieldname == "monetary_units")
	    {
	      if(vg.monetary_units->findText(var.toString()) > -1)
		vg.monetary_units->setCurrentIndex
		  (vg.monetary_units->findText(var.toString()));
	      else
		vg.monetary_units->setCurrentIndex
		  (vg.monetary_units->findText("UNKNOWN"));
	    }
	  else if(fieldname == "vgplatform")
	    {
	      if(vg.platform->findText(var.toString()) > -1)
		vg.platform->setCurrentIndex
		  (vg.platform->findText(var.toString()));
	      else
		vg.platform->setCurrentIndex(0);
	    }
	  else if(fieldname == "location")
	    {
	      if(vg.location->findText(var.toString()) > -1)
		vg.location->setCurrentIndex
		  (vg.location->findText(var.toString()));
	      else
		vg.location->setCurrentIndex
		  (vg.location->findText("UNKNOWN"));
	    }
	  else if(fieldname == "id")
	    {
	      if(state == qtbook::EDITABLE)
		str = QString("BiblioteQ: Modify Video Game Entry (%1)").arg
		  (var.toString());
	      else
		str = QString("BiblioteQ: View Video Game Details (%1)").arg
		  (var.toString());

	      vg.id->setText(var.toString());
	      setWindowTitle(str);
	    }
	  else if(fieldname == "description")
	    vg.description->setPlainText(var.toString());
	  else if(fieldname == "vgmode")
	    {
	      if(vg.mode->findText(var.toString()) > -1)
		vg.mode->setCurrentIndex
		  (vg.mode->findText(var.toString()));
	      else
		vg.mode->setCurrentIndex
		  (vg.mode->findText("UNKNOWN"));
	    }
	  else if(fieldname == "front_cover")
	    {
	      if(!query.record().field(i).isNull())
		vg.front_image->loadFromData(var.toByteArray());
	    }
	  else if(fieldname == "back_cover")
	    {
	      if(!query.record().field(i).isNull())
		vg.back_image->image.loadFromData
		  (var.toByteArray());
	    }
	  else if(fieldname == "offsystem_url")
	    {
	      if(!query.record().field(i).isNull())
		vg.url->setText(QString("<a href=\"%1\">%1</a>").arg
				(var.toString()));
	    }
	}

      foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
	textfield->setCursorPosition(0);

      storeData(this);
    }

  vg.id->setFocus();
  raise();
}

/*
** -- insert() --
*/

void qtbook_videogame::insert(void)
{
  slotReset();
  vg.copiesButton->setEnabled(false);
  vg.queryButton->setEnabled(true);
  vg.okButton->setText("&Save");
  vg.release_date->setDate(QDate::fromString("01/01/2000",
					     "MM/dd/yyyy"));
  vg.price->setMinimum(0.00);
  vg.price->setValue(0.00);
  vg.quantity->setMinimum(1);
  vg.quantity->setValue(1);
  vg.genre->clear();
  vg.showUserButton->setEnabled(false);
  vg.location->setCurrentIndex(0);
  vg.mode->setCurrentIndex(0);
  vg.language->setCurrentIndex(0);
  vg.monetary_units->setCurrentIndex(0);
  vg.rating->setCurrentIndex(0);
  vg.url->clear();
  misc_functions::highlightWidget
    (vg.id, QColor(255, 248, 220));
  misc_functions::highlightWidget
    (vg.title, QColor(255, 248, 220));
  misc_functions::highlightWidget
    (vg.publisher->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (vg.developer->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (vg.description->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (vg.genre->viewport(), QColor(255, 248, 220));
  setWindowTitle("BiblioteQ: Create Video Game Entry");
  vg.id->setFocus();
  storeData(this);
  misc_functions::center(this, parentWid);
  show();
}

/*
** -- slotReset() --
*/

void qtbook_videogame::slotReset(void)
{
  QAction *action = qobject_cast<QAction *> (sender());
  QString name = "";

  if(action != 0)
    {
      name = action->text();

      if(name.contains("Front Cover Image"))
	vg.front_image->clear();
      else if(name.contains("Back Cover Image"))
	vg.back_image->clear();
      else if(name.contains("UPC"))
	{
	  vg.id->clear();
	  vg.id->setFocus();
	}
      else if(name.contains("Title"))
	{
	  vg.title->clear();
	  vg.title->setFocus();
	}
      else if(name.contains("Rating"))
	{
	  vg.rating->setCurrentIndex(0);
	  vg.rating->setFocus();
	}
      else if(name.contains("Developer(s)"))
	{
	  vg.developer->clear();
	  vg.developer->setFocus();
	}
      else if(name.contains("Release Date"))
	{
	  if(windowTitle().contains("Search"))
	    vg.release_date->setDate
	      (QDate::fromString("01/01/7999", "MM/dd/yyyy"));
	  else
	    vg.release_date->setDate
	      (QDate::fromString("01/01/2000", "MM/dd/yyyy"));

	  vg.release_date->setFocus();
	}
      else if(name.contains("Publisher"))
	{
	  vg.publisher->clear();
	  vg.publisher->setFocus();
	}
      else if(name.contains("Genre"))
	{
	  vg.genre->clear();
	  vg.genre->setFocus();
	}
      else if(name.contains("Price"))
	{
	  vg.price->setValue(vg.price->minimum());
	  vg.price->setFocus();
	}
      else if(name.contains("Language"))
	{
	  vg.language->setCurrentIndex(0);
	  vg.language->setFocus();
	}
      else if(name.contains("Monetary Units"))
	{
	  vg.monetary_units->setCurrentIndex(0);
	  vg.monetary_units->setFocus();
	}
      else if(name.contains("Game Rating"))
	{
	  vg.rating->setCurrentIndex(0);
	  vg.rating->setFocus();
	}
      else if(name.contains("Abstract"))
	{
	  vg.description->clear();
	  vg.description->setFocus();
	}
      else if(name.contains("Copies"))
	{
	  vg.quantity->setValue(vg.quantity->minimum());
	  vg.quantity->setFocus();
	}
      else if(name.contains("Location"))
	{
	  vg.location->setCurrentIndex(0);
	  vg.location->setFocus();
	}
      else if(name.contains("Platform"))
	{
	  vg.platform->setCurrentIndex(0);
	  vg.platform->setFocus();
	}
      else if(name.contains("Mode"))
	{
	  vg.mode->setCurrentIndex(0);
	  vg.mode->setFocus();
	}
      else if(name.contains("OFFSYSTEM URL"))
	{
	  vg.url->clear();
	  vg.url->setFocus();
	}
    }
  else
    {
      /*
      ** Reset all.
      */

      vg.id->clear();
      vg.title->clear();
      vg.developer->clear();
      vg.publisher->clear();
      vg.genre->clear();

      if(windowTitle().contains("Search"))
	vg.release_date->setDate(QDate::fromString("01/01/7999",
						   "MM/dd/yyyy"));
      else
	vg.release_date->setDate(QDate::fromString("01/01/2000",
						   "MM/dd/yyyy"));

      vg.quantity->setValue(vg.quantity->minimum());
      vg.description->clear();
      vg.location->setCurrentIndex(0);
      vg.rating->setCurrentIndex(0);
      vg.price->setValue(vg.price->minimum());
      vg.language->setCurrentIndex(0);
      vg.monetary_units->setCurrentIndex(0);
      vg.platform->setCurrentIndex(0);
      vg.mode->setCurrentIndex(0);
      vg.url->clear();
      vg.front_image->clear();
      vg.back_image->clear();
      vg.id->setFocus();
    }
}

/*
** -- closeEvent() --
*/

void qtbook_videogame::closeEvent(QCloseEvent *e)
{
  if(windowTitle().contains("Create") || windowTitle().contains("Modify"))
    if(hasDataChanged(this))
      if(QMessageBox::question(this, "BiblioteQ: Question",
			       "You have unsaved data. Continue closing?",
			       QMessageBox::Yes | QMessageBox::No,
			       QMessageBox::No) == QMessageBox::No)
	{
	  e->ignore();
	  return;
	}

  qmain->removeVideoGame(this);
}

/*
** -- slotCancel() --
*/

void qtbook_videogame::slotCancel(void)
{
  close();
}

/*
** -- slotPopulateCopiesEditor() --
*/

void qtbook_videogame::slotPopulateCopiesEditor(void)
{
  copy_editor *copyeditor = 0;

  if((copyeditor = new(std::nothrow) copy_editor
      (static_cast<QWidget *> (this),
       static_cast<qtbook_item *> (this),
       false,
       vg.quantity->value(), oid,
       vg.id->text(),
       vg.quantity, font(), "Video Game")) != 0)
    copyeditor->populateCopiesEditor();
}

/*
** -- slotShowUsers() --
*/

void qtbook_videogame::slotShowUsers(void)
{
  int state = 0;
  borrowers_editor *borrowerseditor = 0;

  if(!vg.okButton->isHidden())
    state = qtbook::EDITABLE;
  else
    state = qtbook::VIEW_ONLY;

  if((borrowerseditor = new(std::nothrow) borrowers_editor
      (static_cast<QWidget *> (this), static_cast<qtbook_item *> (this),
       vg.quantity->value(), oid, vg.id->text(), font(),
       "Video Game", state)) != 0)
    borrowerseditor->showUsers();
}

/*
** -- slotQuery() --
*/

void qtbook_videogame::slotQuery(void)
{
}

/*
** -- slotPrint() --
*/

void qtbook_videogame::slotPrint(void)
{
  html = "";
  html += "<b>UPC:</b> " + vg.id->text().trimmed() + "<br>";
  html += "<b>Game Rating:</b> " + vg.rating->currentText() + "<br>";
  html += "<b>Developer(s):</b> " + vg.developer->toPlainText().trimmed() +
    "<br>";
  html += "<b>Platform:</b> " + vg.platform->currentText() + "<br>";
  html += "<b>Mode:</b> " + vg.mode->currentText() + "<br>";

  /*
  ** General information.
  */

  html += "<b>Title:</b> " + vg.title->text().trimmed() + "<br>";
  html += "<b>Release Date:</b> " + vg.release_date->date().
    toString("MM/dd/yyyy") + "<br>";
  html += "<b>Publisher:</b> " + vg.publisher->toPlainText().trimmed() +
    "<br>";
  html += "<b>Genre:</b> " + vg.genre->toPlainText().trimmed() + "<br>";
  html += "<b>Price:</b> " + vg.price->text() + "<br>";
  html += "<b>Language:</b> " + vg.language->currentText() + "<br>";
  html += "<b>Monetary Units:</b> " + vg.monetary_units->currentText() +
    "<br>";
  html += "<b>Copies:</b> " + vg.quantity->text() + "<br>";
  html += "<b>Location:</b> " + vg.location->currentText() + "<br>";
  html += "<b>Abstract:</b> " + vg.description->toPlainText().trimmed() +
    "<br>";
  html += "<b>OFFSYSTEM URL:</b> " + vg.url->toPlainText().trimmed();
  print(this);
}

/*
** -- slotSelectImage() --
*/

void qtbook_videogame::slotSelectImage(void)
{
  QFileDialog dialog(this);
  QPushButton *button = static_cast<QPushButton *> (sender());

  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setFilter("Image Files (*.bmp *.jpg *.jpeg *.png)");

  if(button == vg.frontButton)
    dialog.setWindowTitle("Front Cover Image Selection");
  else
    dialog.setWindowTitle("Back Cover Image Selection");

  dialog.exec();

  if(dialog.result() == QDialog::Accepted)
    {
      if(button == vg.frontButton)
	{
	  vg.front_image->clear();
	  vg.front_image->image = QImage(dialog.selectedFiles().at(0));
	  vg.front_image->imageFormat = dialog.selectedFiles().at(0).mid
	    (dialog.selectedFiles().at(0).lastIndexOf(".") + 1).toUpper();
	  vg.front_image->scene()->addPixmap
	    (QPixmap().fromImage(vg.front_image->image));
	  vg.front_image->scene()->items().at(0)->setFlags
	    (QGraphicsItem::ItemIsSelectable);
	}
      else
	{
	  vg.back_image->clear();
	  vg.back_image->image = QImage(dialog.selectedFiles().at(0));
	  vg.back_image->imageFormat = dialog.selectedFiles().at(0).mid
	    (dialog.selectedFiles().at(0).lastIndexOf(".") + 1).toUpper();
	  vg.back_image->scene()->addPixmap
	    (QPixmap().fromImage(vg.back_image->image));
	  vg.back_image->scene()->items().at(0)->setFlags
	    (QGraphicsItem::ItemIsSelectable);
	}
    }
}
