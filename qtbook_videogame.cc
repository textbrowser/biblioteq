/*
** -- Qt Includes --
*/

#include <QSqlField>
#include <QSqlRecord>

/*
** Includes video game-specific methods.
*/

/*
** -- Local Includes --
*/

#include "qtbook.h"
#include "borrowers_editor.h"
#include "qtbook_videogame.h"

extern qtbook *qmain;
extern QApplication *qapp;

/*
** -- qtbook_videogame() --
*/

qtbook_videogame::qtbook_videogame(QMainWindow *parentArg,
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

  if((menu = new(std::nothrow) QMenu(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((validator1 = new(std::nothrow) QRegExpValidator(rx, this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene1 = new(std::nothrow) QGraphicsScene(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene2 = new(std::nothrow) QGraphicsScene(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  oid = oidArg;
  row = rowArg;
  isQueryEnabled = false;
  parentWid = parentArg;
  oldq = misc_functions::getColumnString
    (qmain->getUI().table, row, tr("Quantity")).toInt();
  vg.setupUi(this);
#ifdef Q_WS_MAC
  setAttribute(Qt::WA_MacMetalStyle, true);
#endif
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
  connect(menu->addAction(tr("Reset &Front Cover Image")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Back Cover Image")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &UPC")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Game Rating")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Developer(s)")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Platform")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Mode")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Title")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Release Date")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Publisher")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Place of Publication")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Genres")),
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
  connect(menu->addAction(tr("Reset &Keywords")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(vg.frontButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(vg.backButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  vg.id->setValidator(validator1);
  vg.resetButton->setMenu(menu);

  QString errorstr("");

  qapp->setOverrideCursor(Qt::WaitCursor);
  vg.rating->addItems
    (misc_functions::getVideoGameRatings(qmain->getDB(),
					 errorstr));
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the video game ratings.")),
       errorstr, __FILE__, __LINE__);

  qapp->setOverrideCursor(Qt::WaitCursor);
  vg.platform->addItems
    (misc_functions::getVideoGamePlatforms(qmain->getDB(),
					   errorstr));
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the video game platforms.")),
       errorstr, __FILE__, __LINE__);

  qapp->setOverrideCursor(Qt::WaitCursor);
  vg.language->addItems
    (misc_functions::getLanguages(qmain->getDB(),
				  errorstr));
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the languages.")),
       errorstr, __FILE__, __LINE__);

  qapp->setOverrideCursor(Qt::WaitCursor);
  vg.monetary_units->addItems
    (misc_functions::getMonetaryUnits(qmain->getDB(),
				      errorstr));
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the monetary units.")),
       errorstr, __FILE__, __LINE__);

  qapp->setOverrideCursor(Qt::WaitCursor);
  vg.location->addItems
    (misc_functions::getLocations(qmain->getDB(),
				  "Video Game",
				  errorstr));
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the video game locations.")),
       errorstr, __FILE__, __LINE__);

  vg.queryButton->setVisible(isQueryEnabled);
  vg.front_image->setScene(scene1);
  vg.back_image->setScene(scene2);

  if(vg.platform->findText(tr("UNKNOWN")) == -1)
    vg.platform->addItem(tr("UNKNOWN"));

  if(vg.rating->findText(tr("UNKNOWN")) == -1)
    vg.rating->addItem(tr("UNKNOWN"));

  if(vg.language->findText(tr("UNKNOWN")) == -1)
    vg.language->addItem(tr("UNKNOWN"));

  if(vg.monetary_units->findText(tr("UNKNOWN")) == -1)
    vg.monetary_units->addItem(tr("UNKNOWN"));

  if(vg.location->findText(tr("UNKNOWN")) == -1)
    vg.location->addItem(tr("UNKNOWN"));

  resize(0.90 * parentWid->size().width(),
	 0.90 * parentWid->size().height());
  vg.splitter->setStretchFactor(0, 0);
  vg.splitter->setStretchFactor(1, 1);
  vg.splitter->setStretchFactor(2, 0);
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

  if(engWindowTitle.contains("Create") ||
     engWindowTitle.contains("Modify"))
    {
      if(engWindowTitle.contains("Modify") && row > -1)
	{
	  newq = vg.quantity->value();
	  qapp->setOverrideCursor(Qt::WaitCursor);
	  maxcopynumber = misc_functions::getMaxCopyNumber
	    (qmain->getDB(), oid, "Video Game", errorstr);

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
		    "number of copies while there are copies "
		    "that have been reserved."));
	      vg.quantity->setValue(oldq);
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

      str = vg.id->text().trimmed();
      vg.id->setText(str);

      if(vg.id->text().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the UPC field."));
	  vg.id->setFocus();
	  return;
	}

      str = vg.developer->toPlainText().trimmed();
      vg.developer->setPlainText(str);

      if(vg.developer->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Developer(s) field."));
	  vg.developer->setFocus();
	  return;
	}

      str = vg.title->text().trimmed();
      vg.title->setText(str);

      if(vg.title->text().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Title field."));
	  vg.title->setFocus();
	  return;
	}

      str = vg.publisher->toPlainText().trimmed();
      vg.publisher->setPlainText(str);

      if(vg.publisher->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Publisher field."));
	  vg.publisher->setFocus();
	  return;
	}

      str = vg.place->toPlainText().trimmed();
      vg.place->setPlainText(str);

      if(vg.place->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Place of Publication "
				   "field."));
	  vg.place->setFocus();
	  return;
	}

      str = vg.genre->toPlainText().trimmed();
      vg.genre->setPlainText(str);

      if(vg.genre->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Genres field."));
	  vg.genre->setFocus();
	  return;
	}

      str = vg.description->toPlainText().trimmed();
      vg.description->setPlainText(str);

      if(vg.description->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Abstract field."));
	  vg.description->setFocus();
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

      str = vg.keyword->toPlainText().trimmed();
      vg.keyword->setPlainText(str);
      qapp->restoreOverrideCursor();

      if(engWindowTitle.contains("Modify"))
	query.prepare("UPDATE videogame SET id = ?, "
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
		      "place = ?, "
		      "keyword = ? "
		      "WHERE "
		      "myoid = ?");
      else if(qmain->getDB().driverName() != "QSQLITE")
	query.prepare("INSERT INTO videogame (id, title, "
		      "vgrating, developer, rdate, publisher, "
		      "genre, price, description, language, "
		      "monetary_units, quantity, "
		      "vgplatform, location, vgmode, "
		      "front_cover, "
		      "back_cover, "
		      "place, keyword) "
		      "VALUES (?, ?, ?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, ?, ?, ?, ?, ?, ?)");
      else
	query.prepare("INSERT INTO videogame (id, title, "
		      "vgrating, developer, rdate, publisher, "
		      "genre, price, description, language, "
		      "monetary_units, quantity, "
		      "vgplatform, location, vgmode, "
		      "front_cover, "
		      "back_cover, "
		      "place, keyword, myoid) "
		      "VALUES (?, ?, ?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

      query.bindValue(0, vg.id->text());
      query.bindValue(1, vg.title->text());
      query.bindValue(2, vg.rating->currentText().trimmed());
      query.bindValue(3, vg.developer->toPlainText().trimmed());
      query.bindValue(4, vg.release_date->date().toString("MM/dd/yyyy"));
      query.bindValue(5, vg.publisher->toPlainText().trimmed());
      query.bindValue(6, vg.genre->toPlainText().trimmed());
      query.bindValue(7, vg.price->value());
      query.bindValue(8, vg.description->toPlainText().trimmed());
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
	  query.bindValue(15, bytes.toBase64());
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
	  query.bindValue(16, bytes.toBase64());
	}
      else
	{
	  vg.back_image->imageFormat = "";
	  query.bindValue(16, QVariant());
	}

      query.bindValue(17, vg.place->toPlainText().trimmed());
      query.bindValue(18, vg.keyword->toPlainText().trimmed());

      if(engWindowTitle.contains("Modify"))
	query.bindValue(19, oid);
      else if(qmain->getDB().driverName() == "QSQLITE")
	{
	  qint64 value = misc_functions::getSqliteUniqueId(qmain->getDB(),
							   errorstr);

	  if(errorstr.isEmpty())
	    query.bindValue(19, value);
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
	  qmain->addError(QString(tr("Database Error")),
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
	      query.prepare(QString("DELETE FROM videogame_copy_info WHERE "
				    "copy_number > ? AND "
				    "item_oid = "
				    "?"));
	      query.bindValue(0, vg.quantity->text());
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

	      misc_functions::createInitialCopies(vg.id->text(),
						  vg.quantity->value(),
						  qmain->getDB(),
						  "Video Game",
						  errorstr);

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

	  oldq = vg.quantity->value();

	  if(vg.front_image->image.isNull())
	    vg.front_image->imageFormat = "";

	  if(vg.back_image->image.isNull())
	    vg.back_image->imageFormat = "";

	  vg.developer->setMultipleLinks("videogame_search", "developer",
					 vg.developer->toPlainText());
	  vg.publisher->setMultipleLinks("videogame_search",
					 "publisher",
					 vg.publisher->toPlainText());
	  vg.place->setMultipleLinks("videogame_search", "place",
				     vg.place->toPlainText());
	  vg.genre->setMultipleLinks("videogame_search", "genre",
				     vg.genre->toPlainText());
	  vg.keyword->setMultipleLinks("videogame_search", "keyword",
				       vg.keyword->toPlainText());
	  qapp->restoreOverrideCursor();

	  if(engWindowTitle.contains("Modify"))
	    {
	      str = QString(tr("BiblioteQ: Modify Video Game Entry (")) +
		vg.id->text() + tr(")");
	      setWindowTitle(str);
	      engWindowTitle = "Modify";

	      if((qmain->getTypeFilterString() == "All" ||
		  qmain->getTypeFilterString() == "All Overdue" ||
		  qmain->getTypeFilterString() == "All Requested" ||
		  qmain->getTypeFilterString() == "All Reserved" ||
		  qmain->getTypeFilterString() == "Video Games") &&
		 oid == misc_functions::getColumnString(qmain->getUI().table,
							row, "MYOID") &&
		 misc_functions::getColumnString(qmain->getUI().table,
						 row, tr("Type")) ==
		 "Video Game")
		{
		  qmain->getUI().table->setSortingEnabled(false);

		  for(i = 0; i < qmain->getUI().table->columnCount(); i++)
		    {
		      column = qmain->getUI().table->horizontalHeaderItem(i);

		      if(column == 0)
			continue;

		      if(column->text() == tr("UPC") ||
			 column->text() == tr("ID Number"))
			qmain->getUI().table->item(row, i)->setText
			  (vg.id->text());
		      else if(column->text() == tr("Title"))
			qmain->getUI().table->item(row, i)->setText
			  (vg.title->text());
		      else if(column->text() == tr("Game Rating"))
			qmain->getUI().table->item(row, i)->setText
			  (vg.rating->currentText().trimmed());
		      else if(column->text() == tr("Release Date") ||
			      column->text() == tr("Publication Date"))
			qmain->getUI().table->item(row, i)->setText
			  (vg.release_date->date().toString
			   (Qt::SystemLocaleShortDate));
		      else if(column->text() == tr("Publisher"))
			qmain->getUI().table->item(row, i)->setText
			  (vg.publisher->toPlainText());
		      else if(column->text() == tr("Place of Publication"))
			qmain->getUI().table->item(row, i)->setText
			  (vg.place->toPlainText());
		      else if(column->text() == tr("Genres") ||
			      column->text() == tr("Categories"))
			qmain->getUI().table->item(row, i)->setText
			  (vg.genre->toPlainText().trimmed());
		      else if(column->text() == tr("Price"))
			qmain->getUI().table->item(row, i)->setText
			  (vg.price->cleanText());
		      else if(column->text() == tr("Language"))
			qmain->getUI().table->item(row, i)->setText
			  (vg.language->currentText().trimmed());
		      else if(column->text() == tr("Monetary Units"))
			qmain->getUI().table->item(row, i)->setText
			  (vg.monetary_units->currentText().trimmed());
		      else if(column->text() == tr("Quantity"))
			qmain->getUI().table->item(row, i)->setText
			  (vg.quantity->text());
		      else if(column->text() == tr("Platform"))
			qmain->getUI().table->item(row, i)->setText
			  (vg.platform->currentText().trimmed());
		      else if(column->text() == tr("Location"))
			qmain->getUI().table->item(row, i)->setText
			  (vg.location->currentText().trimmed());
		      else if(column->text() == tr("Mode"))
			qmain->getUI().table->item(row, i)->setText
			  (vg.mode->currentText().trimmed());
		      else if(column->text() == tr("Availability"))
			{
			  qmain->getUI().table->item(row, i)->setText
			    (misc_functions::getAvailability
			     (oid, qmain->getDB(), "Video Game",
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
		  qmain->updateSceneItem(oid, vg.front_image->image);
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
		  qmain->addError(QString(tr("Database Error")),
				  QString(tr("Unable to retrieve the "
					     "video game's OID.")),
				  errorstr, __FILE__, __LINE__);
		  QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
					tr("Unable to retrieve the "
					   "video game's OID."));
		}
	      else
		qmain->replaceVideoGame(oid, this);

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
      searchstr = "SELECT videogame.title, "
	"videogame.vgrating, "
	"videogame.vgplatform, "
	"videogame.vgmode, "
	"videogame.publisher, videogame.rdate, videogame.place, "
	"videogame.genre, videogame.language, videogame.id, "
	"videogame.price, videogame.monetary_units, "
	"videogame.quantity, "
	"videogame.location, "
	"videogame.quantity - COUNT(item_borrower_vw.item_oid) "
	"AS availability, "
	"COUNT(item_borrower_vw.item_oid) AS total_reserved, "
	"videogame.type, "
	"videogame.myoid, "
	"videogame.front_cover "
	"FROM "
	"videogame LEFT JOIN item_borrower_vw ON "
	"videogame.myoid = item_borrower_vw.item_oid "
	"AND item_borrower_vw.type = 'Video Game' "
	"WHERE ";
      searchstr.append("id LIKE '%" + vg.id->text().trimmed() +
		       "%' AND ");

      QString E("");

      if(qmain->getDB().driverName() != "QSQLITE")
	E = "E";

      searchstr.append("title LIKE " + E + "'%" +
		       myqstring::escape(vg.title->text().trimmed()) +
		       "%' AND ");

      if(vg.rating->currentText() != tr("Any"))
	searchstr.append("vgrating = " + E + "'" +
			 myqstring::escape(vg.rating->currentText().
					   trimmed()) +
			 "' AND ");

      searchstr.append("developer LIKE " + E + "'%" +
		       myqstring::escape(vg.developer->toPlainText().
					 trimmed()) + "%' "
		       "AND ");

      if(vg.release_date->date().toString
	 ("MM/yyyy") != "01/7999")
	searchstr.append("SUBSTR(rdate, 1, 3) || SUBSTR(rdate, 7) = '" +
			 vg.release_date->date().toString
			 ("MM/yyyy") +
			 "' AND ");

      searchstr.append("publisher LIKE " + E + "'%" +
		       myqstring::escape
		       (vg.publisher->toPlainText().trimmed()) +
		       "%' AND ");
      searchstr.append("place LIKE " + E + "'%" +
		       myqstring::escape
		       (vg.place->toPlainText().trimmed()) +
		       "%' AND ");
      searchstr.append("genre LIKE " + E + "'%" +
		       myqstring::escape(vg.genre->toPlainText().trimmed()) +
		       "%' AND ");

      if(vg.price->value() > -0.01)
	{
	  searchstr.append("price = ");
	  searchstr.append(QString::number(vg.price->value()));
	  searchstr.append(" AND ");
	}

      if(vg.language->currentText() != tr("Any"))
	searchstr.append("language = " + E + "'" +
			 myqstring::escape(vg.language->currentText().
					   trimmed()) +
			 "' AND ");

      if(vg.monetary_units->currentText() != tr("Any"))
	searchstr.append("monetary_units = " + E + "'" +
			 myqstring::escape
			 (vg.monetary_units->currentText().trimmed()) +
			 "' AND ");

      if(vg.platform->currentText() != tr("Any"))
	searchstr.append("vgplatform = " + E + "'" +
			 myqstring::escape(vg.rating->currentText().
					   trimmed()) +
			 "' AND ");

      searchstr.append("description LIKE " + E + "'%" +
		       myqstring::escape
		       (vg.description->toPlainText().trimmed()) + "%' ");

      if(vg.quantity->value() != 0)
	searchstr.append("AND quantity = " + vg.quantity->text() + " ");

      if(vg.location->currentText() != tr("Any"))
	searchstr.append("AND location = " + E + "'" +
			 myqstring::escape
			 (vg.location->currentText().trimmed()) + "' ");

      if(vg.mode->currentText() != tr("Any"))
	searchstr.append("AND vgmode = " + E + "'" +
			 myqstring::escape
			 (vg.mode->currentText().trimmed()) + "' ");

      searchstr.append("AND COALESCE(keyword, '') LIKE " + E + "'%" +
		       myqstring::escape
		       (vg.keyword->toPlainText().trimmed()) + "%' ");

      /*
      ** Search the database.
      */

      (void) qmain->populateTable
	(qtbook::POPULATE_SEARCH, "Video Games", searchstr);
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
  vg.keyword->clear();
  vg.genre->clear();
  vg.copiesButton->setVisible(false);
  vg.showUserButton->setVisible(false);
  vg.queryButton->setVisible(false);
  vg.okButton->setText(tr("&Search"));
  vg.release_date->setDate(QDate::fromString("01/7999",
					     "MM/yyyy"));
  vg.release_date->setDisplayFormat("MM/yyyy");
  vg.price->setMinimum(-0.01);
  vg.price->setValue(-0.01);
  vg.quantity->setMinimum(0);
  vg.quantity->setValue(0);

  /*
  ** Add "any".
  */

  if(vg.rating->findText(tr("Any")) == -1)
    vg.rating->insertItem(0, tr("Any"));

  if(vg.language->findText(tr("Any")) == -1)
    vg.language->insertItem(0, tr("Any"));

  if(vg.monetary_units->findText(tr("Any")) == -1)
    vg.monetary_units->insertItem(0, tr("Any"));

  if(vg.platform->findText(tr("Any")) == -1)
    vg.platform->insertItem(0, tr("Any"));

  if(vg.location->findText(tr("Any")) == -1)
    vg.location->insertItem(0, tr("Any"));

  if(vg.mode->findText(tr("Any")) == -1)
    vg.mode->insertItem(0, tr("Any"));

  vg.location->setCurrentIndex(0);
  vg.rating->setCurrentIndex(0);
  vg.language->setCurrentIndex(0);
  vg.monetary_units->setCurrentIndex(0);
  vg.platform->setCurrentIndex(0);
  vg.mode->setCurrentIndex(0);

  if(field.isEmpty() && value.isEmpty())
    {
      QList<QAction *> actions = vg.resetButton->menu()->actions();

      actions[0]->setVisible(false);
      actions[1]->setVisible(false);
      actions.clear();
      setWindowTitle(tr("BiblioteQ: Database Video Game Search"));
      engWindowTitle = "Search";
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
      else if(field == "genre")
	vg.genre->setPlainText(value);
      else if(field == "place")
	vg.place->setPlainText(value);
      else if(field == "keyword")
	vg.keyword->setPlainText(value);

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
      str = QString(tr("BiblioteQ: Modify Video Game Entry (")) +
	vg.id->text() + tr(")");
      engWindowTitle = "Modify";
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
      str = QString(tr("BiblioteQ: View Video Game Details (")) +
	vg.id->text() + tr(")");
      engWindowTitle = "View";
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
      setWindowTitle(tr("BiblioteQ: Modify Video Game Entry"));
      engWindowTitle = "Modify";
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
      misc_functions::highlightWidget
	(vg.place->viewport(), QColor(255, 248, 220));
    }
  else
    {
      setWindowTitle(tr("BiblioteQ: View Video Game Details"));
      engWindowTitle = "View";
      vg.copiesButton->setVisible(false);
      vg.showUserButton->setEnabled(true);
      vg.okButton->setVisible(false);
      vg.queryButton->setVisible(false);
      vg.resetButton->setVisible(false);
      vg.frontButton->setVisible(false);
      vg.backButton->setVisible(false);

      QList<QAction *> actions = vg.resetButton->menu()->actions();

      actions[0]->setVisible(false);
      actions[1]->setVisible(false);
      actions.clear();
    }

  vg.quantity->setMinimum(1);
  vg.queryButton->setEnabled(true);
  vg.price->setMinimum(0.00);
  vg.okButton->setText(tr("&Save"));
  str = oid;
  searchstr = "SELECT title, "
    "vgrating, "
    "vgplatform, "
    "vgmode, "
    "developer, "
    "publisher, rdate, place, "
    "genre, language, id, "
    "price, monetary_units, quantity, "
    "location, description, "
    "front_cover, "
    "back_cover, "
    "keyword "
    "FROM "
    "videogame "
    "WHERE myoid = ";
  searchstr.append(str);
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(searchstr) || !query.next())
    {
      qapp->restoreOverrideCursor();
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to retrieve the selected video "
				 "game's data.")),
		      query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to retrieve the selected video "
			       "game's data."));
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
	    vg.publisher->setMultipleLinks
	      ("videogame_search", "publisher",
	       var.toString());
	  else if(fieldname == "rdate")
	    vg.release_date->setDate
	      (QDate::fromString(var.toString(), "MM/dd/yyyy"));
	  else if(fieldname == "vgrating")
	    {
	      if(vg.rating->findText(var.toString()) > -1)
		vg.rating->setCurrentIndex
		  (vg.rating->findText(var.toString()));
	      else
		vg.rating->setCurrentIndex
		  (vg.rating->findText(tr("UNKNOWN")));
	    }
	  else if(fieldname == "price")
	    vg.price->setValue(var.toDouble());
	  else if(fieldname == "place")
	    vg.place->setMultipleLinks("videogame_search", "place",
				       var.toString());
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
		  (vg.language->findText(tr("UNKNOWN")));
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
		  (vg.monetary_units->findText(tr("UNKNOWN")));
	    }
	  else if(fieldname == "vgplatform")
	    {
	      if(vg.platform->findText(var.toString()) > -1)
		vg.platform->setCurrentIndex
		  (vg.platform->findText(var.toString()));
	      else
		vg.platform->setCurrentIndex
		  (vg.platform->findText(tr("UNKNOWN")));
	    }
	  else if(fieldname == "location")
	    {
	      if(vg.location->findText(var.toString()) > -1)
		vg.location->setCurrentIndex
		  (vg.location->findText(var.toString()));
	      else
		vg.location->setCurrentIndex
		  (vg.location->findText(tr("UNKNOWN")));
	    }
	  else if(fieldname == "id")
	    {
	      if(state == qtbook::EDITABLE)
		{
		  str = QString(tr("BiblioteQ: Modify Video Game Entry (")) +
		    var.toString() + tr(")");
		  engWindowTitle = "Modify";
		}
	      else
		{
		  str = QString(tr("BiblioteQ: View Video Game Details (")) +
		    var.toString() + tr(")");
		  engWindowTitle = "View";
		}

	      vg.id->setText(var.toString());
	      setWindowTitle(str);
	    }
	  else if(fieldname == "description")
	    vg.description->setPlainText(var.toString());
	  else if(fieldname == "keyword")
	    vg.keyword->setMultipleLinks
	      ("videogame_search", "keyword",
	       var.toString());
	  else if(fieldname == "vgmode")
	    {
	      if(vg.mode->findText(var.toString()) > -1)
		vg.mode->setCurrentIndex
		  (vg.mode->findText(var.toString()));
	      else
		vg.mode->setCurrentIndex(0);
	    }
	  else if(fieldname == "front_cover")
	    {
	      if(!query.record().field(i).isNull())
		{
		  vg.front_image->loadFromData
		    (QByteArray::fromBase64(var.toByteArray()));

		  if(vg.front_image->image.isNull())
		    vg.front_image->loadFromData(var.toByteArray());
		}
	    }
	  else if(fieldname == "back_cover")
	    {
	      if(!query.record().field(i).isNull())
		{
		  vg.back_image->loadFromData
		    (QByteArray::fromBase64(var.toByteArray()));

		  if(vg.back_image->image.isNull())
		    vg.back_image->loadFromData(var.toByteArray());
		}
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
  vg.okButton->setText(tr("&Save"));
  vg.release_date->setDate(QDate::fromString("01/01/2000",
					     "MM/dd/yyyy"));
  vg.price->setMinimum(0.00);
  vg.price->setValue(0.00);
  vg.quantity->setMinimum(1);
  vg.quantity->setValue(1);
  vg.developer->setPlainText("N/A");
  vg.publisher->setPlainText("N/A");
  vg.place->setPlainText("N/A");
  vg.genre->setPlainText("N/A");
  vg.description->setPlainText("N/A");
  vg.keyword->clear();
  vg.showUserButton->setEnabled(false);
  vg.location->setCurrentIndex(0);
  vg.mode->setCurrentIndex(0);
  vg.language->setCurrentIndex(0);
  vg.monetary_units->setCurrentIndex(0);
  vg.rating->setCurrentIndex(0);
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
  misc_functions::highlightWidget
    (vg.place->viewport(), QColor(255, 248, 220));
  setWindowTitle(tr("BiblioteQ: Create Video Game Entry"));
  engWindowTitle = "Create";
  vg.id->setFocus();
  storeData(this);
  show();
}

/*
** -- slotReset() --
*/

void qtbook_videogame::slotReset(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(action != 0)
    {
      QList<QAction *> actions = vg.resetButton->menu()->actions();

      if(action == actions[0])
	vg.front_image->clear();
      else if(action == actions[1])
	vg.back_image->clear();
      else if(action == actions[2])
	{
	  vg.id->clear();
	  vg.id->setFocus();
	}
      else if(action == actions[3])
	{
	  vg.rating->setCurrentIndex(0);
	  vg.rating->setFocus();
	}
      else if(action == actions[4])
	{
	  if(engWindowTitle.contains("Search"))
	    vg.developer->clear();
	  else
	    vg.developer->setPlainText("N/A");

	  vg.developer->setFocus();
	}
      else if(action == actions[5])
	{
	  vg.platform->setCurrentIndex(0);
	  vg.platform->setFocus();
	}
      else if(action == actions[6])
	{
	  vg.mode->setCurrentIndex(0);
	  vg.mode->setFocus();
	}
      else if(action == actions[7])
	{
	  vg.title->clear();
	  vg.title->setFocus();
	}
      else if(action == actions[8])
	{
	  if(engWindowTitle.contains("Search"))
	    vg.release_date->setDate
	      (QDate::fromString("01/7999", "MM/yyyy"));
	  else
	    vg.release_date->setDate
	      (QDate::fromString("01/01/2000", "MM/dd/yyyy"));

	  vg.release_date->setFocus();
	}
      else if(action == actions[9])
	{
	  if(engWindowTitle.contains("Search"))
	    vg.publisher->clear();
	  else
	    vg.publisher->setPlainText("N/A");

	  vg.publisher->setFocus();
	}
      else if(action == actions[10])
	{
	  if(engWindowTitle.contains("Search"))
	    vg.place->clear();
	  else
	    vg.place->setPlainText("N/A");

	  vg.place->setFocus();
	}
      else if(action == actions[11])
	{
	  if(engWindowTitle.contains("Search"))
	    vg.genre->clear();
	  else
	    vg.genre->setPlainText("N/A");

	  vg.genre->setFocus();
	}
      else if(action == actions[12])
	{
	  vg.price->setValue(vg.price->minimum());
	  vg.price->setFocus();
	}
      else if(action == actions[13])
	{
	  vg.language->setCurrentIndex(0);
	  vg.language->setFocus();
	}
      else if(action == actions[14])
	{
	  vg.monetary_units->setCurrentIndex(0);
	  vg.monetary_units->setFocus();
	}
      else if(action == actions[15])
	{
	  vg.quantity->setValue(vg.quantity->minimum());
	  vg.quantity->setFocus();
	}
      else if(action == actions[16])
	{
	  vg.location->setCurrentIndex(0);
	  vg.location->setFocus();
	}
      else if(action == actions[17])
	{
	  if(engWindowTitle.contains("Search"))
	    vg.description->clear();
	  else
	    vg.description->setPlainText("N/A");

	  vg.description->setFocus();
	}
      else if(action == actions[18])
	{
	  vg.keyword->clear();
	  vg.keyword->setFocus();
	}

      actions.clear();
    }
  else
    {
      /*
      ** Reset all.
      */

      vg.id->clear();
      vg.title->clear();

      if(engWindowTitle.contains("Search"))
	vg.developer->clear();
      else
	vg.developer->setPlainText("N/A");

      if(engWindowTitle.contains("Search"))
	vg.publisher->clear();
      else
	vg.publisher->setPlainText("N/A");

      if(engWindowTitle.contains("Search"))
	vg.place->clear();
      else
	vg.place->setPlainText("N/A");

      if(engWindowTitle.contains("Search"))
	vg.genre->clear();
      else
	vg.genre->setPlainText("N/A");

      if(engWindowTitle.contains("Search"))
	vg.description->clear();
      else
	vg.description->setPlainText("N/A");

      if(engWindowTitle.contains("Search"))
	vg.release_date->setDate(QDate::fromString("01/7999",
						   "MM/yyyy"));
      else
	vg.release_date->setDate(QDate::fromString("01/01/2000",
						   "MM/dd/yyyy"));

      vg.quantity->setValue(vg.quantity->minimum());
      vg.location->setCurrentIndex(0);
      vg.rating->setCurrentIndex(0);
      vg.price->setValue(vg.price->minimum());
      vg.language->setCurrentIndex(0);
      vg.monetary_units->setCurrentIndex(0);
      vg.platform->setCurrentIndex(0);
      vg.mode->setCurrentIndex(0);
      vg.front_image->clear();
      vg.back_image->clear();
      vg.keyword->clear();
      vg.id->setFocus();
    }
}

/*
** -- closeEvent() --
*/

void qtbook_videogame::closeEvent(QCloseEvent *e)
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

  qmain->removeVideoGame(this);
  QMainWindow::closeEvent(e);
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
  html += "<b>" + tr("UPC:") + "</b> " + vg.id->text().trimmed() + "<br>";
  html += "<b>" + tr("Game Rating:") + "</b> " +
    vg.rating->currentText() + "<br>";
  html += "<b>" + tr("Developer(s):") + "</b> " +
    vg.developer->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Platform:") + "</b> " +
    vg.platform->currentText() + "<br>";
  html += "<b>" + tr("Mode:") + "</b> " + vg.mode->currentText() + "<br>";

  /*
  ** General information.
  */

  html += "<b>" + tr("Title:") + "</b> " + vg.title->text().trimmed() + "<br>";
  html += "<b>" + tr("Release Date:") + "</b> " + vg.release_date->date().
    toString(Qt::SystemLocaleShortDate) + "<br>";
  html += "<b>" + tr("Publisher:") + "</b> " +
    vg.publisher->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Place of Publication:") + "</b> " +
    vg.place->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Genre:") + "</b> " +
    vg.genre->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Price:") + "</b> " + vg.price->cleanText() + "<br>";
  html += "<b>" + tr("Language:") + "</b> " +
    vg.language->currentText() + "<br>";
  html += "<b>" + tr("Monetary Units:") + "</b> " +
    vg.monetary_units->currentText() + "<br>";
  html += "<b>" + tr("Copies:") + "</b> " + vg.quantity->text() + "<br>";
  html += "<b>" + tr("Location:") + "</b> " +
    vg.location->currentText() + "<br>";
  html += "<b>" + tr("Abstract:") + "</b> " +
    vg.description->toPlainText().trimmed() +
    "<br>";
  html += "<b>" + tr("Keywords:") + "</b> " +
    vg.keyword->toPlainText().trimmed() +
    "<br>";
  print(this);
}

/*
** -- slotSelectImage() --
*/

void qtbook_videogame::slotSelectImage(void)
{
  QFileDialog dialog(this);
  QPushButton *button = qobject_cast<QPushButton *> (sender());

#ifdef Q_WS_MAC
  dialog.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setDirectory(QDir::homePath());
  dialog.setNameFilter("Image Files (*.bmp *.jpg *.jpeg *.png)");

  if(button == vg.frontButton)
    dialog.setWindowTitle(tr("BiblioteQ: Front Cover Image Selection"));
  else
    dialog.setWindowTitle(tr("BiblioteQ: Back Cover Image Selection"));

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

/*
** -- duplicate() --
*/

void qtbook_videogame::duplicate(const QString &p_oid, const int state)
{
  modify(state); // Initial population.
  vg.copiesButton->setEnabled(false);
  vg.showUserButton->setEnabled(false);
  oid = p_oid;
  setWindowTitle(tr("BiblioteQ: Duplicate Video Game Entry"));
  engWindowTitle = "Create";
}
