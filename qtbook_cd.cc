/*
** Includes cd-specific methods.
*/

/*
** -- Local Includes --
*/

#include "qtbook_cd.h"

extern qtbook *qmain;
extern QApplication *qapp;

/*
** -- qtbook_cd() --
*/

qtbook_cd::qtbook_cd(QMainWindow *parentArg,
		     const QStringList &categories,
		     const QStringList &languages,
		     const QStringList &monetary_units,
		     const QStringList &locations,
		     const QStringList &formats,
		     const QString &oidArg,
		     const int rowArg):
  QMainWindow()
{
  QMenu *menu = NULL;
  QRegExp rx1("[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]");
  QValidator *validator1 = NULL;
  QGraphicsScene *scene1 = NULL;
  QGraphicsScene *scene2 = NULL;

  if((menu = new QMenu()) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((tracks_diag = new QDialog(this)) == NULL)
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
  cd.setupUi(this);
  updateFont(qapp->font(), (QWidget *) this);
  tracks_diag->setWindowModality(Qt::WindowModal);
  trd.setupUi(tracks_diag);
  trd.table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
  updateFont(qapp->font(), (QWidget *) tracks_diag);
  connect(trd.table->horizontalHeader(), SIGNAL(sectionClicked(int)),
	  qmain, SLOT(slotResizeColumnsAfterSort(void)));
  connect(cd.okButton, SIGNAL(clicked(void)), this, SLOT(slotGo(void)));
  connect(cd.printButton, SIGNAL(clicked(void)), this, SLOT(slotPrint(void)));
  connect(cd.showUserButton, SIGNAL(clicked(void)), this,
	  SLOT(slotShowUsers(void)));
  connect(cd.queryButton, SIGNAL(clicked(void)), this,
	  SLOT(slotQuery(void)));
  connect(cd.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCancel(void)));
  connect(cd.copiesButton, SIGNAL(clicked(void)), this,
	  SLOT(slotPopulateCopiesEditor(void)));
  connect(cd.resetButton, SIGNAL(clicked(void)), this,
	  SLOT(slotReset(void)));
  connect(cd.tracksButton, SIGNAL(clicked(void)), this,
	  SLOT(slotPopulateTracksBrowser(void)));
  connect(trd.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCloseTracksBrowser(void)));
  connect(trd.insertButton, SIGNAL(clicked(void)), this,
	  SLOT(slotInsertTrack(void)));
  connect(trd.deleteButton, SIGNAL(clicked(void)), this,
	  SLOT(slotDeleteTrack(void)));
  connect(trd.saveButton, SIGNAL(clicked(void)), this,
	  SLOT(slotSaveTracks(void)));
  connect(cd.computeButton, SIGNAL(clicked(void)), this,
	  SLOT(slotComputeRuntime(void)));
  connect(menu->addAction("Reset &Front Cover Image"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Back Cover Image"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Catalog Number"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Format"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Artist"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Number of Discs"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Runtime"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Audio"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Recording Type"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Title"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Release Date"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction("Reset &Recording Label"),
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
  connect(cd.frontButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(cd.backButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  cd.queryButton->setVisible(isQueryEnabled);
  cd.resetButton->setMenu(menu);
  cd.id->setValidator(validator1);
  cd.category->addItems(categories);
  cd.language->addItems(languages);
  cd.monetary_units->addItems(monetary_units);
  cd.location->addItems(locations);
  cd.format->addItems(formats);
  cd.front_image->setScene(scene1);
  cd.back_image->setScene(scene2);

  if(cd.category->count() == 0)
    cd.category->addItem("UNKNOWN");

  if(cd.language->count() == 0)
    cd.language->addItem("UNKNOWN");

  if(cd.monetary_units->count() == 0)
    cd.monetary_units->addItem("UNKNOWN");

  if(cd.location->count() == 0)
    cd.location->addItem("UNKNOWN");

  if(cd.format->count() == 0)
    cd.format->addItem("UNKNOWN");

  resize(baseSize());
  misc_functions::center(this, parentWid);
  misc_functions::hideAdminFields(this, qmain->getRoles());
}

/*
** -- ~qtbook_cd() --
*/

qtbook_cd::~qtbook_cd()
{
}

/*
** -- slotGo() --
*/

void qtbook_cd::slotGo(void)
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
	  newq = cd.quantity->value();
	  qapp->setOverrideCursor(Qt::WaitCursor);
	  maxcopynumber = misc_functions::getMaxCopyNumber
	    (qmain->getDB(), oid, "CD", errorstr);

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
	      cd.quantity->setValue(oldq);
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
      str = cd.id->text().trimmed();
      cd.id->setText(str);

      if(cd.id->text().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Catalog Number field.");
	  cd.id->setFocus();
	  goto db_rollback;
	}

      str = cd.artist->toPlainText().trimmed();
      cd.artist->setPlainText(str);

      if(cd.artist->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Artist field.");
	  cd.artist->setFocus();
	  goto db_rollback;
	}

      str = cd.title->text().trimmed();
      cd.title->setText(str);

      if(cd.title->text().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Title field.");
	  cd.title->setFocus();
	  goto db_rollback;
	}

      if(cd.runtime->text() == "00:00:00")
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please provide a valid Runtime.");
	  cd.runtime->setFocus();
	  goto db_rollback;
	}

      str = cd.recording_label->toPlainText().trimmed();
      cd.recording_label->setPlainText(str);

      if(cd.recording_label->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Recording Label "
				"field.");
	  cd.recording_label->setFocus();
	  goto db_rollback;
	}

      str = cd.description->toPlainText().trimmed();
      cd.description->setPlainText(str);

      if(cd.description->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please complete the Abstract field.");
	  cd.description->setFocus();
	  goto db_rollback;
	}

      str = cd.url->toPlainText().trimmed();
      cd.url->setPlainText(str);

      if(windowTitle().contains("Modify"))
	query.prepare(QString("UPDATE cd SET "
			      "id = ?, "
			      "title = ?, "
			      "cdformat = ?, "
			      "artist = ?, "
			      "cddiskcount = ?, "
			      "cdruntime = ?, "
			      "rdate = ?, "
			      "recording_label = ?, "
			      "category = ?, "
			      "price = ?, "
			      "language = ?, "
			      "monetary_units = ?, "
			      "description = ?, "
			      "quantity = ?, "
			      "location = ?, "
			      "cdrecording = ?, "
			      "cdaudio = ?, "
			      "front_cover = ?, "
			      "back_cover = ?, "
			      "offsystem_url = ? "
			      "WHERE "
			      "myoid = ?"));
      else if(qmain->getDB().driverName() != "QSQLITE")
	query.prepare(QString("INSERT INTO cd "
			      "(id, "
			      "title, "
			      "cdformat, "
			      "artist, "
			      "cddiskcount, "
			      "cdruntime, "
			      "rdate, "
			      "recording_label, "
			      "category, "
			      "price, "
			      "language, "
			      "monetary_units, "
			      "description, "
			      "quantity, "
			      "location, "
			      "cdrecording, "
			      "cdaudio, front_cover, "
			      "back_cover, "
			      "offsystem_url) "
			      "VALUES "
			      "(?, ?, "
			      "?, ?, "
			      "?, ?, ?, "
			      "?, ?, ?, "
			      "?, ?, "
			      "?, ?, ?, ?, ?, ?, ?, ?)"));
      else
	query.prepare(QString("INSERT INTO cd "
			      "(id, "
			      "title, "
			      "cdformat, "
			      "artist, "
			      "cddiskcount, "
			      "cdruntime, "
			      "rdate, "
			      "recording_label, "
			      "category, "
			      "price, "
			      "language, "
			      "monetary_units, "
			      "description, "
			      "quantity, "
			      "location, "
			      "cdrecording, "
			      "cdaudio, front_cover, "
			      "back_cover, "
			      "offsystem_url, myoid) "
			      "VALUES "
			      "(?, ?, "
			      "?, ?, "
			      "?, ?, ?, "
			      "?, ?, ?, "
			      "?, ?, "
			      "?, ?, ?, ?, ?, ?, ?, ?, ?)"));

      query.bindValue(0, cd.id->text());
      query.bindValue(1, cd.title->text());
      query.bindValue(2, cd.format->currentText().trimmed());
      query.bindValue(3, cd.artist->toPlainText());
      query.bindValue(4, cd.no_of_discs->text());
      query.bindValue(5, cd.runtime->text());
      query.bindValue(6, cd.release_date->date().toString("MM/dd/yyyy"));
      query.bindValue(7, cd.recording_label->toPlainText());
      query.bindValue(8, cd.category->currentText().trimmed());
      query.bindValue(9, cd.price->text());
      query.bindValue(10, cd.language->currentText().trimmed());
      query.bindValue(11, cd.monetary_units->currentText().trimmed());
      query.bindValue(12, cd.description->toPlainText());
      query.bindValue(13, cd.quantity->text());
      query.bindValue(14, cd.location->currentText().trimmed());
      query.bindValue(15, cd.recording_type->currentText().trimmed());
      query.bindValue(16, cd.audio->currentText().trimmed());

      if(!cd.front_image->image.isNull())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);
	  buffer.open(QIODevice::WriteOnly);
	  cd.front_image->image.save
	    (&buffer, cd.front_image->imageFormat.toAscii(), 100);
	  query.bindValue(17, bytes);
	}
      else
	{
	  cd.front_image->imageFormat = "";
	  query.bindValue(17, QVariant());
	}

      if(!cd.back_image->image.isNull())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);
	  buffer.open(QIODevice::WriteOnly);
	  cd.back_image->image.save
	    (&buffer, cd.back_image->imageFormat.toAscii(), 100);
	  query.bindValue(18, bytes);
	}
      else
	{
	  cd.back_image->imageFormat = "";
	  query.bindValue(18, QVariant());
	}

      if(!cd.url->toPlainText().isEmpty())
	query.bindValue(19, cd.url->toPlainText());
      else
	query.bindValue(19, QVariant(QVariant::String));

      if(windowTitle().contains("Modify"))
	query.bindValue(20, oid);
      else if(qmain->getDB().driverName() == "QSQLITE")
	query.bindValue(20, cd.id->text());

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
	      query.prepare(QString("DELETE FROM cd_copy_info WHERE "
				    "copy_number > ? AND "
				    "item_oid = "
				    "?"));
	      query.bindValue(0, cd.quantity->text());
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

	      misc_functions::createInitialCopies(cd.id->text(),
						  cd.quantity->value(),
						  qmain->getDB(),
						  "CD", errorstr);

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

	  oldq = cd.quantity->value();

	  if(cd.front_image->image.isNull())
	    cd.front_image->imageFormat = "";

	  if(cd.back_image->image.isNull())
	    cd.back_image->imageFormat = "";

	  cd.artist->setText
	    (QString("<a href=\"cd_search?artist?%1\">" +
		     cd.artist->toPlainText() + "</a>").arg
	     (cd.artist->toPlainText()));
	  cd.recording_label->setText
	    (QString("<a href=\"cd_search?recording_label?%1\">" +
		     cd.recording_label->toPlainText() + "</a>").arg
	     (cd.recording_label->toPlainText()));

	  if(!cd.url->toPlainText().isEmpty())
	    cd.url->setText(QString("<a href=\"%1\">%1</a>").arg
			    (cd.url->toPlainText()));

	  qapp->restoreOverrideCursor();

	  if(windowTitle().contains("Modify"))
	    {
	      str = QString("BiblioteQ: Modify CD Entry (%1)").arg
		(cd.id->text());
	      setWindowTitle(str);

	      if((qmain->getUI().typefilter->currentText() == "All" ||
		  qmain->getUI().typefilter->currentText() == "Music CDs") &&
		 oid == misc_functions::getColumnString(qmain->getUI().table,
							row, "OID"))
		{
		  for(i = 0; i < qmain->getUI().table->columnCount(); i++)
		    {
		      column = qmain->getUI().table->horizontalHeaderItem(i);

		      if(column == NULL)
			continue;

		      if(column->text() == "Catalog Number" ||
			 column->text() == "ID Number")
			qmain->getUI().table->item(row, i)->setText
			  (cd.id->text());
		      else if(column->text() == "Title")
			qmain->getUI().table->item(row, i)->setText
			  (cd.title->text());
		      else if(column->text() == "Format")
			qmain->getUI().table->item(row, i)->setText
			  (cd.format->currentText().trimmed());
		      else if(column->text() == "Artist")
			qmain->getUI().table->item(row, i)->setText
			  (cd.artist->toPlainText());
		      else if(column->text() == "Number of Discs")
			qmain->getUI().table->item(row, i)->setText
			  (cd.no_of_discs->text());
		      else if(column->text() == "Runtime")
			qmain->getUI().table->item(row, i)->setText
			  (cd.runtime->text());
		      else if(column->text() == "Release Date" ||
			      column->text() == "Publication Date")
			qmain->getUI().table->item(row, i)->setText
			  (cd.release_date->date().toString("MM/dd/yyyy"));
		      else if(column->text() == "Recording Label" ||
			      column->text() == "Publisher")
			qmain->getUI().table->item(row, i)->setText
			  (cd.recording_label->toPlainText());
		      else if(column->text() == "Category")
			qmain->getUI().table->item(row, i)->setText
			  (cd.category->currentText().trimmed());
		      else if(column->text() == "Price")
			qmain->getUI().table->item(row, i)->setText
			  (cd.price->text());
		      else if(column->text() == "Language")
			qmain->getUI().table->item(row, i)->setText
			  (cd.language->currentText().trimmed());
		      else if(column->text() == "Monetary Units")
			qmain->getUI().table->item(row, i)->setText
			  (cd.monetary_units->currentText().trimmed());
		      else if(column->text() == "Quantity")
			qmain->getUI().table->item(row, i)->setText
			  (cd.quantity->text());
		      else if(column->text() == "Location")
			qmain->getUI().table->item(row, i)->setText
			  (cd.location->currentText().trimmed());
		      else if(column->text() == "Recording Type")
			qmain->getUI().table->item(row, i)->setText
			  (cd.recording_type->currentText().trimmed());
		      else if(column->text() == "Audio")
			qmain->getUI().table->item(row, i)->setText
			  (cd.audio->currentText().trimmed());
		      else if(column->text() == "Availability")
			{
			  qmain->getUI().table->item(row, i)->setText
			    (misc_functions::getAvailability
			     (oid, qmain->getDB(), "CD", errorstr));

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
	      oid = misc_functions::getOID(cd.id->text(),
					   "CD",
					   qmain->getDB(),
					   errorstr);
	      qapp->restoreOverrideCursor();

	      if(!errorstr.isEmpty())
		{
		  oid = "insert";
		  qmain->addError(QString("Database Error"),
				  QString("Unable to retrieve the CD's "
					  "OID."),
				  errorstr, __FILE__, __LINE__);
		  QMessageBox::critical(this, "BiblioteQ: Database Error",
					"Unable to retrieve the CD's "
					"OID.");
		}
	      else
		qmain->replaceCD("insert", this);

	      updateWindow(qtbook::EDITABLE);

	      if(qmain->getUI().actionAutoPopulateOnCreation->isChecked())
		(void) qmain->populateTable
		  (qtbook::POPULATE_ALL, "Music CDs", QString(""));

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
      searchstr = "SELECT cd.title, "
	"cd.artist, "
	"cd.cdformat, "
	"cd.recording_label, "
	"cd.rdate, "
	"cd.cddiskcount, "
	"cd.cdruntime, "
	"cd.category, "
	"cd.language, "
	"cd.id, "
	"cd.price, "
	"cd.monetary_units, "
	"cd.quantity, "
	"cd.location, "
	"cd.cdaudio, "
	"cd.cdrecording, "
	"cd.quantity - COUNT(item_borrower_vw.item_oid) AS availability, "
	"cd.type, "
	"cd.myoid "
	"FROM "
	"cd LEFT JOIN item_borrower_vw ON "
	"cd.myoid = item_borrower_vw.item_oid "
	"AND item_borrower_vw.type = 'CD' "
	"WHERE ";
      searchstr.append("id LIKE '%").append(cd.id->text()).append("%' AND ");

      if(cd.format->currentText() != "Any")
	searchstr.append("cdformat = '" +
			 myqstring::escape(cd.format->currentText()) +
			 "' AND ");

      searchstr.append("LOWER(artist) LIKE '%").append
	(myqstring::escape(cd.artist->toPlainText().toLower())).append
	("%' AND ");

      if(cd.no_of_discs->value() > 0)
	searchstr.append("cddiskcount = ").append
	  (cd.no_of_discs->text()).append(" AND ");

      if(cd.runtime->text() != "00:00:00")
	searchstr.append("cdruntime = '" + cd.runtime->text() +
			 "' AND ");

      if(cd.audio->currentText() != "Any")
	searchstr.append("cdaudio = '" + cd.audio->currentText() +
			 "' AND ");

      if(cd.recording_type->currentText() != "Any")
	searchstr.append("cdrecording = '" +
			 cd.recording_type->currentText() +
			 "' AND ");

      searchstr.append("LOWER(title) LIKE '%").append
	(myqstring::escape(cd.title->text().toLower())).append("%' AND ");

      if(cd.release_date->date().toString
	 ("MM/dd/yyyy") != "01/01/7999")
	searchstr.append("rdate = '" +
			 cd.release_date->date().toString
			 ("MM/dd/yyyy") +
			 "' AND ");

      searchstr.append("LOWER(recording_label) LIKE '%" +
		       myqstring::escape(cd.recording_label->toPlainText().
					 toLower()) + "%' AND ");

      if(cd.category->currentText() != "Any")
	searchstr.append("category = '" +
			 myqstring::escape(cd.category->currentText()) +
			 "' AND ");

      if(cd.price->value() > 0)
	{
	  searchstr.append("price = ");
	  searchstr.append(cd.price->text());
	  searchstr.append(" AND ");
	}

      if(cd.language->currentText() != "Any")
	searchstr.append("language = '" +
			 myqstring::escape(cd.language->currentText()) +
			 "' AND ");

      if(cd.monetary_units->currentText() != "Any")
	searchstr.append("monetary_units = '" +
			 myqstring::escape
			 (cd.monetary_units->currentText()) +
			 "' AND ");

      searchstr.append("LOWER(description) LIKE '%" +
		       myqstring::escape
		       (cd.description->toPlainText().toLower()) + "%' ");

      if(cd.quantity->value() != 0)
	searchstr.append(" AND quantity = " + cd.quantity->text());

      if(cd.location->currentText() != "Any")
	searchstr.append(" AND location = '" +
			 myqstring::escape
			 (cd.location->currentText()) + "' ");

      if(!cd.url->toPlainText().isEmpty())
	searchstr.append(" AND LOWER(COALESCE(offsystem_url, '')) LIKE '%" +
			 myqstring::escape(cd.url->toPlainText().toLower()) +
			 "%' ");

      slotCancel();

      /*
      ** Search the database.
      */

      (void) qmain->populateTable
	(qtbook::POPULATE_SEARCH, "Music CDs", searchstr);
    }
}

/*
** -- search() --
*/

void qtbook_cd::search(const QString &field, const QString &value)
{
  cd.id->clear();
  cd.artist->clear();
  cd.title->clear();
  cd.recording_label->clear();
  cd.description->clear();
  cd.copiesButton->setVisible(false);
  cd.tracksButton->setVisible(false);
  cd.queryButton->setVisible(false);
  cd.showUserButton->setVisible(false);
  cd.computeButton->setVisible(false);
  cd.tracks_lbl->setVisible(false);
  cd.okButton->setText("&Search");
  cd.release_date->setDate(QDate::fromString("01/01/7999",
					     "MM/dd/yyyy"));
  cd.runtime->setTime(QTime(0, 0, 0));
  cd.runtime->setMinimumTime(QTime(0, 0, 0));
  cd.price->setMinimum(0.00);
  cd.price->setValue(0.00);
  cd.quantity->setMinimum(0);
  cd.quantity->setValue(0);
  cd.no_of_discs->setMinimum(0);
  cd.no_of_discs->setValue(0);

  if(cd.audio->findText("Any") == -1)
    cd.audio->insertItem(0, "Any");

  if(cd.recording_type->findText("Any") == -1)
    cd.recording_type->insertItem(0, "Any");

  if(cd.format->findText("Any") == -1)
    cd.format->insertItem(0, "Any");

  if(cd.category->findText("Any") == -1)
    cd.category->insertItem(0, "Any");

  if(cd.language->findText("Any") == -1)
    cd.language->insertItem(0, "Any");

  if(cd.monetary_units->findText("Any") == -1)
    cd.monetary_units->insertItem(0, "Any");

  if(cd.location->findText("Any") == -1)
    cd.location->insertItem(0, "Any");

  cd.audio->setCurrentIndex(0);
  cd.location->setCurrentIndex(0);
  cd.category->setCurrentIndex(0);
  cd.language->setCurrentIndex(0);
  cd.monetary_units->setCurrentIndex(0);
  cd.recording_type->setCurrentIndex(0);
  cd.format->setCurrentIndex(0);
  cd.url->clear();

  if(field.isEmpty() && value.isEmpty())
    {
      cd.coverImages->setVisible(false);
      setWindowTitle("BiblioteQ: Database CD Search");
      cd.id->setFocus();
      misc_functions::center(this, parentWid);
      show();
    }
  else
    {
      if(field == "artist")
	cd.artist->setPlainText(value);
      else if(field == "recording_label")
	cd.recording_label->setPlainText(value);

      slotGo();
    }
}

/*
** -- updateWindow() --
*/

void qtbook_cd::updateWindow(const int state)
{
  QString str = "";

  if(state == qtbook::EDITABLE)
    {
      cd.showUserButton->setEnabled(true);
      cd.copiesButton->setEnabled(true);
      cd.okButton->setVisible(true);
      cd.queryButton->setVisible(isQueryEnabled);
      cd.resetButton->setVisible(true);
      cd.computeButton->setEnabled(true);
      trd.saveButton->setVisible(true);
      trd.insertButton->setVisible(true);
      trd.deleteButton->setVisible(true);
      cd.frontButton->setVisible(true);
      cd.backButton->setVisible(true);
      str = QString("BiblioteQ: Modify CD Entry (%1)").arg(cd.id->text());
    }
  else
    {
      cd.showUserButton->setEnabled(true);
      cd.copiesButton->setEnabled(false);
      cd.okButton->setVisible(false);
      cd.queryButton->setVisible(false);
      cd.resetButton->setVisible(false);
      cd.computeButton->setEnabled(false);
      trd.saveButton->setVisible(false);
      trd.insertButton->setVisible(false);
      trd.deleteButton->setVisible(false);      
      cd.frontButton->setVisible(false);
      cd.backButton->setVisible(false);
      str = QString("BiblioteQ: View CD Details (%1)").arg(cd.id->text());
    }

  setWindowTitle(str);
  cd.tracksButton->setEnabled(true);
}

/*
** -- modify() --
*/

void qtbook_cd::modify(const int state)
{
  int i = 0;
  QString str = "";
  QString fieldname = "";
  QString searchstr = "";
  QVariant var;
  QSqlQuery query(qmain->getDB());

  if(state == qtbook::EDITABLE)
    {
      setWindowTitle("BiblioteQ: Modify CD Entry");
      cd.showUserButton->setEnabled(true);
      cd.copiesButton->setEnabled(true);
      cd.okButton->setVisible(true);
      cd.queryButton->setVisible(isQueryEnabled);
      cd.resetButton->setVisible(true);
      cd.computeButton->setVisible(true);
      trd.saveButton->setVisible(true);
      trd.insertButton->setVisible(true);
      trd.deleteButton->setVisible(true);
      cd.frontButton->setVisible(true);
      cd.backButton->setVisible(true);
      misc_functions::highlightWidget
	(cd.id, QColor(255, 248, 220));
      misc_functions::highlightWidget
	(cd.title, QColor(255, 248, 220));
      misc_functions::highlightWidget
	(cd.recording_label->viewport(), QColor(255, 248, 220));
      misc_functions::highlightWidget
	(cd.artist->viewport(), QColor(255, 248, 220));
      misc_functions::highlightWidget
	(cd.description->viewport(), QColor(255, 248, 220));
    }
  else
    {
      setWindowTitle("BiblioteQ: View CD Details");
      cd.showUserButton->setEnabled(true);
      cd.copiesButton->setEnabled(false);
      cd.okButton->setVisible(false);
      cd.queryButton->setVisible(false);
      cd.resetButton->setVisible(false);
      cd.computeButton->setEnabled(false);
      trd.saveButton->setVisible(false);
      trd.insertButton->setVisible(false);
      trd.deleteButton->setVisible(false);      
      cd.frontButton->setVisible(false);
      cd.backButton->setVisible(false);

      foreach(QAction *action,
	      cd.resetButton->menu()->findChildren<QAction *>())
	if(action->text().contains("Cover Image"))
	  action->setVisible(false);
    }

  cd.tracksButton->setEnabled(true);
  cd.queryButton->setEnabled(true);
  cd.okButton->setText("&Save");
  cd.runtime->setMinimumTime(QTime(0, 0, 1));
  cd.price->setMinimum(0.01);
  cd.price->setValue(0.01);
  cd.quantity->setMinimum(1);
  cd.quantity->setValue(1);
  cd.no_of_discs->setMinimum(1);
  cd.no_of_discs->setValue(1);
  str = oid;
  searchstr = "SELECT id, "
    "title, "
    "cdformat, "
    "artist, "
    "cddiskcount, "
    "cdruntime, "
    "rdate, "
    "recording_label, "
    "category, "
    "price, "
    "language, "
    "monetary_units, "
    "description, "
    "quantity, "
    "cdaudio, "
    "cdrecording, "
    "location, "
    "front_cover, "
    "back_cover, "
    "offsystem_url "
    "FROM "
    "cd "
    "WHERE myoid = ";
  searchstr.append(str);
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(searchstr) || !query.next())
    {
      qapp->restoreOverrideCursor();
      qmain->addError(QString("Database Error"),
		      QString("Unable to retrieve the selected CD's data."),
		      query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(this, "BiblioteQ: Database Error",
			    "Unable to retrieve the selected CD's "
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
	    cd.title->setText(var.toString());
	  else if(fieldname == "recording_label")
	    cd.recording_label->setText
	      (QString("<a href=\"cd_search?recording_label?%1\">" +
		       var.toString() + "</a>").arg
	       (var.toString()));
	  else if(fieldname == "rdate")
	    cd.release_date->setDate
	      (QDate::fromString(var.toString(), "MM/dd/yyyy"));
	  else if(fieldname == "price")
	    cd.price->setValue(var.toDouble());
	  else if(fieldname == "category")
	    {
	      if(cd.category->findText(var.toString()) > -1)
		cd.category->setCurrentIndex
		  (cd.category->findText(var.toString()));
	      else
		cd.category->setCurrentIndex
		  (cd.category->findText("UNKNOWN"));
	    }
	  else if(fieldname == "language")
	    {
	      if(cd.language->findText(var.toString()) > -1)
		cd.language->setCurrentIndex
		  (cd.language->findText(var.toString()));
	      else
		cd.language->setCurrentIndex
		  (cd.language->findText("UNKNOWN"));
	    }
	  else if(fieldname == "quantity")
	    cd.quantity->setValue(var.toInt());
	  else if(fieldname == "monetary_units")
	    {
	      if(cd.monetary_units->findText(var.toString()) > -1)
		cd.monetary_units->setCurrentIndex
		  (cd.monetary_units->findText(var.toString()));
	      else
		cd.monetary_units->setCurrentIndex
		  (cd.monetary_units->findText("UNKNOWN"));
	    }
	  else if(fieldname == "cddiskcount")
	    cd.no_of_discs->setValue(var.toInt());
	  else if(fieldname == "cdruntime")
	    cd.runtime->setTime(QTime::fromString(var.toString(), "hh:mm:ss"));
	  else if(fieldname == "location")
	    {
	      if(cd.location->findText(var.toString()) > -1)
		cd.location->setCurrentIndex
		  (cd.location->findText(var.toString()));
	      else
		cd.location->setCurrentIndex
		  (cd.location->findText("UNKNOWN"));
	    }
	  else if(fieldname == "id")
	    {
	      if(state == qtbook::EDITABLE)
		str = QString("BiblioteQ: Modify CD Entry (%1)").arg
		  (var.toString());
	      else
		str = QString("BiblioteQ: View CD Details (%1)").arg
		  (var.toString());

	      setWindowTitle(str);
	      cd.id->setText(var.toString());
	    }
	  else if(fieldname == "description")
	    cd.description->setPlainText(var.toString());
	  else if(fieldname == "cdformat")
	    {
	      if(cd.format->findText(var.toString()) > -1)
		cd.format->setCurrentIndex
		  (cd.format->findText(var.toString()));
	      else
		cd.format->setCurrentIndex(0);
	    }
	  else if(fieldname == "artist")
	    cd.artist->setText
	      (QString("<a href=\"cd_search?artist?%1\">" +
		       var.toString() + "</a>").arg
	       (var.toString()));
	  else if(fieldname == "cdaudio")
	    {
	      if(cd.audio->findText(var.toString()) > -1)
		cd.audio->setCurrentIndex
		  (cd.audio->findText(var.toString()));
	      else
		cd.audio->setCurrentIndex(0);
	    }
	  else if(fieldname == "cdrecording")
	    {
	      if(cd.recording_type->findText(var.toString()) > -1)
		cd.recording_type->setCurrentIndex
		  (cd.recording_type->findText(var.toString()));
	      else
		cd.recording_type->setCurrentIndex(0);
	    }
	  else if(fieldname == "front_cover")
	    {
	      if(!query.record().field(i).isNull())
		{
		  cd.front_image->image.loadFromData
		    (var.toByteArray());
		  cd.front_image->determineFormat(var.toByteArray());
		  cd.front_image->scene()->addPixmap
		    (QPixmap().fromImage(cd.front_image->image));
		  cd.front_image->scene()->items().at(0)->setFlags
		    (QGraphicsItem::ItemIsSelectable);
		}
	    }
	  else if(fieldname == "back_cover")
	    {
	      if(!query.record().field(i).isNull())
		{
		  cd.back_image->image.loadFromData
		    (var.toByteArray());
		  cd.back_image->determineFormat(var.toByteArray());
		  cd.back_image->scene()->addPixmap
		    (QPixmap().fromImage(cd.back_image->image));
		  cd.back_image->scene()->items().at(0)->setFlags
		    (QGraphicsItem::ItemIsSelectable);
		}
	    }
	  else if(fieldname == "offsystem_url")
	    {
	      if(!query.record().field(i).isNull())
		cd.url->setText(QString("<a href=\"%1\">%1</a>").arg
				(var.toString()));
	    }
	}

      foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
	textfield->setCursorPosition(0);
    }

  cd.id->setFocus();
  showNormal();
  raise();
}

/*
** -- insert() --
*/

void qtbook_cd::insert(void)
{
  slotReset();
  cd.id->clear();
  cd.artist->clear();
  cd.title->clear();
  cd.recording_label->clear();
  cd.description->clear();
  cd.copiesButton->setEnabled(false);
  cd.tracksButton->setEnabled(false);
  cd.showUserButton->setEnabled(false);
  cd.queryButton->setEnabled(true);
  cd.computeButton->setEnabled(false);
  cd.okButton->setText("&Save");
  cd.release_date->setDate(QDate::fromString("01/01/2000",
					     "MM/dd/yyyy"));
  cd.runtime->setTime(QTime(0, 0, 1));
  cd.runtime->setMinimumTime(QTime(0, 0, 1));
  cd.price->setMinimum(0.01);
  cd.price->setValue(0.01);
  cd.quantity->setMinimum(1);
  cd.quantity->setValue(1);
  cd.no_of_discs->setMinimum(1);
  cd.no_of_discs->setValue(1);
  cd.audio->setCurrentIndex(0);
  cd.location->setCurrentIndex(0);
  cd.category->setCurrentIndex(0);
  cd.language->setCurrentIndex(0);
  cd.monetary_units->setCurrentIndex(0);
  cd.recording_type->setCurrentIndex(0);
  cd.format->setCurrentIndex(0);
  cd.url->clear();      
  misc_functions::highlightWidget
    (cd.id, QColor(255, 248, 220));
  misc_functions::highlightWidget
    (cd.title, QColor(255, 248, 220));
  misc_functions::highlightWidget
    (cd.recording_label->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (cd.artist->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (cd.description->viewport(), QColor(255, 248, 220));
  setWindowTitle("BiblioteQ: Create CD Entry");
  cd.id->setFocus();
  misc_functions::center(this, parentWid);
  show();
}

/*
** -- slotPopulateTracksBrowser() --
*/

void qtbook_cd::slotPopulateTracksBrowser(void)
{
  int i = -1;
  int j = 0;
  QString str = "";
  QString querystr = "";
  QSpinBox *trackEdit = NULL;
  QComboBox *comboBox = NULL;
  QSqlQuery query(qmain->getDB());
  QTimeEdit * timeEdit = NULL;
  QStringList list;
  QStringList comboBoxList;
  QProgressDialog progress(tracks_diag);
  QTableWidgetItem *item = NULL;

  querystr = "SELECT albumnum, songnum, songtitle, runtime "
    "FROM cd_songs WHERE item_oid = ";
  querystr.append(oid);
  querystr.append(" ORDER BY albumnum, songnum, songtitle, runtime");
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(querystr))
    {
      qapp->restoreOverrideCursor();
      qmain->addError(QString("Database Error"),
		      QString("Unable to retrieve track data for table "
			      "populating."),
		      query.lastError().text(),
		      __FILE__, __LINE__);
      QMessageBox::critical(this, "BiblioteQ: Database Error",
			    "Unable to retrieve track data for "
			    "table populating.");
      return;
    }

  qapp->restoreOverrideCursor();

  for(i = 1; i <= cd.no_of_discs->value(); i++)
    comboBoxList.append(QString::number(i));

  trd.table->clear();
  trd.table->setCurrentItem(NULL);
  trd.table->setColumnCount(0);
  trd.table->setRowCount(0);
  list.append("Album Number");
  list.append("Track Number");
  list.append("Track Title");
  list.append("Track Runtime");
  trd.table->setColumnCount(list.size());
  trd.table->setHorizontalHeaderLabels(list);
  list.clear();
  trd.table->setRowCount(0);
  trd.table->scrollToTop();
  trd.table->horizontalScrollBar()->setValue(0);
  tracks_diag->updateGeometry();
  tracks_diag->setWindowTitle
    (QString("BiblioteQ: Album Tracks Browser (%1)").arg(cd.id->text()));
  tracks_diag->resize(tracks_diag->sizeHint());
  tracks_diag->show();
  trd.table->setSortingEnabled(false);

  if(qmain->getDB().driverName() != "QSQLITE")
    trd.table->setRowCount(query.size());

  progress.setModal(true);
  progress.setWindowTitle("BiblioteQ: Progress Dialog");
  progress.setLabelText("Populating the table...");

  if(qmain->getDB().driverName() == "QSQLITE")
    progress.setMaximum(misc_functions::sqliteQuerySize(querystr,
							qmain->getDB(),
							__FILE__, __LINE__));
  else
    progress.setMaximum(query.size());

  progress.show();
  progress.update();
  i = -1;

  while(i++, !progress.wasCanceled() && query.next())
    {
      if(query.isValid())
	for(j = 0; j < query.record().count(); j++)
	  {
	    str = query.value(j).toString();

	    if(qmain->getDB().driverName() == "QSQLITE")
	      trd.table->setRowCount(i + 1);

	    if(j == 0)
	      {
		if((comboBox = new QComboBox()) != NULL)
		  {
		    comboBox->addItems(comboBoxList);
		    comboBox->setCurrentIndex(comboBox->findText(str));
		    trd.table->setCellWidget(i, j, comboBox);
		  }
		else
		  qmain->addError(QString("Memory Error"),
				  QString("Unable to allocate memory for the "
					  "\"comboBox\" object. "
					  "This is a serious problem!"),
				  QString(""), __FILE__, __LINE__);
	      }
	    else if(j == 1)
	      {
		if((trackEdit = new QSpinBox()) != NULL)
		  {
		    trackEdit->setMinimum(1);
		    trackEdit->setValue(str.toInt());
		    trd.table->setCellWidget(i, j, trackEdit);
		  }
		else
		  qmain->addError(QString("Memory Error"),
				  QString("Unable to allocate memory for the "
					  "\"trackEdit\" object. "
					  "This is a serious problem!"),
				  QString(""), __FILE__, __LINE__);
	      }
	    else if(j == 3)
	      {
		if((timeEdit = new QTimeEdit()) != NULL)
		  {
		    timeEdit->setDisplayFormat("hh:mm:ss");
		    timeEdit->setTime(QTime::fromString(str, "hh:mm:ss"));
		    trd.table->setCellWidget(i, j, timeEdit);
		  }
		else
		  qmain->addError(QString("Memory Error"),
				  QString("Unable to allocate memory for the "
					  "\"timeEdit\" object. "
					  "This is a serious problem!"),
				  QString(""), __FILE__, __LINE__);
	      }
	    else if((item = new QTableWidgetItem()) != NULL)
	      {
		item->setText(str);
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled |
			       Qt::ItemIsEditable);
		trd.table->setItem(i, j, item);
	      }
	    else
	      qmain->addError(QString("Memory Error"),
			      QString("Unable to allocate memory for the "
				      "\"item\" object. "
				      "This is a serious problem!"),
			      QString(""), __FILE__, __LINE__);

	    trd.table->resizeColumnToContents(i);
	  }

      progress.setValue(i + 1);
      progress.update();
      qapp->processEvents();
    }

  trd.table->setSortingEnabled(false);
  comboBoxList.clear();
  query.clear();
  trd.table->setRowCount(i);
  trd.table->resizeColumnsToContents();
}

/*
** -- slotCloseTracksBrowser() --
*/

void qtbook_cd::slotCloseTracksBrowser(void)
{
  trd.table->clear();
  trd.table->setCurrentItem(NULL);
  trd.table->setRowCount(0);
  tracks_diag->close();
}

/*
** -- slotInsertTrack() --
*/

void qtbook_cd::slotInsertTrack(void)
{
  int i = 0;
  int trow = trd.table->currentRow();
  QString str = "";
  QSpinBox *trackEdit = NULL;
  QStringList list;
  QComboBox *comboBox = NULL;
  QTimeEdit *timeEdit = NULL;
  QTableWidgetItem *item = NULL;

  if(trow < 0)
    trow = trd.table->rowCount();
  else
    trow += 1;

  trd.table->insertRow(trow);
  trd.table->setSortingEnabled(false);

  for(i = 1; i <= cd.no_of_discs->value(); i++)
    list.append(QString::number(i));

  for(i = 0; i < 4; i++)
    {
      if(i == 1)
	str = "1";
      else if(i == 2)
	str = "Title";

      if(i == 0)
	{
	  if((comboBox = new QComboBox()) != NULL)
	    {
	      comboBox->addItems(list);
	      trd.table->setCellWidget(trow, i, comboBox);
	    }
	  else
	    qmain->addError(QString("Memory Error"),
			    QString("Unable to allocate memory for the "
				    "\"comboBox\" object. "
				    "This is a serious problem!"),
			    QString(""), __FILE__, __LINE__);
	}
      else if(i == 1)
	{
	  if((trackEdit = new QSpinBox()) != NULL)
	    {
	      trackEdit->setMinimum(1);
	      trackEdit->setValue(1);
	      trd.table->setCellWidget(trow, i, trackEdit);
	    }
	  else
	    qmain->addError(QString("Memory Error"),
			    QString("Unable to allocate memory for the "
				    "\"trackEdit\" object. "
				    "This is a serious problem!"),
			    QString(""), __FILE__, __LINE__);
	}
      else if(i == 3)
	{
	  if((timeEdit = new QTimeEdit()) != NULL)
	    {
	      timeEdit->setDisplayFormat("hh:mm:ss");
	      trd.table->setCellWidget(trow, i, timeEdit);
	    }
	  else
	    qmain->addError(QString("Memory Error"),
			    QString("Unable to allocate memory for the "
				    "\"timeEdit\" object. "
				    "This is a serious problem!"),
			    QString(""), __FILE__, __LINE__);
	}
      else if((item = new QTableWidgetItem()) != NULL)
	{
	  item->setText(str);
	  item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled |
			 Qt::ItemIsEditable);
	  trd.table->setItem(trow, i, item);
	}
      else
	qmain->addError(QString("Memory Error"),
			QString("Unable to allocate memory for the "
				"\"item\" object. "
				"This is a serious problem!"),
			QString(""), __FILE__, __LINE__);

      trd.table->resizeColumnToContents(i);
    }

  list.clear();
  trd.table->setSortingEnabled(false);
  trd.table->resizeColumnsToContents();
}

/*
** -- slotDeleteTrack() --
*/

void qtbook_cd::slotDeleteTrack(void)
{
  trd.table->removeRow(trd.table->currentRow());
}

/*
** -- slotSaveTracks() --
*/

void qtbook_cd::slotSaveTracks(void)
{
  int i = 0;
  QString errormsg = "";
  QString lastError = "";
  QSqlQuery query(qmain->getDB());
  QProgressDialog progress(this);

  for(i = 0; i < trd.table->rowCount(); i++)
    if(trd.table->item(i, 2) != NULL &&
       trd.table->item(i, 2)->text().trimmed().isEmpty())
      {
	errormsg = QString("Row number %1 contains an empty Song Title.").arg
	  (i + 1);
	QMessageBox::critical(tracks_diag, "BiblioteQ: User Error", errormsg);
	return;
      }

  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!qmain->getDB().transaction())
    {
      qapp->restoreOverrideCursor();
      qmain->addError(QString("Database Error"),
		      QString("Unable to create a database transaction."),
		      qmain->getDB().lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(tracks_diag, "BiblioteQ: Database Error",
			    "Unable to create a database transaction.");
      return;
    }

  qapp->restoreOverrideCursor();
  query.prepare(QString("DELETE FROM cd_songs WHERE "
			"item_oid = ?"));
  query.bindValue(0, oid);
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
    {
      qapp->restoreOverrideCursor();
      qmain->addError(QString("Database Error"),
		      QString("Unable to purge track data."),
		      query.lastError().text(), __FILE__, __LINE__);
      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!qmain->getDB().rollback())
	qmain->addError(QString("Database Error"),
			QString("Rollback failure."),
			qmain->getDB().lastError().text(),
			__FILE__, __LINE__);

      qapp->restoreOverrideCursor();
    }
  else
    {
      qapp->restoreOverrideCursor();
      progress.setModal(true);
      progress.setWindowTitle("BiblioteQ: Progress Dialog");
      progress.setLabelText("Saving the track data...");
      progress.setMaximum(trd.table->rowCount());
      progress.show();
      progress.update();

      for(i = 0; i < trd.table->rowCount(); i++)
	{
	  query.prepare(QString("INSERT INTO cd_songs ("
				"item_oid, "
				"albumnum, "
				"songnum, "
				"songtitle, "
				"runtime"
				") "
				"VALUES (?, "
				"?, ?, ?, ?)"));
	  query.bindValue(0, oid);

	  if(trd.table->cellWidget(i, 0) != NULL)
	    query.bindValue(1, qobject_cast<QComboBox *>
			    (trd.table->cellWidget(i, 0))->currentText());

	  if(trd.table->cellWidget(i, 1) != NULL)
	    query.bindValue(2, qobject_cast<QSpinBox *>
			    (trd.table->cellWidget(i, 1))->value());

	  if(trd.table->item(i, 2) != NULL)
	    query.bindValue(3, trd.table->item(i, 2)->text().trimmed());

	  if(trd.table->cellWidget(i, 3) != NULL)
	    query.bindValue(4, qobject_cast<QTimeEdit *>
			    (trd.table->cellWidget(i, 3))->time().toString
			    ("hh:mm:ss"));

	  if(!query.exec())
	    {
	      qmain->addError(QString("Database Error"),
			      QString("Unable to create track data."),
			      query.lastError().text(), __FILE__, __LINE__);
	      lastError = query.lastError().text();
	    }

	  progress.setValue(i + 1);
	  progress.update();
	  qapp->processEvents();
	}

      progress.hide();

      if(!lastError.isEmpty())
	QMessageBox::critical(tracks_diag, "BiblioteQ: Database Error",
			      "Some or all of the track data has not "
			      "been saved. Please review the Error Log.");

      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!qmain->getDB().commit())
	{
	  qapp->restoreOverrideCursor();
	  qmain->addError(QString("Database Error"),
			  QString("Commit failure."),
			  qmain->getDB().lastError().text(), __FILE__,
			  __LINE__);
	  QMessageBox::critical(tracks_diag, "BiblioteQ: Database Error",
				"Unable to commit the track data.");
	}

      qapp->restoreOverrideCursor();
    }
}

/*
** -- slotReset() --
*/

void qtbook_cd::slotReset(void)
{
  QAction *action = qobject_cast<QAction *>(sender());
  QString name = "";

  if(action != NULL)
    {
      name = action->text();

      if(name.contains("Front Cover Image"))
	cd.front_image->clear();
      else if(name.contains("Back Cover Image"))
	cd.back_image->clear();
      else if(name.contains("Catalog Number"))
	{
	  cd.id->clear();
	  cd.id->setFocus();
	}
      else if(name.contains("Title"))
	{
	  cd.title->clear();
	  cd.title->setFocus();
	}
      else if(name.contains("Format"))
	{
	  cd.format->setCurrentIndex(0);
	  cd.format->setFocus();
	}
      else if(name.contains("Artist"))
	{
	  cd.artist->clear();
	  cd.artist->setFocus();
	}
      else if(name.contains("Number of Discs"))
	{
	  cd.no_of_discs->setValue(cd.no_of_discs->minimum());
	  cd.no_of_discs->setFocus();
	}
      else if(name.contains("Runtime"))
	{
	  if(windowTitle().contains("Search"))
	    cd.runtime->setTime(QTime(0, 0, 0));
	  else
	    cd.runtime->setTime(QTime(0, 0, 1));

	  cd.runtime->setFocus();
	}
      else if(name.contains("Release Date"))
	{
	  if(windowTitle().contains("Search"))
	    cd.release_date->setDate
	      (QDate::fromString("01/01/7999", "MM/dd/yyyy"));
	  else
	    cd.release_date->setDate
	      (QDate::fromString("01/01/2000", "MM/dd/yyyy"));

	  cd.release_date->setFocus();
	}
      else if(name.contains("Audio"))
	{
	  cd.audio->setCurrentIndex(0);
	  cd.audio->setFocus();
	}
      else if(name.contains("Recording Type"))
	{
	  cd.recording_type->setCurrentIndex(0);
	  cd.recording_type->setFocus();
	}
      else if(name.contains("Recording Label"))
	{
	  cd.recording_label->clear();
	  cd.recording_label->setFocus();
	}
      else if(name.contains("Category"))
	{
	  cd.category->setCurrentIndex(0);
	  cd.category->setFocus();
	}
      else if(name.contains("Price"))
	{
	  cd.price->setValue(cd.price->minimum());
	  cd.price->setFocus();
	}
      else if(name.contains("Language"))
	{
	  cd.language->setCurrentIndex(0);
	  cd.language->setFocus();
	}
      else if(name.contains("Monetary Units"))
	{
	  cd.monetary_units->setCurrentIndex(0);
	  cd.monetary_units->setFocus();
	}
      else if(name.contains("Abstract"))
	{
	  cd.description->clear();
	  cd.description->setFocus();
	}
      else if(name.contains("Copies"))
	{
	  cd.quantity->setValue(cd.quantity->minimum());
	  cd.quantity->setFocus();
	}
      else if(name.contains("Location"))
	{
	  cd.location->setCurrentIndex(0);
	  cd.location->setFocus();
	}
      else if(name.contains("OFFSYSTEM URL"))
	{
	  cd.url->clear();
	  cd.url->setFocus();
	}
    }
  else
    {
      /*
      ** Reset all.
      */

      cd.title->clear();
      cd.recording_label->clear();

      if(windowTitle().contains("Search"))
	{
	  cd.runtime->setTime(QTime(0, 0, 0));
	  cd.release_date->setDate(QDate::fromString("01/01/7999",
						     "MM/dd/yyyy"));
	}
      else
	{
	  cd.runtime->setTime(QTime(0, 0, 1));
	  cd.release_date->setDate(QDate::fromString("01/01/2000",
						     "MM/dd/yyyy"));
	}

      cd.id->clear();
      cd.artist->clear();
      cd.price->setValue(cd.price->minimum());
      cd.quantity->setValue(cd.quantity->minimum());
      cd.no_of_discs->setValue(cd.no_of_discs->minimum());
      cd.description->clear();
      cd.audio->setCurrentIndex(0);
      cd.location->setCurrentIndex(0);
      cd.category->setCurrentIndex(0);
      cd.language->setCurrentIndex(0);
      cd.monetary_units->setCurrentIndex(0);
      cd.recording_type->setCurrentIndex(0);
      cd.format->setCurrentIndex(0);
      cd.front_image->clear();
      cd.back_image->clear();
      cd.url->clear();
      cd.id->setFocus();
    }
}

/*
** -- closeEvent() --
*/

void qtbook_cd::closeEvent(QCloseEvent *e)
{
  (void) e;
  qmain->removeCD(this);
}

/*
** -- slotCancel() --
*/

void qtbook_cd::slotCancel(void)
{
  close();
  qmain->removeCD(this);
}

/*
** -- slotPopulateCopiesEditor() --
*/

void qtbook_cd::slotPopulateCopiesEditor(void)
{
  copy_editor *copyeditor = NULL;

  if((copyeditor = new copy_editor(qobject_cast<QWidget *>(this),
				   (qtbook_item *) this,
				   false,
				   cd.quantity->value(), oid,
				   cd.id->text(),
				   cd.quantity, font(), "CD",
				   cd.title->text().trimmed(),
				   "CD")) != NULL)
    copyeditor->populateCopiesEditor();
}

/*
** -- slotShowUsers() --
*/

void qtbook_cd::slotShowUsers(void)
{
  int state = 0;
  borrowers_editor *borrowerseditor = NULL;

  if(!cd.okButton->isHidden())
    state = qtbook::EDITABLE;
  else
    state = qtbook::VIEW_ONLY;

  if((borrowerseditor = new borrowers_editor
      (qobject_cast<QWidget *>(this), (qtbook_item *) this,
       cd.quantity->value(), oid, cd.id->text(), font(), "CD",
       state)) != NULL)
    borrowerseditor->showUsers();
}

/*
** -- slotQuery() --
*/

void qtbook_cd::slotQuery(void)
{
}

/*
** -- slotComputeRuntime() --
*/

void qtbook_cd::slotComputeRuntime(void)
{
  int secs = 0;
  QTime sum(0, 0, 0);
  QTime time(0, 0, 0);
  QString querystr = "";
  QSqlQuery query(qmain->getDB());

  querystr = "SELECT runtime FROM cd_songs WHERE item_oid = ";
  querystr.append(oid);
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(query.exec(querystr))
    while(query.next())
      {
	time = QTime::fromString(query.value(0).toString(), "hh:mm:ss");
	secs = time.hour() * 3600 + time.minute() * 60 + time.second();
	sum = sum.addSecs(secs);
      }

  qapp->restoreOverrideCursor();

  if(sum.toString("hh:mm:ss") == "00:00:00")
    QMessageBox::critical(this, "BiblioteQ: Error",
			  "The total runtime of the available tracks is "
			  "zero. Please set the individual runtimes.");
  else
    cd.runtime->setTime(QTime::fromString(sum.toString("hh:mm:ss"),
					  "hh:mm:ss"));
}

/*
** -- slotPrint() --
*/

void qtbook_cd::slotPrint(void)
{
  html = "";
  html += "<b>Catalog Number:</b> " + cd.id->text() + "<br>";
  html += "<b>Format:</b> " + cd.format->currentText() + "<br>";
  html += "<b>Artist:</b> " + cd.artist->toPlainText() + "<br>";
  html += "<b>Number of Discs:</b> " + cd.no_of_discs->text() + "<br>";
  html += "<b>Runtime:</b> " + cd.runtime->text() + "<br>";
  html += "<b>Audio:</b> " + cd.audio->currentText() + "<br>";
  html += "<b>Recording Type:</b> " + cd.recording_type->currentText() +
    "<br>";

  /*
  ** General information.
  */

  html += "<b>Title:</b> " + cd.title->text() + "<br>";
  html += "<b>Release Date:</b> " + cd.release_date->date().
    toString("MM/dd/yyyy") + "<br>";
  html += "<b>Recording Label:</b> " + cd.recording_label->toPlainText() +
    "<br>";
  html += "<b>Category:</b> " + cd.category->currentText() + "<br>";
  html += "<b>Price:</b> " + cd.price->text() + "<br>";
  html += "<b>Language:</b> " + cd.language->currentText() + "<br>";
  html += "<b>Monetary Units:</b> " + cd.monetary_units->currentText() +
    "<br>";
  html += "<b>Copies:</b> " + cd.quantity->text() + "<br>";
  html += "<b>Location:</b> " + cd.location->currentText() + "<br>";
  html += "<b>Abstract:</b> " + cd.description->toPlainText() + "<br>";
  html += "<b>OFFSYSTEM URL:</b> " + cd.url->toPlainText();
  print(this);
}

/*
** -- slotSelectImage() --
*/

void qtbook_cd::slotSelectImage(void)
{
  QFileDialog dialog(this);
  QPushButton *button = qobject_cast<QPushButton *> (sender());

  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setFilter("Image Files (*.bmp *.jpg *.jpeg *.png)");

  if(button == cd.frontButton)
    dialog.setWindowTitle("Front Cover Image Selection");
  else
    dialog.setWindowTitle("Back Cover Image Selection");

  dialog.exec();

  if(dialog.result() == QDialog::Accepted)
    if(button == cd.frontButton)
      {
	cd.front_image->clear();
	cd.front_image->image = QImage(dialog.selectedFiles().at(0));
	cd.front_image->imageFormat = dialog.selectedFiles().at(0).mid
	  (dialog.selectedFiles().at(0).lastIndexOf(".") + 1).toUpper();
	cd.front_image->scene()->addPixmap
	  (QPixmap().fromImage(cd.front_image->image));
	cd.front_image->scene()->items().at(0)->setFlags
	  (QGraphicsItem::ItemIsSelectable);
      }
    else
      {
	cd.back_image->clear();
	cd.back_image->image = QImage(dialog.selectedFiles().at(0));
	cd.back_image->imageFormat = dialog.selectedFiles().at(0).mid
	  (dialog.selectedFiles().at(0).lastIndexOf(".") + 1).toUpper();
	cd.back_image->scene()->addPixmap
	  (QPixmap().fromImage(cd.back_image->image));
	cd.back_image->scene()->items().at(0)->setFlags
	  (QGraphicsItem::ItemIsSelectable);
      }
}
