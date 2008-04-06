/*
** Includes dvd-specific methods.
*/

/*
** -- Local Includes --
*/

#include "qtbook_dvd.h"

extern qtbook *qmain;
extern QApplication *qapp;

/*
** -- qtbook_dvd() --
*/

qtbook_dvd::qtbook_dvd(QMainWindow *parentArg,
		       const QStringList &categories,
		       const QStringList &languages,
		       const QStringList &monetary_units,
		       const QStringList &locations,
		       const QStringList &ratings,
		       const QStringList &aspectratios,
		       const QStringList &regions,
		       const QString &oidArg,
		       const int rowArg):
  QMainWindow()
{
  QMenu *menu = NULL;
  QPoint p(0, 0);
  QRegExp rx1("[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]");
  QValidator *validator1 = NULL;
  QGraphicsScene *scene1 = NULL;
  QGraphicsScene *scene2 = NULL;

  if((menu = new QMenu()) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((validator1 = new QRegExpValidator(rx1, this)) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene1 = new QGraphicsScene()) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene2 = new QGraphicsScene()) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  oid = oidArg;
  row = rowArg;
  isQueryEnabled = false;
  parentWid = parentArg;
  oldq = misc_functions::getColumnString
    (qmain->getUI().table, row, "Quantity").toInt();
  dvd.setupUi(this);
  updateFont(qapp->font(), (QWidget *) this);
  connect(dvd.okButton, SIGNAL(clicked(void)), this, SLOT(slotGo(void)));
  connect(dvd.printButton, SIGNAL(clicked(void)), this, SLOT(slotPrint(void)));
  connect(dvd.showUserButton, SIGNAL(clicked(void)), this,
	  SLOT(slotShowUsers(void)));
  connect(dvd.queryButton, SIGNAL(clicked(void)), this,
	  SLOT(slotQuery(void)));
  connect(dvd.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCancel(void)));
  connect(dvd.copiesButton, SIGNAL(clicked(void)), this,
	  SLOT(slotPopulateCopiesEditor(void)));
  connect(dvd.resetButton, SIGNAL(clicked(void)), this,
	  SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Front Cover Image"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Back Cover Image"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &UPC"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Rating"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Actor(s)"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Director(s)"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Number of Discs"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Runtime"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Format"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Region"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Aspect Ratio"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Title"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Release Date"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Studio"),
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
  connect(dvd.frontButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(dvd.backButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  dvd.queryButton->setVisible(isQueryEnabled);
  dvd.resetButton->setMenu(menu);
  dvd.id->setValidator(validator1);
  dvd.category->addItems(categories);
  dvd.language->addItems(languages);
  dvd.monetary_units->addItems(monetary_units);
  dvd.location->addItems(locations);
  dvd.rating->addItems(ratings);
  dvd.aspectratio->addItems(aspectratios);
  dvd.region->addItems(regions);
  dvd.front_image->setScene(scene1);
  dvd.back_image->setScene(scene2);

  if(dvd.category->count() == 0)
    dvd.category->addItem("UNKNOWN");

  if(dvd.language->count() == 0)
    dvd.language->addItem("UNKNOWN");

  if(dvd.monetary_units->count() == 0)
    dvd.monetary_units->addItem("UNKNOWN");

  if(dvd.location->count() == 0)
    dvd.location->addItem("UNKNOWN");

  if(dvd.rating->count() == 0)
    dvd.rating->addItem("UNKNOWN");

  if(dvd.aspectratio->count() == 0)
    dvd.aspectratio->addItem("UNKNOWN");

  if(dvd.region->count() == 0)
    dvd.region->addItem("UNKNOWN");

  resize(baseSize());
  p = parentWid->mapToGlobal(p);
  move(p.x() + parentWid->width() / 2  - width() / 2,
       p.y() + parentWid->height() / 2 - height() / 2);
}

/*
** -- ~qtbook_dvd() --
*/

qtbook_dvd::~qtbook_dvd()
{
}

/*
** -- slotGo() --
*/

void qtbook_dvd::slotGo(void)
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
	  newq = dvd.quantity->value();
	  qapp->setOverrideCursor(Qt::WaitCursor);
	  maxcopynumber = misc_functions::getMaxCopyNumber
	    (qmain->getDB(), oid, "dvd", errorstr);

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
		 "It appears that you are decreasing the "
		 "number of copies while there are copies "
		 "that have been reserved.");
	      dvd.quantity->setValue(oldq);
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
      str = dvd.id->text().trimmed();
      dvd.id->setText(str);

      if(dvd.id->text().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the UPC field.");
	  dvd.id->setFocus();
	  goto db_rollback;
	}

      str = dvd.actors->toPlainText().trimmed();
      dvd.actors->setPlainText(str);

      if(dvd.actors->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Actor(s) field.");
	  dvd.actors->setFocus();
	  goto db_rollback;
	}

      str = dvd.directors->toPlainText().trimmed();
      dvd.directors->setPlainText(str);

      if(dvd.directors->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Director(s) field.");
	  dvd.directors->setFocus();
	  goto db_rollback;
	}

      if(dvd.runtime->text() == "00:00:00")
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please provide a valid Runtime.");
	  dvd.runtime->setFocus();
	  goto db_rollback;
	}

      str = dvd.format->text().trimmed();
      dvd.format->setText(str);

      if(dvd.format->text().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Format field.");
	  dvd.format->setFocus();
	  goto db_rollback;
	}

      str = dvd.title->text().trimmed();
      dvd.title->setText(str);

      if(dvd.title->text().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Title "
				"field.");
	  dvd.title->setFocus();
	  goto db_rollback;
	}

      str = dvd.studio->text().trimmed();
      dvd.studio->setText(str);

      if(dvd.studio->text().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Studio field.");
	  dvd.studio->setFocus();
	  goto db_rollback;
	}

      str = dvd.description->toPlainText().trimmed();
      dvd.description->setPlainText(str);

      if(dvd.description->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Abstract field.");
	  dvd.description->setFocus();
	  goto db_rollback;
	}

      if(windowTitle().contains("Modify"))
	query.prepare(QString("UPDATE dvd SET "
			      "id = ?, "
			      "dvdrating = ?, "
			      "dvdactor = ?, "
			      "dvddirector = ?, "
			      "dvddiskcount = ?, "
			      "dvdruntime = ?, "
			      "dvdformat = ?, "
			      "dvdregion = ?, "
			      "dvdaspectratio = ?, "
			      "title = ?, "
			      "rdate = ?, "
			      "studio = ?, "
			      "category = ?, "
			      "price = ?, "
			      "language = ?, "
			      "monetary_units = ?, "
			      "quantity = ?, "
			      "location = ?, "
			      "description = ?, "
			      "front_cover = ?, "
			      "back_cover = ?, "
			      "front_cover_fmt = ?, "
			      "back_cover_fmt = ? "
			      "WHERE "
			      "myoid = ?"));
      else
	query.prepare(QString("INSERT INTO dvd "
			      "(id, "
			      "dvdrating, "
			      "dvdactor, "
			      "dvddirector, "
			      "dvddiskcount, "
			      "dvdruntime, "
			      "dvdformat, "
			      "dvdregion, "
			      "dvdaspectratio, "
			      "title, "
			      "rdate, "
			      "studio, "
			      "category, "
			      "price, "
			      "language, "
			      "monetary_units, "
			      "quantity, "
			      "location, "
			      "description, front_cover, "
			      "back_cover, front_cover_fmt, "
			      "back_cover_fmt) "
			      "VALUES "
			      "(?, ?, ?, "
			      "?, ?, "
			      "?, ?, ?, "
			      "?, ?, ?, "
			      "?, ?, ?, "
			      "?, ?, ?, ?, ?, ?, ?, ?, ?)"));

      query.bindValue(0, dvd.id->text());
      query.bindValue(1, dvd.rating->currentText().trimmed());
      query.bindValue(2, dvd.actors->toPlainText());
      query.bindValue(3, dvd.directors->toPlainText());
      query.bindValue(4, dvd.no_of_discs->text());
      query.bindValue(5, dvd.runtime->text());
      query.bindValue(6, dvd.format->text());
      query.bindValue(7, dvd.region->currentText().trimmed());
      query.bindValue(8, dvd.aspectratio->currentText().trimmed());
      query.bindValue(9, dvd.title->text());
      query.bindValue(10, dvd.release_date->date().toString("MM/dd/yyyy"));
      query.bindValue(11, dvd.studio->text());
      query.bindValue(12, dvd.category->currentText().trimmed());
      query.bindValue(13, dvd.price->text());
      query.bindValue(14, dvd.language->currentText().trimmed());
      query.bindValue(15, dvd.monetary_units->currentText().trimmed());
      query.bindValue(16, dvd.quantity->text());
      query.bindValue(17, dvd.location->currentText().trimmed());
      query.bindValue(18, dvd.description->toPlainText());

      if(dvd.frontCheck->isChecked())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);
	  buffer.open(QIODevice::WriteOnly);
	  frontImage.save(&buffer, frontImageFormat.toAscii(), 100);
	  query.bindValue(19, bytes);
	}
      else
	{
	  frontImageFormat = "";
	  query.bindValue(19, QVariant());
	}

      if(dvd.backCheck->isChecked())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);
	  buffer.open(QIODevice::WriteOnly);
	  backImage.save(&buffer, backImageFormat.toAscii(), 100);
	  query.bindValue(20, bytes);
	}
      else
	{
	  backImageFormat = "";
	  query.bindValue(20, QVariant());
	}

      query.bindValue(21, frontImageFormat);
      query.bindValue(22, backImageFormat);

      if(windowTitle().contains("Modify"))
	query.bindValue(23, oid);

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
	      query.prepare(QString("DELETE FROM dvd_copy_info WHERE "
				    "copy_number > ? AND "
				    "item_oid = "
				    "?"));
	      query.bindValue(0, dvd.quantity->text());
	      query.bindValue(1, oid);

	      if(!query.exec())
		{
		  qapp->restoreOverrideCursor();
		  qmain->addError
		    (QString("Database Error"),
		     QString("Unable to purge unnecessary copy "
			     "data."),
		     query.lastError().text(), __FILE__, __LINE__);
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

	      misc_functions::createInitialCopies(dvd.id->text(),
						  dvd.quantity->value(),
						  qmain->getDB(), "dvd",
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

	  oldq = dvd.quantity->value();	  

	  if(!dvd.frontCheck->isChecked())
	    {
	      frontImage = QImage();
	      frontImageFormat = "";

	      if(dvd.front_image->items().size() > 0)
		dvd.front_image->scene()->removeItem
		  (dvd.front_image->items().at(0));
	    }

	  if(!dvd.backCheck->isChecked())
	    {
	      backImage = QImage();
	      backImageFormat = "";

	      if(dvd.back_image->items().size() > 0)
		dvd.back_image->scene()->removeItem
		  (dvd.back_image->items().at(0));
	    }

	  dvd.frontCheck->setChecked
	    (dvd.front_image->items().size() > 0);
	  dvd.backCheck->setChecked
	    (dvd.back_image->items().size() > 0);
	  qapp->restoreOverrideCursor();

	  if(windowTitle().contains("Modify"))
	    {
	      qmain->slotUpdateStatusLabel();
	      str = QString("BiblioteQ: Modify DVD Entry (%1)").arg
		(dvd.id->text());
	      setWindowTitle(str);

	      if((qmain->getUI().typefilter->currentText() == "All" ||
		  qmain->getUI().typefilter->currentText() == "DVDs") &&
		 oid == misc_functions::getColumnString(qmain->getUI().table,
							row, "OID"))
		{
		  for(i = 0; i < qmain->getUI().table->columnCount(); i++)
		    {
		      column = qmain->getUI().table->horizontalHeaderItem(i);

		      if(column == NULL)
			continue;

		      if(column->text() == "UPC" ||
			 column->text() == "ID Number")
			qmain->getUI().table->item(row, i)->setText
			  (dvd.id->text());
		      else if(column->text() == "Rating")
			qmain->getUI().table->item(row, i)->setText
			  (dvd.rating->currentText().trimmed());
		      else if(column->text() == "Number of Discs")
			qmain->getUI().table->item(row, i)->setText
			  (dvd.no_of_discs->text());
		      else if(column->text() == "Runtime")
			qmain->getUI().table->item(row, i)->setText
			  (dvd.runtime->text());
		      else if(column->text() == "Format")
			qmain->getUI().table->item(row, i)->setText
			  (dvd.format->text());
		      else if(column->text() == "Region")
			qmain->getUI().table->item(row, i)->setText
			  (dvd.region->currentText().trimmed());
		      else if(column->text() == "Aspect Ratio")
			qmain->getUI().table->item(row, i)->setText
			  (dvd.aspectratio->currentText().trimmed());
		      else if(column->text() == "Title")
			qmain->getUI().table->item(row, i)->setText
			  (dvd.title->text());
		      else if(column->text() == "Release Date" ||
			      column->text() == "Publication Date")
			qmain->getUI().table->item(row, i)->setText
			  (dvd.release_date->date().toString("MM/dd/yyyy"));
		      else if(column->text() == "Studio" ||
			      column->text() == "Publisher")
			qmain->getUI().table->item(row, i)->setText
			  (dvd.studio->text());
		      else if(column->text() == "Category")
			qmain->getUI().table->item(row, i)->setText
			  (dvd.category->currentText().trimmed());
		      else if(column->text() == "Price")
			qmain->getUI().table->item(row, i)->setText
			  (dvd.price->text());
		      else if(column->text() == "Language")
			qmain->getUI().table->item(row, i)->setText
			  (dvd.language->currentText().trimmed());
		      else if(column->text() == "Monetary Units")
			qmain->getUI().table->item(row, i)->setText
			  (dvd.monetary_units->currentText().trimmed());
		      else if(column->text() == "Quantity")
			qmain->getUI().table->item(row, i)->setText
			  (dvd.quantity->text());
		      else if(column->text() == "Location")
			qmain->getUI().table->item(row, i)->setText
			  (dvd.location->currentText().trimmed());
		      else if(column->text() == "Availability")
			{
			  qmain->getUI().table->item(row, i)->setText
			    (misc_functions::getAvailability
			     (oid, qmain->getDB(), "dvd", errorstr));

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
		}
	    }
	  else
	    {
	      qapp->setOverrideCursor(Qt::WaitCursor);
	      oid = misc_functions::getOID(dvd.id->text(),
					   "dvd",
					   qmain->getDB(),
					   errorstr);
	      qapp->restoreOverrideCursor();

	      if(!errorstr.isEmpty())
		{
		  oid = "insert";
		  qmain->addError(QString("Database Error"),
				  QString("Unable to retrieve the DVD's "
					  "OID."),
				  errorstr, __FILE__, __LINE__);
		  QMessageBox::critical(this, "BiblioteQ: Database Error",
					"Unable to retrieve the DVD's "
					"OID.");
		}
	      else
		qmain->replaceDVD("insert", this);

	      updateWindow(qtbook::EDITABLE);

	      if(qmain->getUI().actionAutoPopulateOnCreation->isChecked())
		(void) qmain->populateTable
		  (qtbook::POPULATE_ALL,
		   qmain->getUI().typefilter->findText("DVDs"),
		   QString(""));

	      raise();
	    }
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
      searchstr = "SELECT dvd.title, "
	"dvd.dvdformat, "
	"dvd.studio, "
	"dvd.rdate, "
	"dvd.dvddiskcount, "
	"dvd.dvdruntime, "
	"dvd.category, "
	"dvd.language, "
	"dvd.id, "
	"dvd.price, "
	"dvd.monetary_units, "
	"dvd.quantity, "
	"dvd.location, "
	"dvd.dvdrating, "
	"dvd.dvdregion, "
	"dvd.dvdaspectratio, "
	"dvd.quantity - "
	"COUNT(dvd_borrower_vw.item_oid) "
	"AS availability, "
	"dvd.type, "
	"dvd.myoid "
	"FROM "
	"dvd LEFT JOIN dvd_borrower_vw ON "
	"dvd.myoid = dvd_borrower_vw.item_oid WHERE ";
      searchstr.append("id LIKE '%").append
	(dvd.id->text()).append("%' AND ");
      searchstr.append("LOWER(dvdformat) LIKE '%").append
	(myqstring::escape(dvd.format->text().toLower())).append("%' AND ");

      if(dvd.aspectratio->currentText() != "Any")
	searchstr.append("dvdaspectratio = '" +
			 dvd.aspectratio->currentText() +
			 "' AND ");

      searchstr.append("LOWER(dvdactor) LIKE '%").append
	(myqstring::escape(dvd.actors->toPlainText().toLower())).
	append("%' AND ");
      searchstr.append("LOWER(dvddirector) LIKE '%").append
	(myqstring::escape(dvd.directors->toPlainText().toLower())).
	append("%' AND ");

      if(dvd.no_of_discs->value() > 0)
	searchstr.append("dvddiskcount = ").append
	  (dvd.no_of_discs->text()).append(" AND ");

      if(dvd.runtime->text() != "00:00:00")
	searchstr.append("dvdruntime = '" + dvd.runtime->text() +
			 "' AND ");

      if(dvd.rating->currentText() != "Any")
	searchstr.append("dvdrating = '" +
			 myqstring::escape(dvd.rating->currentText()) +
			 "' AND ");

      if(dvd.region->currentText() != "Any")
	searchstr.append("dvdregion = '" +
			 myqstring::escape(dvd.region->currentText()) +
			 "' AND ");

      searchstr.append("LOWER(title) LIKE '%").append
	(myqstring::escape(dvd.title->text().toLower())).append("%' AND ");

      if(dvd.release_date->date().toString
	 ("MM/dd/yyyy") != "01/01/7999")
	searchstr.append("rdate = '" +
			 dvd.release_date->date().toString
			 ("MM/dd/yyyy") +
			 "' AND ");

      searchstr.append("LOWER(studio) LIKE '%" +
		       myqstring::escape(dvd.studio->text().toLower()) +
		       "%' AND ");

      if(dvd.category->currentText() != "Any")
	searchstr.append("category = '" +
			 myqstring::escape(dvd.category->currentText()) +
			 "' AND ");

      if(dvd.price->value() > 0)
	{
	  searchstr.append("price = ");
	  searchstr.append(dvd.price->text());
	  searchstr.append(" AND ");
	}

      if(dvd.language->currentText() != "Any")
	searchstr.append("language = '" +
			 myqstring::escape(dvd.language->currentText()) +
			 "' AND ");

      if(dvd.monetary_units->currentText() != "Any")
	searchstr.append("monetary_units = '" +
			 myqstring::escape
			 (dvd.monetary_units->currentText()) +
			 "' AND ");

      searchstr.append("LOWER(description) LIKE '%" +
		       myqstring::escape
		       (dvd.description->toPlainText().toLower()) + "%' ");

      if(dvd.quantity->value() != 0)
	searchstr.append("AND quantity = " + dvd.quantity->text() + " ");

      if(dvd.location->currentText() != "Any")
	searchstr.append("AND location = '" +
			 myqstring::escape
			 (dvd.location->currentText()) + "' ");

      slotCancel();

      /*
      ** Search the database.
      */

      (void) qmain->populateTable
	(qtbook::POPULATE_SEARCH,
	 qmain->getUI().typefilter->findText("DVDs"), searchstr);
    }
}

/*
** -- search() --
*/

void qtbook_dvd::search(void)
{
  QPoint p(0, 0);

  dvd.coverImages->setVisible(false);
  dvd.id->clear();
  dvd.actors->clear();
  dvd.directors->clear();
  dvd.format->clear();
  dvd.title->clear();
  dvd.studio->clear();
  dvd.description->clear();
  dvd.copiesButton->setEnabled(false);
  dvd.queryButton->setEnabled(false);
  dvd.showUserButton->setEnabled(false);
  dvd.okButton->setText("&Search");
  dvd.release_date->setDate(QDate::fromString("01/01/7999",
					     "MM/dd/yyyy"));
  dvd.runtime->setTime(QTime(0, 0, 0));
  dvd.runtime->setMinimumTime(QTime(0, 0, 0));
  dvd.price->setMinimum(0.00);
  dvd.price->setValue(0.00);
  dvd.quantity->setMinimum(0);
  dvd.quantity->setValue(0);
  dvd.no_of_discs->setMinimum(0);
  dvd.no_of_discs->setValue(0);

  if(dvd.category->findText("Any") == -1)
    dvd.category->insertItem(0, "Any");

  if(dvd.language->findText("Any") == -1)
    dvd.language->insertItem(0, "Any");

  if(dvd.monetary_units->findText("Any") == -1)
    dvd.monetary_units->insertItem(0, "Any");

  if(dvd.location->findText("Any") == -1)
    dvd.location->insertItem(0, "Any");

  if(dvd.rating->findText("Any") == -1)
    dvd.rating->insertItem(0, "Any");

  if(dvd.region->findText("Any") == -1)
    dvd.region->insertItem(0, "Any");

  if(dvd.aspectratio->findText("Any") == -1)
    dvd.aspectratio->insertItem(0, "Any");

  dvd.location->setCurrentIndex(0);
  dvd.category->setCurrentIndex(0);
  dvd.language->setCurrentIndex(0);
  dvd.monetary_units->setCurrentIndex(0);
  dvd.rating->setCurrentIndex(0);
  dvd.region->setCurrentIndex(0);
  dvd.aspectratio->setCurrentIndex(0);

  foreach(QAction *action, dvd.resetButton->menu()->findChildren<QAction *>())
    if(action->text().contains("Cover Image"))
      action->setVisible(false);

  setWindowTitle("BiblioteQ: Database DVD Search");
  dvd.id->setFocus();
  p = parentWid->mapToGlobal(p);
  move(p.x() + parentWid->width() / 2  - width() / 2,
       p.y() + parentWid->height() / 2 - height() / 2);
  show();
}

/*
** -- updateWindow() --
*/

void qtbook_dvd::updateWindow(const int state)
{
  QString str = "";

  if(state == qtbook::EDITABLE)
    {
      dvd.showUserButton->setEnabled(true);
      dvd.copiesButton->setEnabled(true);
      dvd.okButton->setVisible(true);
      dvd.queryButton->setVisible(isQueryEnabled);
      dvd.resetButton->setVisible(true);
      dvd.frontButton->setVisible(true);
      dvd.backButton->setVisible(true);
      dvd.frontCheck->setVisible(true);
      dvd.backCheck->setVisible(true);
      str = QString("BiblioteQ: Modify DVD Entry (%1)").arg(dvd.id->text());
    }
  else
    {
      dvd.showUserButton->setEnabled(true);
      dvd.copiesButton->setEnabled(false);
      dvd.okButton->setVisible(false);
      dvd.queryButton->setVisible(false);
      dvd.resetButton->setVisible(false);
      dvd.frontButton->setVisible(false);
      dvd.backButton->setVisible(false);
      dvd.frontCheck->setVisible(false);
      dvd.backCheck->setVisible(false);
      str = QString("BiblioteQ: View DVD Details (%1)").arg(dvd.id->text());
    }

  dvd.coverImages->setVisible(true);
  setWindowTitle(str);
}

/*
** -- modify() --
*/

void qtbook_dvd::modify(const int state)
{
  int i = 0;
  QString str = "";
  QString fieldname = "";
  QString searchstr = "";
  QVariant var;
  QSqlQuery query(qmain->getDB());

  if(state == qtbook::EDITABLE)
    {
      setWindowTitle("BiblioteQ: Modify DVD Entry");
      dvd.showUserButton->setEnabled(true);
      dvd.copiesButton->setEnabled(true);
      dvd.okButton->setVisible(true);
      dvd.queryButton->setVisible(isQueryEnabled);
      dvd.resetButton->setVisible(true);
      dvd.frontButton->setVisible(true);
      dvd.backButton->setVisible(true);
      dvd.frontCheck->setVisible(true);
      dvd.backCheck->setVisible(true);
    }
  else
    {
      setWindowTitle("BiblioteQ: View DVD Details");
      dvd.showUserButton->setEnabled(true);
      dvd.copiesButton->setEnabled(false);
      dvd.okButton->setVisible(false);
      dvd.queryButton->setVisible(false);
      dvd.resetButton->setVisible(false);
      dvd.frontButton->setVisible(false);
      dvd.backButton->setVisible(false);
      dvd.frontCheck->setVisible(false);
      dvd.backCheck->setVisible(false);

      foreach(QAction *action,
	      dvd.resetButton->menu()->findChildren<QAction *>())
	if(action->text().contains("Cover Image"))
	  action->setVisible(false);
    }

  dvd.frontCheck->setChecked(false);
  dvd.backCheck->setChecked(false);
  dvd.queryButton->setEnabled(true);
  dvd.okButton->setText("&Save");
  dvd.runtime->setMinimumTime(QTime(0, 0, 1));
  dvd.price->setMinimum(0.01);
  dvd.price->setValue(0.01);
  dvd.quantity->setMinimum(1);
  dvd.quantity->setValue(1);
  dvd.no_of_discs->setMinimum(1);
  dvd.no_of_discs->setValue(1);
  str = oid;
  searchstr = "SELECT id, "
    "title, "
    "dvdformat, "
    "dvddiskcount, "
    "dvdruntime, "
    "rdate, "
    "studio, "
    "category, "
    "price, "
    "language, "
    "monetary_units, "
    "description, "
    "quantity, "
    "dvdactor, "
    "dvddirector, "
    "dvdrating, "
    "dvdregion, "
    "dvdaspectratio, "
    "location, "
    "front_cover_fmt, "
    "back_cover_fmt, "
    "front_cover, "
    "back_cover "
    "FROM "
    "dvd "
    "WHERE myoid = ";
  searchstr.append(str);
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(searchstr) || !query.next())
    {
      qapp->restoreOverrideCursor();
      qmain->addError(QString("Database Error"),
		      QString("Unable to retrieve the selected DVD's data."),
		      query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(this, "BiblioteQ: Database Error",
			    "Unable to retrieve the selected DVD's "
			    "data.");
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
	    dvd.title->setText(var.toString());
	  else if(fieldname == "studio")
	    dvd.studio->setText(var.toString());
	  else if(fieldname == "rdate")
	    dvd.release_date->setDate
	      (QDate::fromString(var.toString(), "MM/dd/yyyy"));
	  else if(fieldname == "price")
	    dvd.price->setValue(var.toDouble());
	  else if(fieldname == "category")
	    {
	      if(dvd.category->findText(var.toString()) > -1)
		dvd.category->setCurrentIndex
		  (dvd.category->findText(var.toString()));
	      else
		dvd.category->setCurrentIndex
		  (dvd.category->findText("UNKNOWN"));
	    }
	  else if(fieldname == "language")
	    {
	      if(dvd.language->findText(var.toString()) > -1)
		dvd.language->setCurrentIndex
		  (dvd.language->findText(var.toString()));
	      else
		dvd.language->setCurrentIndex
		  (dvd.language->findText("UNKNOWN"));
	    }
	  else if(fieldname == "quantity")
	    dvd.quantity->setValue(var.toInt());
	  else if(fieldname == "monetary_units")
	    {
	      if(dvd.monetary_units->findText(var.toString()) > -1)
		dvd.monetary_units->setCurrentIndex
		  (dvd.monetary_units->findText(var.toString()));
	      else
		dvd.monetary_units->setCurrentIndex
		  (dvd.monetary_units->findText("UNKNOWN"));
	    }
	  else if(fieldname == "dvddiskcount")
	    dvd.no_of_discs->setValue(var.toInt());
	  else if(fieldname == "dvdruntime")
	    dvd.runtime->setTime(QTime::fromString(var.toString(),
						   "hh:mm:ss"));
	  else if(fieldname == "location")
	    {
	      if(dvd.location->findText(var.toString()) > -1)
		dvd.location->setCurrentIndex
		  (dvd.location->findText(var.toString()));
	      else
		dvd.location->setCurrentIndex
		  (dvd.location->findText("UNKNOWN"));
	    }
	  else if(fieldname == "id")
	    {
	      if(state == qtbook::EDITABLE)
		str = QString("BiblioteQ: Modify DVD Entry (%1)").arg
		  (var.toString());
	      else
		str = QString("BiblioteQ: View DVD Details (%1)").arg
		  (var.toString());

	      setWindowTitle(str);
	      dvd.id->setText(var.toString());
	    }
	  else if(fieldname == "description")
	    dvd.description->setPlainText(var.toString());
	  else if(fieldname == "dvdformat")
	    dvd.format->setText(var.toString());
	  else if(fieldname == "dvdactor")
	    dvd.actors->setPlainText(var.toString());
	  else if(fieldname == "dvddirector")
	    dvd.directors->setPlainText(var.toString());
	  else if(fieldname == "dvdrating")
	    {
	      if(dvd.rating->findText(var.toString()) > -1)
		dvd.rating->setCurrentIndex
		  (dvd.rating->findText(var.toString()));
	      else
		dvd.rating->setCurrentIndex(0);
	    }
	  else if(fieldname == "dvdregion")
	    {
	      if(dvd.region->findText(var.toString()) > -1)
		dvd.region->setCurrentIndex
		  (dvd.region->findText(var.toString()));
	      else
		dvd.region->setCurrentIndex(0);
	    }
	  else if(fieldname == "dvdaspectratio")
	    {
	      if(dvd.aspectratio->findText(var.toString()) > -1)
		dvd.aspectratio->setCurrentIndex
		  (dvd.aspectratio->findText(var.toString()));
	      else
		dvd.aspectratio->setCurrentIndex(0);
	    }
	  else if(fieldname == "front_cover_fmt")
	    frontImageFormat = var.toString();
	  else if(fieldname == "back_cover_fmt")
	    backImageFormat = var.toString();
	  else if(fieldname == "front_cover")
	    {
	      if(!query.record().field(i).isNull())
		{
		  frontImage.loadFromData(var.toByteArray(),
					  frontImageFormat.toAscii());
		  dvd.front_image->scene()->addPixmap
		    (QPixmap().fromImage(frontImage));
		  dvd.frontCheck->setChecked(true);
		}
	    }
	  else if(fieldname == "back_cover")
	    {
	      if(!query.record().field(i).isNull())
		{
		  backImage.loadFromData(var.toByteArray(),
					 backImageFormat.toAscii());
		  dvd.back_image->scene()->addPixmap
		    (QPixmap().fromImage(backImage));
		  dvd.backCheck->setChecked(true);
		}
	    }
	}

      foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
	textfield->setCursorPosition(0);
    }

  dvd.id->setFocus();
  showNormal();
  raise();
}

/*
** -- insert() --
*/

void qtbook_dvd::insert(void)
{
  QPoint p(0, 0);

  dvd.id->clear();
  dvd.actors->clear();
  dvd.directors->clear();
  dvd.format->clear();
  dvd.title->clear();
  dvd.studio->clear();
  dvd.description->clear();
  dvd.copiesButton->setEnabled(false);
  dvd.showUserButton->setEnabled(false);
  dvd.queryButton->setEnabled(true);
  dvd.okButton->setText("&Save");
  dvd.release_date->setDate(QDate::fromString("01/01/2000",
					     "MM/dd/yyyy"));
  dvd.runtime->setTime(QTime(0, 0, 1));
  dvd.runtime->setMinimumTime(QTime(0, 0, 1));
  dvd.price->setMinimum(0.01);
  dvd.price->setValue(0.01);
  dvd.quantity->setMinimum(1);
  dvd.quantity->setValue(1);
  dvd.no_of_discs->setMinimum(1);
  dvd.no_of_discs->setValue(1);
  dvd.location->setCurrentIndex(0);
  dvd.category->setCurrentIndex(0);
  dvd.language->setCurrentIndex(0);
  dvd.monetary_units->setCurrentIndex(0);
  dvd.rating->setCurrentIndex(0);
  dvd.region->setCurrentIndex(0);
  dvd.aspectratio->setCurrentIndex(0);
  setWindowTitle("BiblioteQ: Create DVD Entry");
  dvd.id->setFocus();
  p = parentWid->mapToGlobal(p);
  move(p.x() + parentWid->width() / 2  - width() / 2,
       p.y() + parentWid->height() / 2 - height() / 2);
  show();
}

/*
** -- slotReset() --
*/

void qtbook_dvd::slotReset(void)
{
  QAction *action = qobject_cast<QAction *>(sender());
  QString name = "";

  if(action != NULL)
    {
      name = action->text();

      if(name.contains("Front Cover Image"))
	{
	  if(dvd.front_image->items().size() > 0)
	    dvd.front_image->scene()->removeItem
	      (dvd.front_image->items().at(0));

	  dvd.frontCheck->setChecked(false);
	}
      else if(name.contains("Back Cover Image"))
	{
	  if(dvd.back_image->items().size() > 0)
	    dvd.back_image->scene()->removeItem
	      (dvd.back_image->items().at(0));

	  dvd.backCheck->setChecked(false);
	}
      else if(name.contains("UPC"))
	{
	  dvd.id->clear();
	  dvd.id->setFocus();
	}
      else if(name.contains("Title"))
	{
	  dvd.title->clear();
	  dvd.title->setFocus();
	}
      else if(name.contains("Format"))
	{
	  dvd.format->clear();
	  dvd.format->setFocus();
	}
      else if(name.contains("Actor(s)"))
	{
	  dvd.actors->clear();
	  dvd.actors->setFocus();
	}
      else if(name.contains("Number of Discs"))
	{
	  dvd.no_of_discs->setValue(dvd.no_of_discs->minimum());
	  dvd.no_of_discs->setFocus();
	}
      else if(name.contains("Runtime"))
	{
	  if(windowTitle().contains("Search"))
	    dvd.runtime->setTime(QTime(0, 0, 0));
	  else
	    dvd.runtime->setTime(QTime(0, 0, 1));

	  dvd.runtime->setFocus();
	}
      else if(name.contains("Release Date"))
	{
	  if(windowTitle().contains("Search"))
	    dvd.release_date->setDate
	      (QDate::fromString("01/01/7999", "MM/dd/yyyy"));
	  else
	    dvd.release_date->setDate
	      (QDate::fromString("01/01/2000", "MM/dd/yyyy"));

	  dvd.release_date->setFocus();
	}
      else if(name.contains("Studio"))
	{
	  dvd.studio->clear();
	  dvd.studio->setFocus();
	}
      else if(name.contains("Category"))
	{
	  dvd.category->setCurrentIndex(0);
	  dvd.category->setFocus();
	}
      else if(name.contains("Price"))
	{
	  dvd.price->setValue(dvd.price->minimum());
	  dvd.price->setFocus();
	}
      else if(name.contains("Language"))
	{
	  dvd.language->setCurrentIndex(0);
	  dvd.language->setFocus();
	}
      else if(name.contains("Monetary Units"))
	{
	  dvd.monetary_units->setCurrentIndex(0);
	  dvd.monetary_units->setFocus();
	}
      else if(name.contains("Abstract"))
	{
	  dvd.description->clear();
	  dvd.description->setFocus();
	}
      else if(name.contains("Copies"))
	{
	  dvd.quantity->setValue(dvd.quantity->minimum());
	  dvd.quantity->setFocus();
	}
      else if(name.contains("Location"))
	{
	  dvd.location->setCurrentIndex(0);
	  dvd.location->setFocus();
	}
      else if(name.contains("Director(s)"))
	{
	  dvd.directors->clear();
	  dvd.directors->setFocus();
	}
      else if(name.contains("Rating"))
	{
	  dvd.rating->setCurrentIndex(0);
	  dvd.rating->setFocus();
	}
      else if(name.contains("Region"))
	{
	  dvd.region->setCurrentIndex(0);
	  dvd.region->setFocus();
	}
      else if(name.contains("Aspect Ratio"))
	{
	  dvd.aspectratio->setCurrentIndex(0);
	  dvd.aspectratio->setFocus();
	}
    }
  else
    {
      /*
      ** Reset all.
      */

      dvd.title->clear();
      dvd.studio->clear();

      if(windowTitle().contains("Search"))
	{
	  dvd.runtime->setTime(QTime(0, 0, 0));
	  dvd.release_date->setDate(QDate::fromString("01/01/7999",
						     "MM/dd/yyyy"));
	}
      else
	{
	  dvd.runtime->setTime(QTime(0, 0, 1));
	  dvd.release_date->setDate(QDate::fromString("01/01/2000",
						     "MM/dd/yyyy"));
	}

      dvd.id->clear();
      dvd.actors->clear();
      dvd.price->setValue(dvd.price->minimum());
      dvd.quantity->setValue(dvd.quantity->minimum());
      dvd.no_of_discs->setValue(dvd.no_of_discs->minimum());
      dvd.description->clear();
      dvd.location->setCurrentIndex(0);
      dvd.category->setCurrentIndex(0);
      dvd.language->setCurrentIndex(0);
      dvd.monetary_units->setCurrentIndex(0);
      dvd.rating->setCurrentIndex(0);
      dvd.region->setCurrentIndex(0);
      dvd.aspectratio->setCurrentIndex(0);

      if(dvd.front_image->items().size() > 0)
	dvd.front_image->scene()->removeItem
	  (dvd.front_image->items().at(0));

      if(dvd.back_image->items().size() > 0)
	dvd.back_image->scene()->removeItem
	  (dvd.back_image->items().at(0));

      dvd.frontCheck->setChecked(false);
      dvd.backCheck->setChecked(false);
      dvd.format->clear();
      dvd.directors->clear();
      dvd.id->setFocus();
    }
}

/*
** -- closeEvent() --
*/

void qtbook_dvd::closeEvent(QCloseEvent *e)
{
  (void) e;
  qmain->removeDVD(this);
}

/*
** -- slotCancel() --
*/

void qtbook_dvd::slotCancel(void)
{
  hide();
  qmain->removeDVD(this);
}

/*
** -- slotPopulateCopiesEditor() --
*/

void qtbook_dvd::slotPopulateCopiesEditor(void)
{
  copy_editor *copyeditor = NULL;

  if((copyeditor = new copy_editor(qobject_cast<QWidget *>(this),
				   (qtbook_item *) this,
				   false,
				   dvd.quantity->value(), oid,
				   dvd.id->text(),
				   dvd.quantity, font(), "dvd",
				   dvd.title->text().trimmed(),
				   "DVD")) != NULL)
    copyeditor->populateCopiesEditor();
}

/*
** -- slotShowUsers() --
*/

void qtbook_dvd::slotShowUsers(void)
{
  int state = 0;
  borrowers_editor *borrowerseditor = NULL;

  if(!dvd.okButton->isHidden())
    state = qtbook::EDITABLE;
  else
    state = qtbook::VIEW_ONLY;

  if((borrowerseditor = new borrowers_editor
      (qobject_cast<QWidget *>(this), (qtbook_item *) this,
       dvd.quantity->value(), oid, dvd.id->text(), font(), "DVD",
       state)) != NULL)
    borrowerseditor->showUsers();
}

/*
** -- slotQuery() --
*/

void qtbook_dvd::slotQuery(void)
{
}

/*
** -- slotPrint() --
*/

void qtbook_dvd::slotPrint(void)
{
  html = "";
  html += "<b>UPC:</b> " + dvd.id->text() + "<br>";
  html += "<b>Rating:</b> " + dvd.rating->currentText() + "<br>";
  html += "<b>Actor(s):</b> " + dvd.actors->toPlainText() + "<br>";
  html += "<b>Director(s):</b> " + dvd.directors->toPlainText() + "<br>";
  html += "<b>Number of Discs:</b> " + dvd.no_of_discs->text() + "<br>";
  html += "<b>Runtime:</b> " + dvd.runtime->text() + "<br>";
  html += "<b>Format:</b> " + dvd.format->text() + "<br>";
  html += "<b>Region:</b> " + dvd.region->currentText() + "<br>";
  html += "<b>Aspect Ratio:</b> " + dvd.aspectratio->currentText() + "<br>";

  /*
  ** General information.
  */

  html += "<b>Title:</b> " + dvd.title->text() + "<br>";
  html += "<b>Release Date:</b> " + dvd.release_date->date().
    toString("MM/dd/yyyy") + "<br>";
  html += "<b>Studio:</b> " + dvd.studio->text() + "<br>";
  html += "<b>Category:</b> " + dvd.category->currentText() + "<br>";
  html += "<b>Price:</b> " + dvd.price->text() + "<br>";
  html += "<b>Language:</b> " + dvd.language->currentText() + "<br>";
  html += "<b>Monetary Units:</b> " + dvd.monetary_units->currentText() +
    "<br>";
  html += "<b>Copies:</b> " + dvd.quantity->text() + "<br>";
  html += "<b>Location:</b> " + dvd.location->currentText() + "<br>";
  html += "<b>Abstract:</b> " + dvd.description->toPlainText();
  print(this);
}

/*
** -- slotSelectImage() --
*/

void qtbook_dvd::slotSelectImage(void)
{
  QFileDialog dialog(this);
  QPushButton *button = qobject_cast<QPushButton *> (sender());

  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setFilter("Image Files (*.bmp *.jpg *.jpeg *.png)");

  if(button == dvd.frontButton)
    dialog.setWindowTitle("Front Cover Image Selection");
  else
    dialog.setWindowTitle("Back Cover Image Selection");

  dialog.exec();

  if(dialog.result() == QDialog::Accepted)
    if(button == dvd.frontButton)
      {
	if(dvd.front_image->items().size() > 0)
	  dvd.front_image->scene()->removeItem
	    (dvd.front_image->items().at(0));

	frontImage = QImage(dialog.selectedFiles().at(0));
	frontImageFormat = dialog.selectedFiles().at(0).mid
	  (dialog.selectedFiles().at(0).lastIndexOf(".") + 1);
	frontImageFormat = frontImageFormat.toUpper();
	dvd.front_image->scene()->addPixmap(QPixmap().fromImage(frontImage));
	dvd.frontCheck->setChecked(true);
      }
    else
      {
	if(dvd.back_image->items().size() > 0)
	  dvd.back_image->scene()->removeItem
	    (dvd.back_image->items().at(0));

	backImage = QImage(dialog.selectedFiles().at(0));
	backImageFormat = dialog.selectedFiles().at(0).mid
	  (dialog.selectedFiles().at(0).lastIndexOf(".") + 1);
	backImageFormat = backImageFormat.toUpper();
	dvd.back_image->scene()->addPixmap(QPixmap().fromImage(backImage));
	dvd.backCheck->setChecked(true);
      }
}
