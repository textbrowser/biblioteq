/*
** -- Qt Includes --
*/

#include <QSqlField>
#include <QSqlRecord>

/*
** Includes cd-specific methods.
*/

/*
** -- Local Includes --
*/

#include "biblioteq.h"
#include "biblioteq_borrowers_editor.h"
#include "biblioteq_cd.h"

extern biblioteq *qmain;

/*
** -- biblioteq_cd() --
*/

biblioteq_cd::biblioteq_cd(QMainWindow *parentArg,
			   const QString &oidArg,
			   const int rowArg):
  QMainWindow(), biblioteq_item(rowArg)
{
  QMenu *menu = 0;
  QRegExp rx1("[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]");
  QValidator *validator1 = 0;
  QGraphicsScene *scene1 = 0;
  QGraphicsScene *scene2 = 0;

  if((menu = new(std::nothrow) QMenu(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  if((m_tracks_diag = new(std::nothrow) QDialog(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  if((validator1 = new(std::nothrow) QRegExpValidator(rx1, this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene1 = new(std::nothrow) QGraphicsScene(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene2 = new(std::nothrow) QGraphicsScene(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  m_oid = oidArg;
  m_row = rowArg;
  m_isQueryEnabled = false;
  m_parentWid = parentArg;
  m_oldq = biblioteq_misc_functions::getColumnString
    (qmain->getUI().table, m_row,
     qmain->getUI().table->columnNumber("Quantity")).toInt();
  cd.setupUi(this);
#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
  m_tracks_diag->setAttribute
    (Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif
  updateFont(QApplication::font(), qobject_cast<QWidget *> (this));
  m_tracks_diag->setWindowModality(Qt::WindowModal);
  trd.setupUi(m_tracks_diag);
#if QT_VERSION >= 0x050000
  trd.table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
  trd.table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
#endif
  updateFont(QApplication::font(), qobject_cast<QWidget *> (m_tracks_diag));
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
  m_composer_action = menu->addAction(tr("Reset &Composer"));
  connect(m_composer_action, SIGNAL(triggered(void)),
	  this, SLOT(slotReset(void)));
  m_composer_action->setVisible(false);
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
  connect(menu->addAction(tr("Reset &Keywords")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(cd.frontButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(cd.backButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  cd.queryButton->setVisible(m_isQueryEnabled);
  cd.resetButton->setMenu(menu);
  cd.id->setValidator(validator1);
  cd.composer->setVisible(false);
  cd.composer_label->setVisible(false);

  QString errorstr("");

  QApplication::setOverrideCursor(Qt::WaitCursor);
  cd.language->addItems
    (biblioteq_misc_functions::getLanguages(qmain->getDB(),
					    errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the languages.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  cd.monetary_units->addItems
    (biblioteq_misc_functions::getMonetaryUnits(qmain->getDB(),
						errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the monetary units.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  cd.location->addItems
    (biblioteq_misc_functions::getLocations(qmain->getDB(),
					    "CD",
					    errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the cd locations.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  cd.format->addItems
    (biblioteq_misc_functions::getCDFormats(qmain->getDB(),
					    errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the cd formats.")),
       errorstr, __FILE__, __LINE__);

  cd.front_image->setScene(scene1);
  cd.back_image->setScene(scene2);

  if(cd.language->findText(tr("UNKNOWN")) == -1)
    cd.language->addItem(tr("UNKNOWN"));

  if(cd.monetary_units->findText(tr("UNKNOWN")) == -1)
    cd.monetary_units->addItem(tr("UNKNOWN"));

  if(cd.location->findText(tr("UNKNOWN")) == -1)
    cd.location->addItem(tr("UNKNOWN"));

  if(cd.format->findText(tr("UNKNOWN")) == -1)
    cd.format->addItem(tr("UNKNOWN"));

  if(m_parentWid)
    resize(qRound(0.95 * m_parentWid->size().width()),
	   qRound(0.95 * m_parentWid->size().height()));

  cd.splitter->setStretchFactor(0, 0);
  cd.splitter->setStretchFactor(1, 1);
  cd.splitter->setStretchFactor(2, 0);
  biblioteq_misc_functions::center(this, m_parentWid);
  biblioteq_misc_functions::hideAdminFields(this, qmain->getRoles());
}

/*
** -- ~biblioteq_cd() --
*/

biblioteq_cd::~biblioteq_cd()
{
}

/*
** -- slotGo() --
*/

void biblioteq_cd::slotGo(void)
{
  int i = 0;
  int newq = 0;
  int maxcopynumber = 0;
  QString str = "";
  QString errorstr = "";
  QString searchstr = "";
  QSqlQuery query(qmain->getDB());

  if(m_engWindowTitle.contains("Create") ||
     m_engWindowTitle.contains("Modify"))
    {
      if(m_engWindowTitle.contains("Modify") && m_row > -1)
	{
	  newq = cd.quantity->value();
	  QApplication::setOverrideCursor(Qt::WaitCursor);
	  maxcopynumber = biblioteq_misc_functions::getMaxCopyNumber
	    (qmain->getDB(), m_oid, "CD", errorstr);

	  if(maxcopynumber < 0)
	    {
	      QApplication::restoreOverrideCursor();
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

	  QApplication::restoreOverrideCursor();

	  if(newq < maxcopynumber)
	    {
	      QMessageBox::critical
		(this, tr("BiblioteQ: User Error"),
		 tr("It appears that you are attempting to decrease the "
		    "number of copies while there are copies "
		    "that have been reserved."));
	      cd.quantity->setValue(m_oldq);
	      return;
	    }
	  else if(newq > m_oldq)
	    if(QMessageBox::question
	       (this, tr("BiblioteQ: Question"),
		tr("You have increased the number of copies. "
		   "Would you like to modify copy information?"),
		QMessageBox::Yes | QMessageBox::No,
		QMessageBox::No) == QMessageBox::Yes)
	      slotPopulateCopiesEditor();
	}

      str = cd.id->text().trimmed();
      cd.id->setText(str);

      if(cd.id->text().isEmpty())
	{
	  QMessageBox::critical
	    (this, tr("BiblioteQ: User Error"),
	     tr("Please complete the Catalog Number field."));
	  cd.id->setFocus();
	  return;
	}

      str = cd.artist->toPlainText().trimmed();
      cd.artist->setPlainText(str);

      if(cd.artist->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Artist field."));
	  cd.artist->setFocus();
	  return;
	}

      if(cd.runtime->text() == "00:00:00")
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please provide a valid Runtime."));
	  cd.runtime->setFocus();
	  return;
	}

      str = cd.title->text().trimmed();
      cd.title->setText(str);

      if(cd.title->text().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Title field."));
	  cd.title->setFocus();
	  return;
	}

      str = cd.recording_label->toPlainText().trimmed();
      cd.recording_label->setPlainText(str);

      if(cd.recording_label->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Recording Label "
				   "field."));
	  cd.recording_label->setFocus();
	  return;
	}

      str = cd.category->toPlainText().trimmed();
      cd.category->setPlainText(str);

      if(cd.category->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Categories "
				   "field."));
	  cd.category->setFocus();
	  return;
	}

      str = cd.description->toPlainText().trimmed();
      cd.description->setPlainText(str);

      if(cd.description->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Abstract field."));
	  cd.description->setFocus();
	  return;
	}

      QApplication::setOverrideCursor(Qt::WaitCursor);

      if(!qmain->getDB().transaction())
	{
	  QApplication::restoreOverrideCursor();
	  qmain->addError
	    (QString(tr("Database Error")),
	     QString(tr("Unable to create a database transaction.")),
	     qmain->getDB().lastError().text(), __FILE__, __LINE__);
	  QMessageBox::critical
	    (this, tr("BiblioteQ: Database Error"),
	     tr("Unable to create a database transaction."));
	  return;
	}

      QApplication::restoreOverrideCursor();
      str = cd.keyword->toPlainText().trimmed();
      cd.keyword->setPlainText(str);

      if(m_engWindowTitle.contains("Modify"))
	query.prepare("UPDATE cd SET "
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
		      "keyword = ? "
		      "WHERE "
		      "myoid = ?");
      else if(qmain->getDB().driverName() != "QSQLITE")
	query.prepare("INSERT INTO cd "
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
		      "back_cover, keyword) "
		      "VALUES "
		      "(?, ?, ?, "
		      "?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, "
		      "?, "
		      "?, ?, ?, ?, ?, ?, ?, ?)");
      else
	query.prepare("INSERT INTO cd "
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
		      "keyword, "
		      "myoid) "
		      "VALUES "
		      "(?, ?, ?, "
		      "?, ?, "
		      "?, ?, "
		      "?, ?, ?, "
		      "?, ?, "
		      "?, ?, ?, ?, ?, ?, ?, ?, ?)");

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

      if(!cd.front_image->m_image.isNull())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);

	  if(buffer.open(QIODevice::WriteOnly))
	    {
	      cd.front_image->m_image.save
		(&buffer, cd.front_image->m_imageFormat.toLatin1(), 100);
	      query.bindValue(17, bytes.toBase64());
	    }
	  else
	    query.bindValue(17, QVariant(QVariant::ByteArray));
	}
      else
	{
	  cd.front_image->m_imageFormat = "";
	  query.bindValue(17, QVariant(QVariant::ByteArray));
	}

      if(!cd.back_image->m_image.isNull())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);

	  if(buffer.open(QIODevice::WriteOnly))
	    {
	      cd.back_image->m_image.save
		(&buffer, cd.back_image->m_imageFormat.toLatin1(), 100);
	      query.bindValue(18, bytes.toBase64());
	    }
	  else
	    query.bindValue(18, QVariant(QVariant::ByteArray));
	}
      else
	{
	  cd.back_image->m_imageFormat = "";
	  query.bindValue(18, QVariant(QVariant::ByteArray));
	}

      query.bindValue(19, cd.keyword->toPlainText().trimmed());

      if(m_engWindowTitle.contains("Modify"))
	query.bindValue(20, m_oid);
      else if(qmain->getDB().driverName() == "QSQLITE")
	{
	  qint64 value = biblioteq_misc_functions::getSqliteUniqueId
	    (qmain->getDB(),
	     errorstr);

	  if(errorstr.isEmpty())
	    query.bindValue(20, value);
	  else
	    qmain->addError(QString(tr("Database Error")),
			    QString(tr("Unable to generate a unique "
				       "integer.")),
			    errorstr);
	}

      QApplication::setOverrideCursor(Qt::WaitCursor);

      if(!query.exec())
	{
	  QApplication::restoreOverrideCursor();
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

	  if(m_engWindowTitle.contains("Modify"))
	    {
	      query.prepare("DELETE FROM cd_copy_info WHERE "
			    "copy_number > ? AND item_oid = ?");
	      query.bindValue(0, cd.quantity->text());
	      query.bindValue(1, m_oid);

	      if(!query.exec())
		{
		  QApplication::restoreOverrideCursor();
		  qmain->addError
		    (QString(tr("Database Error")),
		     QString(tr("Unable to purge unnecessary copy "
				"data.")),
		     query.lastError().text(), __FILE__, __LINE__);
		  goto db_rollback;
		}

	      if(!qmain->getDB().commit())
		{
		  QApplication::restoreOverrideCursor();
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

	      biblioteq_misc_functions::createInitialCopies
		(cd.id->text(),
		 cd.quantity->value(),
		 qmain->getDB(),
		 "CD", errorstr);

	      if(!errorstr.isEmpty())
		{
		  QApplication::restoreOverrideCursor();
		  qmain->addError
		    (QString(tr("Database Error")),
		     QString(tr("Unable to create initial copies.")),
		     errorstr, __FILE__, __LINE__);
		  goto db_rollback;
		}

	      if(!qmain->getDB().commit())
		{
		  QApplication::restoreOverrideCursor();
		  qmain->addError
		    (QString(tr("Database Error")),
		     QString(tr("Unable to commit the current database "
				"transaction.")),
		     qmain->getDB().lastError().text(), __FILE__,
		     __LINE__);
		  goto db_rollback;
		}
	    }

	  m_oldq = cd.quantity->value();

	  if(cd.front_image->m_image.isNull())
	    cd.front_image->m_imageFormat = "";

	  if(cd.back_image->m_image.isNull())
	    cd.back_image->m_imageFormat = "";

	  cd.artist->setMultipleLinks
	    ("cd_search", "artist",
	     cd.artist->toPlainText());
	  cd.recording_label->setMultipleLinks
	    ("cd_search", "recording_label",
	     cd.recording_label->toPlainText());
	  cd.category->setMultipleLinks("cd_search", "category",
					cd.category->toPlainText());
	  cd.keyword->setMultipleLinks("cd_search", "keyword",
				       cd.keyword->toPlainText());
	  QApplication::restoreOverrideCursor();

	  if(m_engWindowTitle.contains("Modify"))
	    {
	      str = QString(tr("BiblioteQ: Modify Music CD Entry (")) +
		cd.id->text() + tr(")");
	      m_engWindowTitle = "Modify";
	      setWindowTitle(str);

	      if((qmain->getTypeFilterString() == "All" ||
		  qmain->getTypeFilterString() == "All Available" ||
		  qmain->getTypeFilterString() == "All Overdue" ||
		  qmain->getTypeFilterString() == "All Requested" ||
		  qmain->getTypeFilterString() == "All Reserved" ||
		  qmain->getTypeFilterString() == "Music CDs") &&
		 m_oid == biblioteq_misc_functions::getColumnString
		 (qmain->getUI().table,
		  m_row, qmain->getUI().table->columnNumber("MYOID")) &&
		 biblioteq_misc_functions::getColumnString
		 (qmain->getUI().table,
		  m_row, qmain->getUI().table->columnNumber("Type")) == "CD")
		{
		  qmain->getUI().table->setSortingEnabled(false);

		  QStringList names(qmain->getUI().table->columnNames());

		  for(i = 0; i < names.size(); i++)
		    {
		      if(names.at(i) == "Catalog Number" ||
			 names.at(i) == "ID Number")
			qmain->getUI().table->item(m_row, i)->setText
			  (cd.id->text());
		      else if(names.at(i) == "Title")
			qmain->getUI().table->item(m_row, i)->setText
			  (cd.title->text());
		      else if(names.at(i) == "Format")
			qmain->getUI().table->item(m_row, i)->setText
			  (cd.format->currentText().trimmed());
		      else if(names.at(i) == "Artist")
			qmain->getUI().table->item(m_row, i)->setText
			  (cd.artist->toPlainText());
		      else if(names.at(i) == "Number of Discs")
			qmain->getUI().table->item(m_row, i)->setText
			  (cd.no_of_discs->text());
		      else if(names.at(i) == "Runtime")
			qmain->getUI().table->item(m_row, i)->setText
			  (cd.runtime->text());
		      else if(names.at(i) == "Release Date" ||
			      names.at(i) == "Publication Date")
			qmain->getUI().table->item(m_row, i)->setText
			  (cd.release_date->date().toString
			   (Qt::ISODate));
		      else if(names.at(i) == "Recording Label" ||
			      names.at(i) == "Publisher")
			qmain->getUI().table->item(m_row, i)->setText
			  (cd.recording_label->toPlainText());
		      else if(names.at(i) == "Categories")
			qmain->getUI().table->item(m_row, i)->setText
			  (cd.category->toPlainText().trimmed());
		      else if(names.at(i) == "Price")
			qmain->getUI().table->item(m_row, i)->setText
			  (cd.price->cleanText());
		      else if(names.at(i) == "Language")
			qmain->getUI().table->item(m_row, i)->setText
			  (cd.language->currentText().trimmed());
		      else if(names.at(i) == "Monetary Units")
			qmain->getUI().table->item(m_row, i)->setText
			  (cd.monetary_units->currentText().trimmed());
		      else if(names.at(i) == "Quantity")
			qmain->getUI().table->item(m_row, i)->setText
			  (cd.quantity->text());
		      else if(names.at(i) == "Location")
			qmain->getUI().table->item(m_row, i)->setText
			  (cd.location->currentText().trimmed());
		      else if(names.at(i) == "Recording Type")
			qmain->getUI().table->item(m_row, i)->setText
			  (cd.recording_type->currentText().trimmed());
		      else if(names.at(i) == "Audio")
			qmain->getUI().table->item(m_row, i)->setText
			  (cd.audio->currentText().trimmed());
		      else if(names.at(i) == "Availability")
			{
			  qmain->getUI().table->item(m_row, i)->setText
			    (biblioteq_misc_functions::getAvailability
			     (m_oid, qmain->getDB(), "CD", errorstr));

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
		}

	      qmain->slotDisplaySummary();
	      qmain->updateSceneItem(m_oid, "CD", cd.front_image->m_image);
	    }
	  else
	    {
	      QApplication::setOverrideCursor(Qt::WaitCursor);
	      m_oid = biblioteq_misc_functions::getOID(cd.id->text(),
						       "CD",
						       qmain->getDB(),
						       errorstr);
	      QApplication::restoreOverrideCursor();

	      if(!errorstr.isEmpty())
		{
		  qmain->addError(QString(tr("Database Error")),
				  QString(tr("Unable to retrieve the CD's "
					     "OID.")),
				  errorstr, __FILE__, __LINE__);
		  QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
					tr("Unable to retrieve the CD's "
					   "OID."));
		}
	      else
		qmain->replaceCD(m_oid, this);

	      updateWindow(biblioteq::EDITABLE);

	      if(qmain->getUI().actionAutoPopulateOnCreation->isChecked())
		(void) qmain->populateTable
		  (biblioteq::POPULATE_ALL, "Music CDs", QString(""));

	      raise();
	    }

	  storeData(this);
	}

      return;

    db_rollback:

      QApplication::setOverrideCursor(Qt::WaitCursor);

      if(!qmain->getDB().rollback())
	qmain->addError
	  (QString(tr("Database Error")), QString(tr("Rollback failure.")),
	   qmain->getDB().lastError().text(), __FILE__, __LINE__);

      QApplication::restoreOverrideCursor();
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to create or update the entry. "
			       "Please verify that "
			       "the entry does not already exist."));
    }
  else
    {
      searchstr = "SELECT DISTINCT cd.title, "
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
	"COUNT(item_borrower_vw.item_oid) AS total_reserved, "
	"cd.type, "
	"cd.myoid, "
	"cd.front_cover "
	"FROM "
	"cd LEFT JOIN item_borrower_vw ON "
	"cd.myoid = item_borrower_vw.item_oid "
	"AND item_borrower_vw.type = 'CD' "
	"WHERE ";
      searchstr.append("id LIKE '%").append(cd.id->text().
					    trimmed()).
	append("%' AND ");

      QString E("");

      if(qmain->getDB().driverName() != "QSQLITE")
	E = "E";

      if(cd.format->currentIndex() != 0)
	searchstr.append("cdformat = " + E + "'" +
			 biblioteq_myqstring::
			 escape(cd.format->currentText().
				trimmed()) +
			 "' AND ");

      searchstr.append("(artist LIKE " + E + "'%").append
	(biblioteq_myqstring::
	 escape(cd.artist->toPlainText().trimmed())).append
	("%' OR ");
      searchstr.append
	("cd.myoid IN (SELECT cd_songs.item_oid FROM cd_songs WHERE "
	 "cd_songs.item_oid = cd.myoid AND ");
      searchstr.append("cd_songs.artist LIKE " + E + "'%").append
	(biblioteq_myqstring::
	 escape(cd.artist->toPlainText().trimmed())).append
	("%')");
      searchstr.append(") AND ");
      searchstr.append
	("cd.myoid IN (SELECT cd_songs.item_oid FROM cd_songs WHERE "
	 "cd_songs.item_oid = cd.myoid AND ");
      searchstr.append("cd_songs.composer LIKE " + E + "'%").append
	(biblioteq_myqstring::
	 escape(cd.composer->toPlainText().trimmed())).append
	("%')");
      searchstr.append(" AND ");

      if(cd.no_of_discs->value() > 0)
	searchstr.append("cddiskcount = ").append
	  (cd.no_of_discs->text()).append(" AND ");

      if(cd.runtime->text() != "00:00:00")
	searchstr.append("cdruntime = '" + cd.runtime->text() +
			 "' AND ");

      if(cd.audio->currentIndex() != 0)
	searchstr.append("cdaudio = '" +
			 cd.audio->currentText().trimmed() +
			 "' AND ");

      if(cd.recording_type->currentIndex() != 0)
	searchstr.append("cdrecording = '" +
			 cd.recording_type->currentText().trimmed() +
			 "' AND ");

      searchstr.append("title LIKE " + E + "'%").append
	(biblioteq_myqstring::
	 escape(cd.title->text().trimmed())).append("%' AND ");

      if(cd.release_date->date().toString("MM/yyyy") != "01/7999")
	searchstr.append("SUBSTR(rdate, 1, 3) || SUBSTR(rdate, 7) = '" +
			 cd.release_date->date().toString
			 ("MM/yyyy") +
			 "' AND ");

      searchstr.append("recording_label LIKE " + E + "'%" +
		       biblioteq_myqstring::
		       escape(cd.recording_label->toPlainText().
			      trimmed()) + "%' AND ");
      searchstr.append("category LIKE " + E + "'%" +
		       biblioteq_myqstring::
		       escape(cd.category->toPlainText().
			      trimmed()) +
		       "%' AND ");

      if(cd.price->value() > -0.01)
	{
	  searchstr.append("price = ");
	  searchstr.append(QString::number(cd.price->value()));
	  searchstr.append(" AND ");
	}

      if(cd.language->currentIndex() != 0)
	searchstr.append("language = " + E + "'" +
			 biblioteq_myqstring::
			 escape(cd.language->currentText().
				trimmed()) +
			 "' AND ");

      if(cd.monetary_units->currentIndex() != 0)
	searchstr.append("monetary_units = " + E + "'" +
			 biblioteq_myqstring::escape
			 (cd.monetary_units->currentText().trimmed()) +
			 "' AND ");

      searchstr.append("description LIKE " + E + "'%" +
		       biblioteq_myqstring::escape
		       (cd.description->toPlainText().trimmed()) + "%' ");
      searchstr.append("AND COALESCE(keyword, '') LIKE " + E + "'%" +
		       biblioteq_myqstring::escape
		       (cd.keyword->toPlainText().trimmed()) +
		       "%' ");

      if(cd.quantity->value() != 0)
	searchstr.append(" AND quantity = " + cd.quantity->text());

      if(cd.location->currentIndex() != 0)
	searchstr.append(" AND location = " + E + "'" +
			 biblioteq_myqstring::escape
			 (cd.location->currentText().trimmed()) + "' ");

      /*
      ** Search the database.
      */

      (void) qmain->populateTable
	(biblioteq::POPULATE_SEARCH, "Music CDs", searchstr);
    }
}

/*
** -- search() --
*/

void biblioteq_cd::search(const QString &field, const QString &value)
{
  m_composer_action->setVisible(true);
  cd.composer->setVisible(true);
  cd.composer_label->setVisible(true);
  cd.id->clear();
  cd.artist->clear();
  cd.title->clear();
  cd.recording_label->clear();
  cd.description->clear();
  cd.keyword->clear();
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
  cd.runtime->setTime(QTime(0, 0, 0));
  cd.runtime->setMinimumTime(QTime(0, 0, 0));
  cd.price->setMinimum(-0.01);
  cd.price->setValue(-0.01);
  cd.quantity->setMinimum(0);
  cd.quantity->setValue(0);
  cd.no_of_discs->setMinimum(0);
  cd.no_of_discs->setValue(0);
  cd.audio->insertItem(0, tr("Any"));
  cd.recording_type->insertItem(0, tr("Any"));
  cd.format->insertItem(0, tr("Any"));
  cd.language->insertItem(0, tr("Any"));
  cd.monetary_units->insertItem(0, tr("Any"));
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

      if(actions.size() >= 2)
	{
	  actions[0]->setVisible(false);
	  actions[1]->setVisible(false);
	}

      actions.clear();
      cd.coverImages->setVisible(false);
      setWindowTitle(tr("BiblioteQ: Database Music CD Search"));
      m_engWindowTitle = "Search";
      cd.id->setFocus();
      biblioteq_misc_functions::center(this, m_parentWid);
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
      else if(field == "keyword")
	cd.keyword->setPlainText(value);

      slotGo();
    }
}

/*
** -- updateWindow() --
*/

void biblioteq_cd::updateWindow(const int state)
{
  QString str = "";

  if(state == biblioteq::EDITABLE)
    {
      cd.showUserButton->setEnabled(true);
      cd.copiesButton->setEnabled(true);
      cd.okButton->setVisible(true);
      cd.queryButton->setVisible(m_isQueryEnabled);
      cd.resetButton->setVisible(true);
      cd.computeButton->setVisible(true);
      trd.saveButton->setVisible(true);
      trd.insertButton->setVisible(true);
      trd.deleteButton->setVisible(true);
      cd.frontButton->setVisible(true);
      cd.backButton->setVisible(true);
      str = QString(tr("BiblioteQ: Modify Music CD Entry (")) +
	cd.id->text() + tr(")");
      m_engWindowTitle = "Modify";
    }
  else
    {
      if(qmain->isGuest())
	cd.showUserButton->setVisible(false);
      else
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
      str = QString(tr("BiblioteQ: View Music CD Details (")) +
	cd.id->text() + tr(")");
      m_engWindowTitle = "View";
    }

  setWindowTitle(str);
  cd.tracksButton->setEnabled(true);
}

/*
** -- modify() --
*/

void biblioteq_cd::modify(const int state)
{
  int i = 0;
  QString str = "";
  QString fieldname = "";
  QVariant var;
  QSqlQuery query(qmain->getDB());

  if(state == biblioteq::EDITABLE)
    {
      setWindowTitle(tr("BiblioteQ: Modify Music CD Entry"));
      m_engWindowTitle = "Modify";
      cd.showUserButton->setEnabled(true);
      cd.copiesButton->setEnabled(true);
      cd.okButton->setVisible(true);
      cd.queryButton->setVisible(m_isQueryEnabled);
      cd.resetButton->setVisible(true);
      cd.computeButton->setVisible(true);
      trd.saveButton->setVisible(true);
      trd.insertButton->setVisible(true);
      trd.deleteButton->setVisible(true);
      cd.frontButton->setVisible(true);
      cd.backButton->setVisible(true);
      biblioteq_misc_functions::highlightWidget
	(cd.id, QColor(255, 248, 220));
      biblioteq_misc_functions::highlightWidget
	(cd.title, QColor(255, 248, 220));
      biblioteq_misc_functions::highlightWidget
	(cd.recording_label->viewport(), QColor(255, 248, 220));
      biblioteq_misc_functions::highlightWidget
	(cd.artist->viewport(), QColor(255, 248, 220));
      biblioteq_misc_functions::highlightWidget
	(cd.description->viewport(), QColor(255, 248, 220));
      biblioteq_misc_functions::highlightWidget
	(cd.category->viewport(), QColor(255, 248, 220));
    }
  else
    {
      setWindowTitle(tr("BiblioteQ: View Music CD Details"));
      m_engWindowTitle = "View";

      if(qmain->isGuest())
	cd.showUserButton->setVisible(false);
      else
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

      if(actions.size() >= 2)
	{
	  actions[0]->setVisible(false);
	  actions[1]->setVisible(false);
	}

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
  str = m_oid;
  query.setForwardOnly(true);
  query.prepare("SELECT id, "
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
		"keyword "
		"FROM "
		"cd "
		"WHERE myoid = ?");
  query.bindValue(0, str);
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!query.exec() || !query.next())
    {
      QApplication::restoreOverrideCursor();
      qmain->addError
	(QString(tr("Database Error")),
	 QString(tr("Unable to retrieve the selected CD's data.")),
	 query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to retrieve the selected CD's "
			       "data."));
      cd.id->setFocus();
      return;
    }
  else
    {
      QApplication::restoreOverrideCursor();
      showNormal();

      for(i = 0; i < query.record().count(); i++)
	{
	  var = query.record().field(i).value();
	  fieldname = query.record().fieldName(i);

	  if(fieldname == "title")
	    cd.title->setText(var.toString());
	  else if(fieldname == "recording_label")
	    cd.recording_label->setMultipleLinks
	      ("cd_search", "recording_label",
	       var.toString());
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
	    cd.runtime->setTime
	      (QTime::fromString(var.toString(), "hh:mm:ss"));
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
	      if(state == biblioteq::EDITABLE)
		{
		  str = QString(tr("BiblioteQ: Modify Music CD Entry (")) +
		    var.toString() + tr(")");
		  m_engWindowTitle = "Modify";
		}
	      else
		{
		  str = QString(tr("BiblioteQ: View Music CD Details (")) +
		    var.toString() + tr(")");
		  m_engWindowTitle = "View";
		}

	      setWindowTitle(str);
	      cd.id->setText(var.toString());
	    }
	  else if(fieldname == "description")
	    cd.description->setPlainText(var.toString());
	  else if(fieldname == "keyword")
	    cd.keyword->setMultipleLinks("cd_search", "keyword",
					 var.toString());
	  else if(fieldname == "cdformat")
	    {
	      if(cd.format->findText(var.toString()) > -1)
		cd.format->setCurrentIndex
		  (cd.format->findText(var.toString()));
	      else
		cd.format->setCurrentIndex
		  (cd.format->findText(tr("UNKNOWN")));
	    }
	  else if(fieldname == "artist")
	    cd.artist->setMultipleLinks
	      ("cd_search", "artist", var.toString());
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
		  cd.front_image->loadFromData
		    (QByteArray::fromBase64(var.toByteArray()));

		  if(cd.front_image->m_image.isNull())
		    cd.front_image->loadFromData(var.toByteArray());
		}
	    }
	  else if(fieldname == "back_cover")
	    {
	      if(!query.record().field(i).isNull())
		{
		  cd.back_image->loadFromData
		    (QByteArray::fromBase64(var.toByteArray()));

		  if(cd.back_image->m_image.isNull())
		    cd.back_image->loadFromData(var.toByteArray());
		}
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

void biblioteq_cd::insert(void)
{
  slotReset();
  cd.id->clear();
  cd.artist->setPlainText("N/A");
  cd.title->clear();
  cd.recording_label->setPlainText("N/A");
  cd.description->setPlainText("N/A");
  cd.keyword->clear();
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
  biblioteq_misc_functions::highlightWidget
    (cd.id, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (cd.title, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (cd.recording_label->viewport(), QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (cd.artist->viewport(), QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (cd.description->viewport(), QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (cd.category->viewport(), QColor(255, 248, 220));
  setWindowTitle(tr("BiblioteQ: Create Music CD Entry"));
  m_engWindowTitle = "Create";
  cd.id->setFocus();
  storeData(this);
  show();
}

/*
** -- slotPopulateTracksBrowser() --
*/

void biblioteq_cd::slotPopulateTracksBrowser(void)
{
  int i = -1;
  int j = 0;
  QString str = "";
  QSpinBox *trackEdit = 0;
  QComboBox *comboBox = 0;
  QSqlQuery query(qmain->getDB());
  QTimeEdit * timeEdit = 0;
  QStringList list;
  QStringList comboBoxList;
  QProgressDialog progress(m_tracks_diag);
  QTableWidgetItem *item = 0;

#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  progress.setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif
  query.setForwardOnly(true);
  query.prepare("SELECT albumnum, songnum, songtitle, runtime, "
		"artist, composer "
		"FROM cd_songs WHERE item_oid = ? "
		"ORDER BY albumnum, songnum, songtitle, runtime, "
		"artist, composer");
  query.bindValue(0, m_oid);
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
    {
      QApplication::restoreOverrideCursor();
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

  QApplication::restoreOverrideCursor();

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
  list.append(tr("Artist"));
  list.append(tr("Composer"));
  trd.table->setColumnCount(list.size());
  trd.table->setHorizontalHeaderLabels(list);
  list.clear();
  trd.table->setRowCount(0);
  trd.table->scrollToTop();
  trd.table->horizontalScrollBar()->setValue(0);
  m_tracks_diag->updateGeometry();
  m_tracks_diag->setWindowTitle
    (QString(tr("BiblioteQ: Album Tracks Browser (")) +
     cd.id->text() + tr(")"));
  m_tracks_diag->show();
  trd.table->setSortingEnabled(false);

  if(qmain->getDB().driverName() != "QSQLITE")
    trd.table->setRowCount(query.size());

  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.setLabelText(tr("Populating the table..."));
  progress.setMinimum(0);

  if(qmain->getDB().driverName() == "QSQLITE")
    {
      if(query.lastError().isValid())
	progress.setMaximum(0);
      else
	progress.setMaximum
	  (biblioteq_misc_functions::sqliteQuerySize(query.lastQuery(),
						     query.boundValues(),
						     qmain->getDB(),
						     __FILE__, __LINE__));
    }
  else
    progress.setMaximum(query.size());

  progress.show();
#ifndef Q_OS_MAC
  progress.repaint();
  QApplication::processEvents();
#endif
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

      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

#ifndef Q_OS_MAC
      progress.repaint();
      QApplication::processEvents();
#endif
    }

  progress.close();
  trd.table->setSortingEnabled(false);
  comboBoxList.clear();
  trd.table->setRowCount(i); // Support cancellation.
  trd.table->resizeColumnsToContents();
}

/*
** -- slotCloseTracksBrowser() --
*/

void biblioteq_cd::slotCloseTracksBrowser(void)
{
  trd.table->clear();
  trd.table->setCurrentItem(0);
  trd.table->setRowCount(0);
  m_tracks_diag->close();
}

/*
** -- slotInsertTrack() --
*/

void biblioteq_cd::slotInsertTrack(void)
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

  for(i = 0; i < trd.table->columnCount(); i++)
    {
      if(i == 1)
	str = "1";
      else if(i == 2)
	str = tr("Title");
      else if(i == 4 || i == 5)
	str = tr("UNKNOWN");
      else
	str.clear();

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
	      trackEdit->setValue(trd.table->rowCount());
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

void biblioteq_cd::slotDeleteTrack(void)
{
  trd.table->removeRow(trd.table->currentRow());
}

/*
** -- slotSaveTracks() --
*/

void biblioteq_cd::slotSaveTracks(void)
{
  int i = 0;
  QString errormsg = "";
  QString lastError = "";
  QSqlQuery query(qmain->getDB());
  QProgressDialog progress(this);

#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  progress.setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif

  for(i = 0; i < trd.table->rowCount(); i++)
    if(trd.table->item(i, 2) != 0 &&
       trd.table->item(i, 2)->text().trimmed().isEmpty())
      {
	errormsg = QString(tr("Row number ")) + QString::number(i + 1) +
	  tr(" contains an empty Song Title.");
	QMessageBox::critical(m_tracks_diag, tr("BiblioteQ: User Error"),
			      errormsg);
	return;
      }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!qmain->getDB().transaction())
    {
      QApplication::restoreOverrideCursor();
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to create a database transaction.")),
		      qmain->getDB().lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(m_tracks_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to create a database transaction."));
      return;
    }

  QApplication::restoreOverrideCursor();
  query.prepare("DELETE FROM cd_songs WHERE item_oid = ?");
  query.bindValue(0, m_oid);
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
    {
      QApplication::restoreOverrideCursor();
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to purge track data.")),
		      query.lastError().text(), __FILE__, __LINE__);
      QApplication::setOverrideCursor(Qt::WaitCursor);

      if(!qmain->getDB().rollback())
	qmain->addError(QString(tr("Database Error")),
			QString(tr("Rollback failure.")),
			qmain->getDB().lastError().text(),
			__FILE__, __LINE__);

      QApplication::restoreOverrideCursor();
    }
  else
    {
      QApplication::restoreOverrideCursor();
      progress.setCancelButton(0);
      progress.setModal(true);
      progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
      progress.setLabelText(tr("Saving the track data..."));
      progress.setMaximum(trd.table->rowCount());
      progress.setMinimum(0);
      progress.show();
      progress.update();

      for(i = 0; i < trd.table->rowCount(); i++)
	{
	  query.prepare("INSERT INTO cd_songs ("
			"item_oid, "
			"albumnum, "
			"songnum, "
			"songtitle, "
			"runtime, "
			"artist, "
			"composer "
			") "
			"VALUES (?, "
			"?, ?, ?, ?, ?, ?)");
	  query.bindValue(0, m_oid);

	  if(trd.table->cellWidget(i, 0) != 0)
	    query.bindValue(1, qobject_cast<QComboBox *>
			    (trd.table->cellWidget(i, 0))->currentText());

	  if(trd.table->cellWidget(i, 1) != 0)
	    query.bindValue(2, qobject_cast<QSpinBox *>
			    (trd.table->cellWidget(i, 1))->value());

	  if(trd.table->item(i, 2) != 0)
	    query.bindValue(3, trd.table->item(i, 2)->text().trimmed());

	  if(trd.table->cellWidget(i, 3) != 0)
	    query.bindValue
	      (4, qobject_cast<QTimeEdit *> (trd.table->
					     cellWidget(i, 3))->time().
	       toString("hh:mm:ss"));

	  if(trd.table->item(i, 4) != 0)
	    query.bindValue(5, trd.table->item(i, 4)->text().trimmed());

	  if(trd.table->item(i, 2) != 0)
	    query.bindValue(6, trd.table->item(i, 5)->text().trimmed());

	  if(!query.exec())
	    {
	      qmain->addError(QString(tr("Database Error")),
			      QString(tr("Unable to create track data.")),
			      query.lastError().text(), __FILE__, __LINE__);
	      lastError = query.lastError().text();
	    }

	  if(i + 1 <= progress.maximum())
	    progress.setValue(i + 1);

#ifndef Q_OS_MAC
	  progress.repaint();
	  QApplication::processEvents();
#endif
	}

      progress.close();
      QApplication::setOverrideCursor(Qt::WaitCursor);

      if(!qmain->getDB().commit())
	{
	  qmain->addError(QString(tr("Database Error")),
			  QString(tr("Commit failure.")),
			  qmain->getDB().lastError().text(), __FILE__,
			  __LINE__);
	  qmain->getDB().rollback();
	}

      QApplication::restoreOverrideCursor();

      if(!lastError.isEmpty() ||
	 qmain->getDB().lastError().isValid())
	QMessageBox::critical(m_tracks_diag, tr("BiblioteQ: Database Error"),
			      tr("Some or all of the track data has not "
				 "been saved."));

      /*
      ** Update the runtime.
      */

      if(lastError.isEmpty() &&
	 !qmain->getDB().lastError().isValid())
	slotComputeRuntime();
    }
}

/*
** -- slotReset() --
*/

void biblioteq_cd::slotReset(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(action != 0)
    {
      QList<QAction *> actions = cd.resetButton->menu()->actions();

      if(actions.size() < 21)
	{
	  // Error.
	}
      else if(action == actions[0])
	cd.front_image->clear();
      else if(action == actions[1])
	cd.back_image->clear();
      else if(action == actions[2])
	{
	  cd.id->clear();
	  cd.id->setFocus();
	}
      else if(action == actions[3])
	{
	  cd.format->setCurrentIndex(0);
	  cd.format->setFocus();
	}
      else if(action == actions[4])
	{
	  if(m_engWindowTitle.contains("Search"))
	    cd.artist->clear();
	  else
	    cd.artist->setPlainText("N/A");

	  cd.artist->setFocus();
	}
      else if(action == actions[5])
	{
	  cd.composer->clear();
	  cd.composer->setFocus();
	}
      else if(action == actions[6])
	{
	  cd.no_of_discs->setValue(cd.no_of_discs->minimum());
	  cd.no_of_discs->setFocus();
	}
      else if(action == actions[7])
	{
	  if(m_engWindowTitle.contains("Search"))
	    cd.runtime->setTime(QTime(0, 0, 0));
	  else
	    cd.runtime->setTime(QTime(0, 0, 1));

	  cd.runtime->setFocus();
	}
      else if(action == actions[8])
	{
	  cd.audio->setCurrentIndex(0);
	  cd.audio->setFocus();
	}
      else if(action == actions[9])
	{
	  cd.recording_type->setCurrentIndex(0);
	  cd.recording_type->setFocus();
	}
      else if(action == actions[10])
	{
	  cd.title->clear();
	  cd.title->setFocus();
	}
      else if(action == actions[11])
	{
	  if(m_engWindowTitle.contains("Search"))
	    cd.release_date->setDate
	      (QDate::fromString("01/7999", "MM/yyyy"));
	  else
	    cd.release_date->setDate
	      (QDate::fromString("01/01/2000", "MM/dd/yyyy"));

	  cd.release_date->setFocus();
	}
      else if(action == actions[12])
	{
	  if(m_engWindowTitle.contains("Search"))
	    cd.recording_label->clear();
	  else
	    cd.recording_label->setPlainText("N/A");

	  cd.recording_label->setFocus();
	}
      else if(action == actions[13])
	{
	  if(m_engWindowTitle.contains("Search"))
	    cd.category->clear();
	  else
	    cd.category->setPlainText("N/A");

	  cd.category->setFocus();
	}
      else if(action == actions[14])
	{
	  cd.price->setValue(cd.price->minimum());
	  cd.price->setFocus();
	}
      else if(action == actions[15])
	{
	  cd.language->setCurrentIndex(0);
	  cd.language->setFocus();
	}
      else if(action == actions[16])
	{
	  cd.monetary_units->setCurrentIndex(0);
	  cd.monetary_units->setFocus();
	}
      else if(action == actions[17])
	{
	  cd.quantity->setValue(cd.quantity->minimum());
	  cd.quantity->setFocus();
	}
      else if(action == actions[18])
	{
	  cd.location->setCurrentIndex(0);
	  cd.location->setFocus();
	}
      else if(action == actions[19])
	{
	  if(m_engWindowTitle.contains("Search"))
	    cd.description->clear();
	  else
	    cd.description->setPlainText("N/A");

	  cd.description->setFocus();
	}
      else if(action == actions[20])
	{
	  cd.keyword->clear();
	  cd.keyword->setFocus();
	}

      actions.clear();
    }
  else
    {
      /*
      ** Reset all.
      */

      cd.title->clear();

      if(m_engWindowTitle.contains("Search"))
	cd.artist->clear();
      else
	cd.artist->setPlainText("N/A");

      if(m_engWindowTitle.contains("Search"))
	cd.recording_label->clear();
      else
	cd.recording_label->setPlainText("N/A");

      if(m_engWindowTitle.contains("Search"))
	cd.category->clear();
      else
	cd.category->setPlainText("N/A");

      if(m_engWindowTitle.contains("Search"))
	cd.description->clear();
      else
	cd.description->setPlainText("N/A");

      if(m_engWindowTitle.contains("Search"))
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
      cd.keyword->clear();
      cd.composer->clear();
      cd.id->setFocus();
    }
}

/*
** -- closeEvent() --
*/

void biblioteq_cd::closeEvent(QCloseEvent *e)
{
  if(m_engWindowTitle.contains("Create") ||
     m_engWindowTitle.contains("Modify"))
    if(hasDataChanged(this))
      if(QMessageBox::
	 question(this, tr("BiblioteQ: Question"),
		  tr("Your changes have not been saved. Continue closing?"),
		  QMessageBox::Yes | QMessageBox::No,
		  QMessageBox::No) == QMessageBox::No)
	{
	  if(e)
	    e->ignore();

	  return;
	}

  qmain->removeCD(this);
}

/*
** -- slotCancel() --
*/

void biblioteq_cd::slotCancel(void)
{
  close();
}

/*
** -- slotPopulateCopiesEditor() --
*/

void biblioteq_cd::slotPopulateCopiesEditor(void)
{
  biblioteq_copy_editor *copyeditor = 0;

  if((copyeditor = new(std::nothrow) biblioteq_copy_editor
      (qobject_cast<QWidget *> (this),
       static_cast<biblioteq_item *> (this),
       false,
       cd.quantity->value(), m_oid,
       cd.quantity, font(), "CD", cd.id->text().trimmed())) != 0)
    copyeditor->populateCopiesEditor();
}

/*
** -- slotShowUsers() --
*/

void biblioteq_cd::slotShowUsers(void)
{
  int state = 0;
  biblioteq_borrowers_editor *borrowerseditor = 0;

  if(!cd.okButton->isHidden())
    state = biblioteq::EDITABLE;
  else
    state = biblioteq::VIEW_ONLY;

  if((borrowerseditor = new(std::nothrow) biblioteq_borrowers_editor
      (qobject_cast<QWidget *> (this), static_cast<biblioteq_item *> (this),
       cd.quantity->value(), m_oid, cd.id->text(), font(), "CD",
       state)) != 0)
    borrowerseditor->showUsers();
}

/*
** -- slotQuery() --
*/

void biblioteq_cd::slotQuery(void)
{
}

/*
** -- slotComputeRuntime() --
*/

void biblioteq_cd::slotComputeRuntime(void)
{
  int secs = 0;
  QTime sum(0, 0, 0);
  QTime time(0, 0, 0);
  QSqlQuery query(qmain->getDB());

  query.setForwardOnly(true);
  query.prepare("SELECT runtime FROM cd_songs WHERE item_oid = ?");
  query.bindValue(0, m_oid);
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(query.exec())
    while(query.next())
      {
	time = QTime::fromString(query.value(0).toString(), "hh:mm:ss");
	secs = time.hour() * 3600 + time.minute() * 60 + time.second();
	sum = sum.addSecs(secs);
      }

  QApplication::restoreOverrideCursor();

  if(sum.toString("hh:mm:ss") == "00:00:00")
    QMessageBox::critical(this, tr("BiblioteQ: User Error"),
			  tr("The total runtime of the available tracks is "
			     "zero. Please set the individual runtimes."));
  else
    cd.runtime->setTime(QTime::fromString(sum.toString("hh:mm:ss"),
					  "hh:mm:ss"));
}

/*
** -- slotPrint() --
*/

void biblioteq_cd::slotPrint(void)
{
  m_html = "<html>";
  m_html += "<b>" + tr("Catalog Number:") + "</b> " +
    cd.id->text().trimmed() + "<br>";
  m_html += "<b>" + tr("Format:") + "</b> " + cd.format->currentText() + "<br>";
  m_html += "<b>" + tr("Artist:") + "</b> " +
    cd.artist->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Number of Discs:") + "</b> " +
    cd.no_of_discs->text() + "<br>";
  m_html += "<b>" + tr("Runtime:") + "</b> " + cd.runtime->text() + "<br>";
  m_html += "<b>" + tr("Audio:") + "</b> " + cd.audio->currentText() + "<br>";
  m_html += "<b>" + tr("Recording Type:") + "</b> " +
    cd.recording_type->currentText() + "<br>";

  /*
  ** General information.
  */

  m_html += "<b>" + tr("Title:") + "</b> " + cd.title->text().trimmed() +
    "<br>";
  m_html += "<b>" + tr("Release Date:") + "</b> " + cd.release_date->date().
    toString(Qt::ISODate) + "<br>";
  m_html += "<b>" + tr("Recording Label:") + "</b> " + cd.recording_label->
    toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Categories:") + "</b> " +
    cd.category->toPlainText().trimmed() + "<br>";
  m_html += "<b>"+ tr("Price:") + "</b> " + cd.price->cleanText() + "<br>";
  m_html += "<b>" + tr("Language:") + "</b> " +
    cd.language->currentText() + "<br>";
  m_html += "<b>" + tr("Monetary Units:") + "</b> " +
    cd.monetary_units->currentText() + "<br>";
  m_html += "<b>" + tr("Copies:") + "</b> " + cd.quantity->text() + "<br>";
  m_html += "<b>" + tr("Location:") + "</b> " +
    cd.location->currentText() + "<br>";
  m_html += "<b>" + tr("Abstract:") + "</b> " +
    cd.description->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Keywords:") + "</b> " +
    cd.keyword->toPlainText().trimmed();
  m_html += "</html>";
  print(this);
}

/*
** -- slotSelectImage() --
*/

void biblioteq_cd::slotSelectImage(void)
{
  QFileDialog dialog(this);
  QPushButton *button = qobject_cast<QPushButton *> (sender());

#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  dialog.setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setDirectory(QDir::homePath());

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
	  cd.front_image->m_image = QImage(dialog.selectedFiles().value(0));

	  if(dialog.selectedFiles().value(0).lastIndexOf(".") > -1)
	    cd.front_image->m_imageFormat = dialog.selectedFiles().value(0).mid
	      (dialog.selectedFiles().value(0).lastIndexOf(".") + 1).
	      toUpper();

	  cd.front_image->scene()->addPixmap
	    (QPixmap::fromImage(cd.front_image->m_image));

	  if(cd.front_image->scene()->items().size() > 0)
	    cd.front_image->scene()->items().at(0)->setFlags
	      (QGraphicsItem::ItemIsSelectable);

	  cd.front_image->scene()->setSceneRect
	    (cd.front_image->scene()->itemsBoundingRect());
	}
      else
	{
	  cd.back_image->clear();
	  cd.back_image->m_image = QImage(dialog.selectedFiles().value(0));

	  if(dialog.selectedFiles().value(0).lastIndexOf(".") > -1)
	    cd.back_image->m_imageFormat = dialog.selectedFiles().value(0).mid
	      (dialog.selectedFiles().value(0).lastIndexOf(".") + 1).
	      toUpper();

	  cd.back_image->scene()->addPixmap
	    (QPixmap::fromImage(cd.back_image->m_image));

	  if(cd.back_image->scene()->items().size() > 0)
	    cd.back_image->scene()->items().at(0)->setFlags
	      (QGraphicsItem::ItemIsSelectable);

	  cd.back_image->scene()->setSceneRect
	    (cd.back_image->scene()->itemsBoundingRect());
	}
    }
}

/*
** -- duplicate() --
*/

void biblioteq_cd::duplicate(const QString &p_oid, const int state)
{
  modify(state); // Initial population.
  cd.copiesButton->setEnabled(false);
  cd.tracksButton->setEnabled(false);
  cd.showUserButton->setEnabled(false);
  m_oid = p_oid;
  setWindowTitle(tr("BiblioteQ: Duplicate Music CD Entry"));
  m_engWindowTitle = "Create";
}

/*
** -- changeEvent() --
*/

void biblioteq_cd::changeEvent(QEvent *event)
{
  if(event)
    switch(event->type())
      {
      case QEvent::LanguageChange:
	{
	  cd.retranslateUi(this);
	  trd.retranslateUi(m_tracks_diag);
	  break;
	}
      default:
	break;
      }

  QMainWindow::changeEvent(event);
}
