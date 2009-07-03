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
		     const QStringList &languages,
		     const QStringList &monetary_units,
		     const QStringList &locations,
		     const QStringList &formats,
		     const QString &oidArg,
		     const int rowArg):
  QMainWindow()
{
  QMenu *menu = 0;
  QRegExp rx1("[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]");
  QValidator *validator1 = 0;
  QGraphicsScene *scene1 = 0;
  QGraphicsScene *scene2 = 0;

  if((menu = new(std::nothrow) QMenu()) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((tracks_diag = new(std::nothrow) QDialog(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((validator1 = new(std::nothrow) QRegExpValidator(rx1, this)) == 0)
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
    (qmain->getUI().table, row, tr("Quantity")).toInt();
  cd.setupUi(this);
  updateFont(qapp->font(), static_cast<QWidget *> (this));
  tracks_diag->setWindowModality(Qt::WindowModal);
  trd.setupUi(tracks_diag);
  trd.table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
  updateFont(qapp->font(), static_cast<QWidget *> (tracks_diag));
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
  connect(menu->addAction(tr("Reset &Front Cover Image")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Back Cover Image")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Catalog Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Format")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Artist")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Number of Discs")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Runtime")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Audio")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Recording Type")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Title")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Release Date")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Recording Label")),
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
  connect(cd.frontButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(cd.backButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  cd.queryButton->setVisible(isQueryEnabled);
  cd.resetButton->setMenu(menu);
  cd.id->setValidator(validator1);
  cd.language->addItems(languages);
  cd.monetary_units->addItems(monetary_units);
  cd.location->addItems(locations);
  cd.format->addItems(formats);
  cd.front_image->setScene(scene1);
  cd.back_image->setScene(scene2);

  if(cd.language->count() == 0)
    cd.language->addItem(tr("UNKNOWN"));

  if(cd.monetary_units->count() == 0)
    cd.monetary_units->addItem(tr("UNKNOWN"));

  if(cd.location->count() == 0)
    cd.location->addItem(tr("UNKNOWN"));

  if(cd.format->count() == 0)
    cd.format->addItem(tr("UNKNOWN"));

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
  QTableWidgetItem *column = 0;

  if(engWindowTitle.contains("Create") ||
     engWindowTitle.contains("Modify"))
    {
      if(engWindowTitle.contains("Modify") && row > -1)
	{
	  newq = cd.quantity->value();
	  qapp->setOverrideCursor(Qt::WaitCursor);
	  maxcopynumber = misc_functions::getMaxCopyNumber
	    (qmain->getDB(), oid, "CD", errorstr);

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
		 tr("It appears that you are attempting to decrease the "
		    "number of copies while there are copies "
		    "that have been reserved."));
	      cd.quantity->setValue(oldq);
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
      str = cd.id->text().trimmed();
      cd.id->setText(str);

      if(cd.id->text().isEmpty())
	{
	  QMessageBox::critical
	    (this, tr("BiblioteQ: User Error"),
	     tr("Please complete the Catalog Number field."));
	  cd.id->setFocus();
	  goto db_rollback;
	}

      str = cd.artist->toPlainText().trimmed();
      cd.artist->setPlainText(str);

      if(cd.artist->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Artist field."));
	  cd.artist->setFocus();
	  goto db_rollback;
	}

      str = cd.title->text().trimmed();
      cd.title->setText(str);

      if(cd.title->text().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Title field."));
	  cd.title->setFocus();
	  goto db_rollback;
	}

      if(cd.runtime->text() == "00:00:00")
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please provide a valid Runtime."));
	  cd.runtime->setFocus();
	  goto db_rollback;
	}

      str = cd.recording_label->toPlainText().trimmed();
      cd.recording_label->setPlainText(str);

      if(cd.recording_label->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Recording Label "
				   "field."));
	  cd.recording_label->setFocus();
	  goto db_rollback;
	}

      str = cd.category->toPlainText().trimmed();
      cd.category->setPlainText(str);

      if(cd.category->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Categories "
				   "field."));
	  cd.category->setFocus();
	  goto db_rollback;
	}

      str = cd.description->toPlainText().trimmed();
      cd.description->setPlainText(str);

      if(cd.description->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Abstract field."));
	  cd.description->setFocus();
	  goto db_rollback;
	}

      if(engWindowTitle.contains("Modify"))
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
			      "back_cover = ? "
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
			      "back_cover) "
			      "VALUES "
			      "(?, ?, "
			      "?, ?, "
			      "?, ?, ?, "
			      "?, ?, ?, "
			      "?, "
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
			      "myoid) "
			      "VALUES "
			      "(?, ?, "
			      "?, ?, "
			      "?, ?, "
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
      query.bindValue(8, cd.category->toPlainText());
      query.bindValue(9, cd.price->value());
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

      if(engWindowTitle.contains("Modify"))
	query.bindValue(19, oid);
      else if(qmain->getDB().driverName() == "QSQLITE")
	query.bindValue(19, cd.id->text());

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
		    (QString(tr("Database Error")),
		     QString(tr("Unable to purge unnecessary copy "
				"data.")),
		     query.lastError().text(), __FILE__, __LINE__);
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

	      misc_functions::createInitialCopies(cd.id->text(),
						  cd.quantity->value(),
						  qmain->getDB(),
						  "CD", errorstr);

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
	  cd.category->setMultipleLinks("cd_search", "category",
					cd.category->toPlainText());
	  qapp->restoreOverrideCursor();

	  if(engWindowTitle.contains("Modify"))
	    {
	      str = QString(tr("BiblioteQ: Modify CD Entry (")) +
		cd.id->text() + tr(")");
	      engWindowTitle = "Modify";
	      setWindowTitle(str);

	      if((qmain->getUI().typefilter->currentText() == tr("All") ||
		  qmain->getUI().typefilter->currentText() ==
		  tr("All Overdue") ||
		  qmain->getUI().typefilter->currentText() ==
		  tr("All Requested") ||
		  qmain->getUI().typefilter->currentText() ==
		  tr("All Reserved") ||
		  qmain->getUI().typefilter->currentText() ==
		  tr("Music CDs")) &&
		 oid == misc_functions::getColumnString(qmain->getUI().table,
							row, "MYOID"))
		{
		  for(i = 0; i < qmain->getUI().table->columnCount(); i++)
		    {
		      column = qmain->getUI().table->horizontalHeaderItem(i);

		      if(column == 0)
			continue;

		      if(column->text() == tr("Catalog Number") ||
			 column->text() == tr("ID Number"))
			qmain->getUI().table->item(row, i)->setText
			  (cd.id->text());
		      else if(column->text() == tr("Title"))
			qmain->getUI().table->item(row, i)->setText
			  (cd.title->text());
		      else if(column->text() == tr("Format"))
			qmain->getUI().table->item(row, i)->setText
			  (cd.format->currentText().trimmed());
		      else if(column->text() == tr("Artist"))
			qmain->getUI().table->item(row, i)->setText
			  (cd.artist->toPlainText());
		      else if(column->text() == tr("Number of Discs"))
			qmain->getUI().table->item(row, i)->setText
			  (cd.no_of_discs->text());
		      else if(column->text() == tr("Runtime"))
			qmain->getUI().table->item(row, i)->setText
			  (cd.runtime->text());
		      else if(column->text() == tr("Release Date") ||
			      column->text() == tr("Publication Date"))
			qmain->getUI().table->item(row, i)->setText
			  (cd.release_date->date().toString("MM/dd/yyyy"));
		      else if(column->text() == tr("Recording Label") ||
			      column->text() == tr("Publisher"))
			qmain->getUI().table->item(row, i)->setText
			  (cd.recording_label->toPlainText());
		      else if(column->text() == tr("Categories"))
			qmain->getUI().table->item(row, i)->setText
			  (cd.category->toPlainText().trimmed());
		      else if(column->text() == tr("Price"))
			qmain->getUI().table->item(row, i)->setText
			  (cd.price->text());
		      else if(column->text() == tr("Language"))
			qmain->getUI().table->item(row, i)->setText
			  (cd.language->currentText().trimmed());
		      else if(column->text() == tr("Monetary Units"))
			qmain->getUI().table->item(row, i)->setText
			  (cd.monetary_units->currentText().trimmed());
		      else if(column->text() == tr("Quantity"))
			qmain->getUI().table->item(row, i)->setText
			  (cd.quantity->text());
		      else if(column->text() == tr("Location"))
			qmain->getUI().table->item(row, i)->setText
			  (cd.location->currentText().trimmed());
		      else if(column->text() == tr("Recording Type"))
			qmain->getUI().table->item(row, i)->setText
			  (cd.recording_type->currentText().trimmed());
		      else if(column->text() == tr("Audio"))
			qmain->getUI().table->item(row, i)->setText
			  (cd.audio->currentText().trimmed());
		      else if(column->text() == tr("Availability"))
			{
			  qmain->getUI().table->item(row, i)->setText
			    (misc_functions::getAvailability
			     (oid, qmain->getDB(), "CD", errorstr));

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
	      oid = misc_functions::getOID(cd.id->text(),
					   "CD",
					   qmain->getDB(),
					   errorstr);
	      qapp->restoreOverrideCursor();

	      if(!errorstr.isEmpty())
		{
		  oid = "insert";
		  qmain->addError(QString(tr("Database Error")),
				  QString(tr("Unable to retrieve the CD's "
					     "OID.")),
				  errorstr, __FILE__, __LINE__);
		  QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
					tr("Unable to retrieve the CD's "
					   "OID."));
		}
	      else
		qmain->replaceCD("insert", this);

	      updateWindow(qtbook::EDITABLE);

	      if(qmain->getUI().actionAutoPopulateOnCreation->isChecked())
		(void) qmain->populateTable
		  (qtbook::POPULATE_ALL, tr("Music CDs"), QString(""));

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

      if(cd.format->currentText() != tr("Any"))
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

      if(cd.audio->currentText() != tr("Any"))
	searchstr.append("cdaudio = '" + cd.audio->currentText() +
			 "' AND ");

      if(cd.recording_type->currentText() != tr("Any"))
	searchstr.append("cdrecording = '" +
			 cd.recording_type->currentText() +
			 "' AND ");

      searchstr.append("LOWER(title) LIKE '%").append
	(myqstring::escape(cd.title->text().toLower())).append("%' AND ");

      if(cd.release_date->date().toString("MM/yyyy") != "01/7999")
	searchstr.append("SUBSTR(rdate, 1, 3) || SUBSTR(rdate, 7) = '" +
			 cd.release_date->date().toString
			 ("MM/yyyy") +
			 "' AND ");

      searchstr.append("LOWER(recording_label) LIKE '%" +
		       myqstring::escape(cd.recording_label->toPlainText().
					 toLower()) + "%' AND ");
      searchstr.append("LOWER(category) LIKE '%" +
		       myqstring::escape(cd.category->toPlainText().
					 toLower()) +
		       "%' AND ");

      if(cd.price->value() > -0.01)
	{
	  searchstr.append("price = ");
	  searchstr.append(QString::number(cd.price->value()));
	  searchstr.append(" AND ");
	}

      if(cd.language->currentText() != tr("Any"))
	searchstr.append("language = '" +
			 myqstring::escape(cd.language->currentText()) +
			 "' AND ");

      if(cd.monetary_units->currentText() != tr("Any"))
	searchstr.append("monetary_units = '" +
			 myqstring::escape
			 (cd.monetary_units->currentText()) +
			 "' AND ");

      searchstr.append("LOWER(description) LIKE '%" +
		       myqstring::escape
		       (cd.description->toPlainText().toLower()) + "%' ");

      if(cd.quantity->value() != 0)
	searchstr.append(" AND quantity = " + cd.quantity->text());

      if(cd.location->currentText() != tr("Any"))
	searchstr.append(" AND location = '" +
			 myqstring::escape
			 (cd.location->currentText()) + "' ");

      hide();

      /*
      ** Search the database.
      */

      (void) qmain->populateTable
	(qtbook::POPULATE_SEARCH, tr("Music CDs"), searchstr);
      slotCancel();
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
  cd.category->clear();
  cd.copiesButton->setVisible(false);
  cd.tracksButton->setVisible(false);
  cd.queryButton->setVisible(false);
  cd.showUserButton->setVisible(false);
  cd.computeButton->setVisible(false);
  cd.tracks_lbl->setVisible(false);
  cd.okButton->setText(tr("&Search"));
  cd.release_date->setDate(QDate::fromString("01/7999",
					     "MM/yyyy"));
  cd.release_date->setDisplayFormat("MM/yyyy");
  cd.runtime->setTime(QTime(0, 0, 0));
  cd.runtime->setMinimumTime(QTime(0, 0, 0));
  cd.price->setMinimum(-0.01);
  cd.price->setValue(-0.01);
  cd.quantity->setMinimum(0);
  cd.quantity->setValue(0);
  cd.no_of_discs->setMinimum(0);
  cd.no_of_discs->setValue(0);

  if(cd.audio->findText(tr("Any")) == -1)
    cd.audio->insertItem(0, tr("Any"));

  if(cd.recording_type->findText(tr("Any")) == -1)
    cd.recording_type->insertItem(0, tr("Any"));

  if(cd.format->findText(tr("Any")) == -1)
    cd.format->insertItem(0, tr("Any"));

  if(cd.language->findText(tr("Any")) == -1)
    cd.language->insertItem(0, tr("Any"));

  if(cd.monetary_units->findText(tr("Any")) == -1)
    cd.monetary_units->insertItem(0, tr("Any"));

  if(cd.location->findText(tr("Any")) == -1)
    cd.location->insertItem(0, tr("Any"));

  cd.audio->setCurrentIndex(0);
  cd.location->setCurrentIndex(0);
  cd.language->setCurrentIndex(0);
  cd.monetary_units->setCurrentIndex(0);
  cd.recording_type->setCurrentIndex(0);
  cd.format->setCurrentIndex(0);

  if(field.isEmpty() && value.isEmpty())
    {
      QList<QAction *> actions = cd.resetButton->menu()->actions();

      actions[0]->setVisible(false);
      actions[1]->setVisible(false);
      actions.clear();
      cd.coverImages->setVisible(false);
      setWindowTitle(tr("BiblioteQ: Database CD Search"));
      engWindowTitle = "Search";
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
      else if(field == "category")
	cd.category->setPlainText(value);

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
      cd.computeButton->setVisible(true);
      trd.saveButton->setVisible(true);
      trd.insertButton->setVisible(true);
      trd.deleteButton->setVisible(true);
      cd.frontButton->setVisible(true);
      cd.backButton->setVisible(true);
      str = QString(tr("BiblioteQ: Modify CD Entry (")) +
	cd.id->text() + tr(")");
      engWindowTitle = "Modify";
    }
  else
    {
      cd.showUserButton->setEnabled(true);
      cd.copiesButton->setVisible(false);
      cd.okButton->setVisible(false);
      cd.queryButton->setVisible(false);
      cd.resetButton->setVisible(false);
      cd.computeButton->setVisible(false);
      trd.saveButton->setVisible(false);
      trd.insertButton->setVisible(false);
      trd.deleteButton->setVisible(false);      
      cd.frontButton->setVisible(false);
      cd.backButton->setVisible(false);
      str = QString(tr("BiblioteQ: View CD Details (")) +
	cd.id->text() + tr(")");
      engWindowTitle = "View";
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
      setWindowTitle(tr("BiblioteQ: Modify CD Entry"));
      engWindowTitle = "Modify";
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
      misc_functions::highlightWidget
	(cd.category->viewport(), QColor(255, 248, 220));
    }
  else
    {
      setWindowTitle(tr("BiblioteQ: View CD Details"));
      engWindowTitle = "View";
      cd.showUserButton->setEnabled(true);
      cd.copiesButton->setVisible(false);
      cd.okButton->setVisible(false);
      cd.queryButton->setVisible(false);
      cd.resetButton->setVisible(false);
      cd.computeButton->setVisible(false);
      trd.saveButton->setVisible(false);
      trd.insertButton->setVisible(false);
      trd.deleteButton->setVisible(false);      
      cd.frontButton->setVisible(false);
      cd.backButton->setVisible(false);

      QList<QAction *> actions = cd.resetButton->menu()->actions();

      actions[0]->setVisible(false);
      actions[1]->setVisible(false);
      actions.clear();
    }

  cd.tracksButton->setEnabled(true);
  cd.queryButton->setEnabled(true);
  cd.okButton->setText(tr("&Save"));
  cd.runtime->setMinimumTime(QTime(0, 0, 1));
  cd.price->setMinimum(0.00);
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
    "back_cover "
    "FROM "
    "cd "
    "WHERE myoid = ";
  searchstr.append(str);
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(searchstr) || !query.next())
    {
      qapp->restoreOverrideCursor();
      qmain->addError
	(QString(tr("Database Error")),
	 QString(tr("Unable to retrieve the selected CD's data.")),
	 query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to retrieve the selected CD's "
			       "data."));
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
	    cd.category->setMultipleLinks("cd_search", "category",
					  var.toString());
	  else if(fieldname == "language")
	    {
	      if(cd.language->findText(var.toString()) > -1)
		cd.language->setCurrentIndex
		  (cd.language->findText(var.toString()));
	      else
		cd.language->setCurrentIndex
		  (cd.language->findText(tr("UNKNOWN")));
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
		  (cd.monetary_units->findText(tr("UNKNOWN")));
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
		  (cd.location->findText(tr("UNKNOWN")));
	    }
	  else if(fieldname == "id")
	    {
	      if(state == qtbook::EDITABLE)
		{
		  str = QString(tr("BiblioteQ: Modify CD Entry (")) +
		    var.toString() + tr(")");
		  engWindowTitle = "Modify";
		}
	      else
		{
		  str = QString(tr("BiblioteQ: View CD Details (")) +
		    var.toString() + tr(")");
		  engWindowTitle = "View";
		}

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
		cd.front_image->loadFromData(var.toByteArray());
	    }
	  else if(fieldname == "back_cover")
	    {
	      if(!query.record().field(i).isNull())
		cd.back_image->loadFromData(var.toByteArray());
	    }
	}

      foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
	textfield->setCursorPosition(0);

      storeData(this);
    }

  cd.id->setFocus();
  raise();
}

/*
** -- insert() --
*/

void qtbook_cd::insert(void)
{
  slotReset();
  cd.id->clear();
  cd.artist->setPlainText("N/A");
  cd.title->clear();
  cd.recording_label->setPlainText("N/A");
  cd.description->setPlainText("N/A");
  cd.category->setPlainText("N/A");
  cd.copiesButton->setEnabled(false);
  cd.tracksButton->setEnabled(false);
  cd.showUserButton->setEnabled(false);
  cd.queryButton->setEnabled(true);
  cd.computeButton->setVisible(true);
  cd.okButton->setText(tr("&Save"));
  cd.release_date->setDate(QDate::fromString("01/01/2000",
					     "MM/dd/yyyy"));
  cd.runtime->setTime(QTime(0, 0, 1));
  cd.runtime->setMinimumTime(QTime(0, 0, 1));
  cd.price->setMinimum(0.00);
  cd.price->setValue(0.00);
  cd.quantity->setMinimum(1);
  cd.quantity->setValue(1);
  cd.no_of_discs->setMinimum(1);
  cd.no_of_discs->setValue(1);
  cd.audio->setCurrentIndex(0);
  cd.location->setCurrentIndex(0);
  cd.language->setCurrentIndex(0);
  cd.monetary_units->setCurrentIndex(0);
  cd.recording_type->setCurrentIndex(0);
  cd.format->setCurrentIndex(0);
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
  misc_functions::highlightWidget
    (cd.category->viewport(), QColor(255, 248, 220));
  setWindowTitle(tr("BiblioteQ: Create CD Entry"));
  engWindowTitle = "Create";
  cd.id->setFocus();
  storeData(this);
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
  QSpinBox *trackEdit = 0;
  QComboBox *comboBox = 0;
  QSqlQuery query(qmain->getDB());
  QTimeEdit * timeEdit = 0;
  QStringList list;
  QStringList comboBoxList;
  QProgressDialog progress(tracks_diag);
  QTableWidgetItem *item = 0;

  querystr = "SELECT albumnum, songnum, songtitle, runtime "
    "FROM cd_songs WHERE item_oid = ";
  querystr.append(oid);
  querystr.append(" ORDER BY albumnum, songnum, songtitle, runtime");
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(querystr))
    {
      qapp->restoreOverrideCursor();
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to retrieve track data for table "
				 "populating.")),
		      query.lastError().text(),
		      __FILE__, __LINE__);
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to retrieve track data for "
			       "table populating."));
      return;
    }

  qapp->restoreOverrideCursor();

  for(i = 1; i <= cd.no_of_discs->value(); i++)
    comboBoxList.append(QString::number(i));

  trd.table->clear();
  trd.table->setCurrentItem(0);
  trd.table->setColumnCount(0);
  trd.table->setRowCount(0);
  list.append(tr("Album Number"));
  list.append(tr("Track Number"));
  list.append(tr("Track Title"));
  list.append(tr("Track Runtime"));
  trd.table->setColumnCount(list.size());
  trd.table->setHorizontalHeaderLabels(list);
  list.clear();
  trd.table->setRowCount(0);
  trd.table->scrollToTop();
  trd.table->horizontalScrollBar()->setValue(0);
  tracks_diag->updateGeometry();
  tracks_diag->setWindowTitle
    (QString(tr("BiblioteQ: Album Tracks Browser (")) +
     cd.id->text() + tr(")"));
  tracks_diag->resize(tracks_diag->sizeHint());
  tracks_diag->show();
  trd.table->setSortingEnabled(false);

  if(qmain->getDB().driverName() != "QSQLITE")
    trd.table->setRowCount(query.size());

  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.setLabelText(tr("Populating the table..."));

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
		if((comboBox = new(std::nothrow) QComboBox()) != 0)
		  {
		    trd.table->setCellWidget(i, j, comboBox);
		    comboBox->addItems(comboBoxList);
		    comboBox->setCurrentIndex(comboBox->findText(str));
		  }
		else
		  qmain->addError
		    (QString(tr("Memory Error")),
		     QString(tr("Unable to allocate memory for the "
				"\"comboBox\" object. "
				"This is a serious problem!")),
		     QString(""), __FILE__, __LINE__);
	      }
	    else if(j == 1)
	      {
		if((trackEdit = new(std::nothrow) QSpinBox()) != 0)
		  {
		    trd.table->setCellWidget(i, j, trackEdit);
		    trackEdit->setMinimum(1);
		    trackEdit->setValue(str.toInt());
		  }
		else
		  qmain->addError
		    (QString(tr("Memory Error")),
		     QString(tr("Unable to allocate memory for the "
				"\"trackEdit\" object. "
				"This is a serious problem!")),
		     QString(""), __FILE__, __LINE__);
	      }
	    else if(j == 3)
	      {
		if((timeEdit = new(std::nothrow) QTimeEdit()) != 0)
		  {
		    trd.table->setCellWidget(i, j, timeEdit);
		    timeEdit->setDisplayFormat("hh:mm:ss");
		    timeEdit->setTime(QTime::fromString(str, "hh:mm:ss"));
		  }
		else
		  qmain->addError
		    (QString(tr("Memory Error")),
		     QString(tr("Unable to allocate memory for the "
				"\"timeEdit\" object. "
				"This is a serious problem!")),
		     QString(""), __FILE__, __LINE__);
	      }
	    else if((item = new(std::nothrow) QTableWidgetItem()) != 0)
	      {
		item->setText(str);
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled |
			       Qt::ItemIsEditable);
		trd.table->setItem(i, j, item);
	      }
	    else
	      qmain->addError(QString(tr("Memory Error")),
			      QString(tr("Unable to allocate memory for the "
					 "\"item\" object. "
					 "This is a serious problem!")),
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
  trd.table->setCurrentItem(0);
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
  QSpinBox *trackEdit = 0;
  QStringList list;
  QComboBox *comboBox = 0;
  QTimeEdit *timeEdit = 0;
  QTableWidgetItem *item = 0;

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
	  if((comboBox = new(std::nothrow) QComboBox()) != 0)
	    {
	      trd.table->setCellWidget(trow, i, comboBox);
	      comboBox->addItems(list);
	    }
	  else
	    qmain->addError(QString(tr("Memory Error")),
			    QString(tr("Unable to allocate memory for the "
				       "\"comboBox\" object. "
				       "This is a serious problem!")),
			    QString(""), __FILE__, __LINE__);
	}
      else if(i == 1)
	{
	  if((trackEdit = new(std::nothrow) QSpinBox()) != 0)
	    {
	      trd.table->setCellWidget(trow, i, trackEdit);
	      trackEdit->setMinimum(1);
	      trackEdit->setValue(1);
	    }
	  else
	    qmain->addError(QString(tr("Memory Error")),
			    QString(tr("Unable to allocate memory for the "
				       "\"trackEdit\" object. "
				       "This is a serious problem!")),
			    QString(""), __FILE__, __LINE__);
	}
      else if(i == 3)
	{
	  if((timeEdit = new(std::nothrow) QTimeEdit()) != 0)
	    {
	      trd.table->setCellWidget(trow, i, timeEdit);
	      timeEdit->setDisplayFormat("hh:mm:ss");
	    }
	  else
	    qmain->addError(QString(tr("Memory Error")),
			    QString(tr("Unable to allocate memory for the "
				       "\"timeEdit\" object. "
				       "This is a serious problem!")),
			    QString(""), __FILE__, __LINE__);
	}
      else if((item = new(std::nothrow) QTableWidgetItem()) != 0)
	{
	  item->setText(str);
	  item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled |
			 Qt::ItemIsEditable);
	  trd.table->setItem(trow, i, item);
	}
      else
	qmain->addError(QString(tr("Memory Error")),
			QString(tr("Unable to allocate memory for the "
				   "\"item\" object. "
				   "This is a serious problem!")),
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
    if(trd.table->item(i, 2) != 0 &&
       trd.table->item(i, 2)->text().trimmed().isEmpty())
      {
	errormsg = QString(tr("Row number ")) + QString::number(i + 1) +
	  tr(" contains an empty Song Title.");
	QMessageBox::critical(tracks_diag, tr("BiblioteQ: User Error"),
			      errormsg);
	return;
      }

  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!qmain->getDB().transaction())
    {
      qapp->restoreOverrideCursor();
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to create a database transaction.")),
		      qmain->getDB().lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(tracks_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to create a database transaction."));
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
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to purge track data.")),
		      query.lastError().text(), __FILE__, __LINE__);
      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!qmain->getDB().rollback())
	qmain->addError(QString(tr("Database Error")),
			QString(tr("Rollback failure.")),
			qmain->getDB().lastError().text(),
			__FILE__, __LINE__);

      qapp->restoreOverrideCursor();
    }
  else
    {
      qapp->restoreOverrideCursor();
      progress.setModal(true);
      progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
      progress.setLabelText(tr("Saving the track data..."));
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

	  if(trd.table->cellWidget(i, 0) != 0)
	    query.bindValue(1, static_cast<QComboBox *>
			    (trd.table->cellWidget(i, 0))->currentText());

	  if(trd.table->cellWidget(i, 1) != 0)
	    query.bindValue(2, static_cast<QSpinBox *>
			    (trd.table->cellWidget(i, 1))->value());

	  if(trd.table->item(i, 2) != 0)
	    query.bindValue(3, trd.table->item(i, 2)->text().trimmed());

	  if(trd.table->cellWidget(i, 3) != 0)
	    query.bindValue(4, static_cast<QTimeEdit *>
			    (trd.table->cellWidget(i, 3))->time().toString
			    ("hh:mm:ss"));

	  if(!query.exec())
	    {
	      qmain->addError(QString(tr("Database Error")),
			      QString(tr("Unable to create track data.")),
			      query.lastError().text(), __FILE__, __LINE__);
	      lastError = query.lastError().text();
	    }

	  progress.setValue(i + 1);
	  progress.update();
	  qapp->processEvents();
	}

      progress.hide();

      if(!lastError.isEmpty())
	QMessageBox::critical(tracks_diag, tr("BiblioteQ: Database Error"),
			      tr("Some or all of the track data has not "
				 "been saved. Please review the Error Log."));

      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!qmain->getDB().commit())
	{
	  qapp->restoreOverrideCursor();
	  qmain->addError(QString(tr("Database Error")),
			  QString(tr("Commit failure.")),
			  qmain->getDB().lastError().text(), __FILE__,
			  __LINE__);
	  QMessageBox::critical(tracks_diag, tr("BiblioteQ: Database Error"),
				tr("Unable to commit the track data."));
	}

      qapp->restoreOverrideCursor();
    }
}

/*
** -- slotReset() --
*/

void qtbook_cd::slotReset(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(action != 0)
    {
      QList<QAction *> actions = cd.resetButton->menu()->actions();

      if(action == actions[0])
	cd.front_image->clear();
      else if(action == actions[1])
	cd.back_image->clear();
      else if(action == actions[2])
	{
	  cd.id->clear();
	  cd.id->setFocus();
	}
      else if(action == actions[9])
	{
	  cd.title->clear();
	  cd.title->setFocus();
	}
      else if(action == actions[3])
	{
	  cd.format->setCurrentIndex(0);
	  cd.format->setFocus();
	}
      else if(action == actions[4])
	{
	  if(engWindowTitle.contains("Search"))
	    cd.artist->clear();
	  else
	    cd.artist->setPlainText("N/A");

	  cd.artist->setFocus();
	}
      else if(action == actions[5])
	{
	  cd.no_of_discs->setValue(cd.no_of_discs->minimum());
	  cd.no_of_discs->setFocus();
	}
      else if(action == actions[6])
	{
	  if(engWindowTitle.contains("Search"))
	    cd.runtime->setTime(QTime(0, 0, 0));
	  else
	    cd.runtime->setTime(QTime(0, 0, 1));

	  cd.runtime->setFocus();
	}
      else if(action == actions[10])
	{
	  if(engWindowTitle.contains("Search"))
	    cd.release_date->setDate
	      (QDate::fromString("01/7999", "MM/yyyy"));
	  else
	    cd.release_date->setDate
	      (QDate::fromString("01/01/2000", "MM/dd/yyyy"));

	  cd.release_date->setFocus();
	}
      else if(action == actions[7])
	{
	  cd.audio->setCurrentIndex(0);
	  cd.audio->setFocus();
	}
      else if(action == actions[8])
	{
	  cd.recording_type->setCurrentIndex(0);
	  cd.recording_type->setFocus();
	}
      else if(action == actions[11])
	{
	  if(engWindowTitle.contains("Search"))
	    cd.recording_label->clear();
	  else
	    cd.recording_label->setPlainText("N/A");

	  cd.recording_label->setFocus();
	}
      else if(action == actions[12])
	{
	  if(engWindowTitle.contains("Search"))
	    cd.category->clear();
	  else
	    cd.category->setPlainText("N/A");

	  cd.category->setFocus();
	}
      else if(action == actions[13])
	{
	  cd.price->setValue(cd.price->minimum());
	  cd.price->setFocus();
	}
      else if(action == actions[14])
	{
	  cd.language->setCurrentIndex(0);
	  cd.language->setFocus();
	}
      else if(action == actions[15])
	{
	  cd.monetary_units->setCurrentIndex(0);
	  cd.monetary_units->setFocus();
	}
      else if(action == actions[18])
	{
	  if(engWindowTitle.contains("Search"))
	    cd.description->clear();
	  else
	    cd.description->setPlainText("N/A");

	  cd.description->setFocus();
	}
      else if(action == actions[16])
	{
	  cd.quantity->setValue(cd.quantity->minimum());
	  cd.quantity->setFocus();
	}
      else if(action == actions[17])
	{
	  cd.location->setCurrentIndex(0);
	  cd.location->setFocus();
	}

      actions.clear();
    }
  else
    {
      /*
      ** Reset all.
      */

      cd.title->clear();

      if(engWindowTitle.contains("Search"))
	cd.artist->clear();
      else
	cd.artist->setPlainText("N/A");

      if(engWindowTitle.contains("Search"))
	cd.recording_label->clear();
      else
	cd.recording_label->setPlainText("N/A");

      if(engWindowTitle.contains("Search"))
	cd.category->clear();
      else
	cd.category->setPlainText("N/A");

      if(engWindowTitle.contains("Search"))
	cd.description->clear();
      else
	cd.description->setPlainText("N/A");

      if(engWindowTitle.contains("Search"))
	{
	  cd.runtime->setTime(QTime(0, 0, 0));
	  cd.release_date->setDate(QDate::fromString("01/7999",
						     "MM/yyyy"));
	}
      else
	{
	  cd.runtime->setTime(QTime(0, 0, 1));
	  cd.release_date->setDate(QDate::fromString("01/01/2000",
						     "MM/dd/yyyy"));
	}

      cd.id->clear();
      cd.price->setValue(cd.price->minimum());
      cd.quantity->setValue(cd.quantity->minimum());
      cd.no_of_discs->setValue(cd.no_of_discs->minimum());
      cd.audio->setCurrentIndex(0);
      cd.location->setCurrentIndex(0);
      cd.language->setCurrentIndex(0);
      cd.monetary_units->setCurrentIndex(0);
      cd.recording_type->setCurrentIndex(0);
      cd.format->setCurrentIndex(0);
      cd.front_image->clear();
      cd.back_image->clear();
      cd.id->setFocus();
    }
}

/*
** -- closeEvent() --
*/

void qtbook_cd::closeEvent(QCloseEvent *e)
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

  qmain->removeCD(this);
}

/*
** -- slotCancel() --
*/

void qtbook_cd::slotCancel(void)
{
  close();
}

/*
** -- slotPopulateCopiesEditor() --
*/

void qtbook_cd::slotPopulateCopiesEditor(void)
{
  copy_editor *copyeditor = 0;

  if((copyeditor = new(std::nothrow) copy_editor
      (static_cast<QWidget *> (this),
       static_cast<qtbook_item *> (this),
       false,
       cd.quantity->value(), oid,
       cd.id->text(),
       cd.quantity, font(), "CD")) != 0)
    copyeditor->populateCopiesEditor();
}

/*
** -- slotShowUsers() --
*/

void qtbook_cd::slotShowUsers(void)
{
  int state = 0;
  borrowers_editor *borrowerseditor = 0;

  if(!cd.okButton->isHidden())
    state = qtbook::EDITABLE;
  else
    state = qtbook::VIEW_ONLY;

  if((borrowerseditor = new(std::nothrow) borrowers_editor
      (static_cast<QWidget *> (this), static_cast<qtbook_item *> (this),
       cd.quantity->value(), oid, cd.id->text(), font(), "CD",
       state)) != 0)
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
    QMessageBox::critical(this, tr("BiblioteQ: Error"),
			  tr("The total runtime of the available tracks is "
			     "zero. Please set the individual runtimes."));
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
  html += "<b>" + tr("Catalog Number:") + "</b> " +
    cd.id->text().trimmed() + "<br>";
  html += "<b>" + tr("Format:") + "</b> " + cd.format->currentText() + "<br>";
  html += "<b>" + tr("Artist:") + "</b> " +
    cd.artist->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Number of Discs:") + "</b> " +
    cd.no_of_discs->text() + "<br>";
  html += "<b>" + tr("Runtime:") + "</b> " + cd.runtime->text() + "<br>";
  html += "<b>" + tr("Audio:") + "</b> " + cd.audio->currentText() + "<br>";
  html += "<b>" + tr("Recording Type:") + "</b> " +
    cd.recording_type->currentText() + "<br>";

  /*
  ** General information.
  */

  html += "<b>" + tr("Title:") + "</b> " + cd.title->text().trimmed() + "<br>";
  html += "<b>" + tr("Release Date:") + "</b> " + cd.release_date->date().
    toString("MM/dd/yyyy") + "<br>";
  html += "<b>" + tr("Recording Label:") + "</b> " + cd.recording_label->
    toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Categories:") + "</b> " +
    cd.category->toPlainText().trimmed() + "<br>";
  html += "<b>"+ tr("Price:") + "</b> " + cd.price->text() + "<br>";
  html += "<b>" + tr("Language:") + "</b> " +
    cd.language->currentText() + "<br>";
  html += "<b>" + tr("Monetary Units:") + "</b> " +
    cd.monetary_units->currentText() + "<br>";
  html += "<b>" + tr("Copies:") + "</b> " + cd.quantity->text() + "<br>";
  html += "<b>" + tr("Location:") + "</b> " +
    cd.location->currentText() + "<br>";
  html += "<b>" + tr("Abstract:") + "</b> " +
    cd.description->toPlainText().trimmed() + "<br>";
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
  dialog.setNameFilter("Image Files (*.bmp *.jpg *.jpeg *.png)");

  if(button == cd.frontButton)
    dialog.setWindowTitle(tr("BiblioteQ: Front Cover Image Selection"));
  else
    dialog.setWindowTitle(tr("BiblioteQ: Back Cover Image Selection"));

  dialog.exec();

  if(dialog.result() == QDialog::Accepted)
    {
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
}
