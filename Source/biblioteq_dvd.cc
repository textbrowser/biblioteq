/*
** -- Qt Includes --
*/

#include <QShortcut>
#include <QSqlField>
#include <QSqlRecord>

/*
** Includes dvd-specific methods.
*/

/*
** -- Local Includes --
*/

#include "biblioteq.h"
#include "biblioteq_borrowers_editor.h"
#include "biblioteq_dvd.h"

extern biblioteq *qmain;

biblioteq_dvd::biblioteq_dvd(QMainWindow *parentArg,
			     const QString &oidArg,
			     const int rowArg):
  QMainWindow(), biblioteq_item(rowArg)
{
  QMenu *menu = 0;
  QRegExp rx1("[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]"
	      "[0-9]");
  QValidator *validator1 = 0;
  QGraphicsScene *scene1 = 0;
  QGraphicsScene *scene2 = 0;

  if((menu = new(std::nothrow) QMenu(this)) == 0)
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
  dvd.setupUi(this);
  dvd.publication_date_enabled->setVisible(false);
  dvd.release_date->setDisplayFormat(qmain->publicationDateFormat("dvds"));
  new (std::nothrow) QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S),
			       this,
			       SLOT(slotGo(void)));
  updateFont(QApplication::font(), qobject_cast<QWidget *> (this));
  connect(dvd.okButton, SIGNAL(clicked(void)), this, SLOT(slotGo(void)));
  connect(dvd.printButton, SIGNAL(clicked(void)), this,
	  SLOT(slotPrint(void)));
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
  connect(menu->addAction(tr("Reset Front Cover Image")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Back Cover Image")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset UPC")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Rating")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Actors")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Directors")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Number of Discs")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Runtime")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Format")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Region")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Aspect Ratio")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Title")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Release Date")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Studio")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Categories")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Price")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Language")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Monetary Units")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Copies")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Location")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Abstract")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Keywords")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Accession Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(dvd.frontButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(dvd.backButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  dvd.queryButton->setVisible(m_isQueryEnabled);
  dvd.resetButton->setMenu(menu);
  dvd.id->setValidator(validator1);

  QString errorstr("");

  QApplication::setOverrideCursor(Qt::WaitCursor);
  dvd.language->addItems
    (biblioteq_misc_functions::getLanguages(qmain->getDB(),
					    errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the languages.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  dvd.monetary_units->addItems
    (biblioteq_misc_functions::getMonetaryUnits(qmain->getDB(),
						errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the monetary units.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  dvd.location->addItems
    (biblioteq_misc_functions::getLocations(qmain->getDB(),
					    "DVD",
					    errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the dvd locations.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  dvd.rating->addItems
    (biblioteq_misc_functions::getDVDRatings(qmain->getDB(),
					     errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the dvd ratings.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  dvd.aspectratio->addItems
    (biblioteq_misc_functions::getDVDAspectRatios(qmain->getDB(),
						  errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the dvd aspect ratios.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  dvd.region->addItems
    (biblioteq_misc_functions::getDVDRegions(qmain->getDB(),
					     errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the dvd regions.")),
       errorstr, __FILE__, __LINE__);

  dvd.front_image->setScene(scene1);
  dvd.back_image->setScene(scene2);

  if(dvd.language->findText(tr("UNKNOWN")) == -1)
    dvd.language->addItem(tr("UNKNOWN"));

  if(dvd.monetary_units->findText(tr("UNKNOWN")) == -1)
    dvd.monetary_units->addItem(tr("UNKNOWN"));

  if(dvd.location->findText(tr("UNKNOWN")) == -1)
    dvd.location->addItem(tr("UNKNOWN"));

  if(dvd.rating->findText(tr("UNKNOWN")) == -1)
    dvd.rating->addItem(tr("UNKNOWN"));

  if(dvd.aspectratio->findText(tr("UNKNOWN")) == -1)
    dvd.aspectratio->addItem(tr("UNKNOWN"));

  if(dvd.region->findText(tr("UNKNOWN")) == -1)
    dvd.region->addItem(tr("UNKNOWN"));

  if(m_parentWid)
    resize(qRound(0.95 * m_parentWid->size().width()),
	   qRound(0.95 * m_parentWid->size().height()));

#ifdef Q_OS_MAC
  foreach(QToolButton *tool_button, findChildren<QToolButton *> ())
#if (QT_VERSION < QT_VERSION_CHECK(5, 10, 0))
    tool_button->setStyleSheet
    ("QToolButton {border: none; padding-right: 10px}"
     "QToolButton::menu-button {border: none;}");
#else
    tool_button->setStyleSheet
      ("QToolButton {border: none; padding-right: 15px}"
       "QToolButton::menu-button {border: none; width: 15px;}");
#endif
#endif

  dvd.splitter->setStretchFactor(0, 0);
  dvd.splitter->setStretchFactor(1, 1);
  dvd.splitter->setStretchFactor(2, 0);
  biblioteq_misc_functions::center(this, m_parentWid);
  biblioteq_misc_functions::hideAdminFields(this, qmain->getRoles());
}

biblioteq_dvd::~biblioteq_dvd()
{
}

void biblioteq_dvd::slotGo(void)
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
	  newq = dvd.quantity->value();
	  QApplication::setOverrideCursor(Qt::WaitCursor);
	  maxcopynumber = biblioteq_misc_functions::getMaxCopyNumber
	    (qmain->getDB(), m_oid, "DVD", errorstr);

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
	      dvd.quantity->setValue(m_oldq);
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

      str = dvd.id->text().trimmed();
      dvd.id->setText(str);

      if(dvd.id->text().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the UPC field."));
	  dvd.id->setFocus();
	  return;
	}

      str = dvd.actors->toPlainText().trimmed();
      dvd.actors->setPlainText(str);

      if(dvd.actors->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Actors field."));
	  dvd.actors->setFocus();
	  return;
	}

      str = dvd.directors->toPlainText().trimmed();
      dvd.directors->setPlainText(str);

      if(dvd.directors->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Directors field."));
	  dvd.directors->setFocus();
	  return;
	}

      if(dvd.runtime->text() == "00:00:00")
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please provide a valid Runtime."));
	  dvd.runtime->setFocus();
	  return;
	}

      str = dvd.format->text().trimmed();
      dvd.format->setText(str);

      if(dvd.format->text().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Format field."));
	  dvd.format->setFocus();
	  return;
	}

      str = dvd.title->text().trimmed();
      dvd.title->setText(str);

      if(dvd.title->text().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Title "
				   "field."));
	  dvd.title->setFocus();
	  return;
	}

      str = dvd.studio->toPlainText().trimmed();
      dvd.studio->setPlainText(str);

      if(dvd.studio->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Studio field."));
	  dvd.studio->setFocus();
	  return;
	}

      str = dvd.category->toPlainText().trimmed();
      dvd.category->setPlainText(str);

      if(dvd.category->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Categories field."));
	  dvd.category->setFocus();
	  return;
	}

      str = dvd.description->toPlainText().trimmed();
      dvd.description->setPlainText(str);

      if(dvd.description->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Abstract field."));
	  dvd.description->setFocus();
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

      str = dvd.keyword->toPlainText().trimmed();
      dvd.keyword->setPlainText(str);
      str = dvd.accession_number->text().trimmed();
      dvd.accession_number->setText(str);
      QApplication::restoreOverrideCursor();

      if(m_engWindowTitle.contains("Modify"))
	query.prepare("UPDATE dvd SET "
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
		      "keyword = ?, "
		      "accession_number = ? "
		      "WHERE "
		      "myoid = ?");
      else if(qmain->getDB().driverName() != "QSQLITE")
	query.prepare("INSERT INTO dvd "
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
		      "back_cover, keyword, accession_number) "
		      "VALUES "
		      "(?, ?, ?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, "
		      "?, ?, "
		      "?, ?, ?, ?, ?, ?, ?, ?)");
      else
	query.prepare("INSERT INTO dvd "
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
		      "description, "
		      "front_cover, "
		      "back_cover, "
		      "keyword, "
		      "accession_number, "
		      "myoid) "
		      "VALUES "
		      "(?, ?, ?, ?, "
		      "?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, ?, ?, ?, ?, ?, ?)");

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
      query.bindValue(11, dvd.studio->toPlainText());
      query.bindValue(12, dvd.category->toPlainText());
      query.bindValue(13, dvd.price->value());
      query.bindValue(14, dvd.language->currentText().trimmed());
      query.bindValue(15, dvd.monetary_units->currentText().trimmed());
      query.bindValue(16, dvd.quantity->text());
      query.bindValue(17, dvd.location->currentText().trimmed());
      query.bindValue(18, dvd.description->toPlainText());

      if(!dvd.front_image->m_image.isNull())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);

	  if(buffer.open(QIODevice::WriteOnly))
	    {
	      dvd.front_image->m_image.save
		(&buffer, dvd.front_image->m_imageFormat.toLatin1(), 100);
	      query.bindValue(19, bytes.toBase64());
	    }
	  else
	    query.bindValue(19, QVariant(QVariant::ByteArray));
	}
      else
	{
	  dvd.front_image->m_imageFormat = "";
	  query.bindValue(19, QVariant(QVariant::ByteArray));
	}

      if(!dvd.back_image->m_image.isNull())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);

	  if(buffer.open(QIODevice::WriteOnly))
	    {
	      dvd.back_image->m_image.save
		(&buffer, dvd.back_image->m_imageFormat.toLatin1(), 100);
	      query.bindValue(20, bytes.toBase64());
	    }
	  else
	    query.bindValue(20, QVariant(QVariant::ByteArray));
	}
      else
	{
	  dvd.back_image->m_imageFormat = "";
	  query.bindValue(20, QVariant(QVariant::ByteArray));
	}

      query.bindValue(21, dvd.keyword->toPlainText().trimmed());
      query.bindValue(22, dvd.accession_number->text().trimmed());

      if(m_engWindowTitle.contains("Modify"))
	query.bindValue(23, m_oid);
      else if(qmain->getDB().driverName() == "QSQLITE")
	{
	  qint64 value = biblioteq_misc_functions::getSqliteUniqueId
	    (qmain->getDB(),
	     errorstr);

	  if(errorstr.isEmpty())
	    query.bindValue(23, value);
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
	      query.prepare("DELETE FROM dvd_copy_info WHERE "
			    "copy_number > ? AND "
			    "item_oid = "
			    "?");
	      query.bindValue(0, dvd.quantity->text());
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
		(dvd.id->text(),
		 dvd.quantity->value(),
		 qmain->getDB(), "DVD",
		 errorstr);

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

	  m_oldq = dvd.quantity->value();

	  if(dvd.front_image->m_image.isNull())
	    dvd.front_image->m_imageFormat = "";

	  if(dvd.back_image->m_image.isNull())
	    dvd.back_image->m_imageFormat = "";

	  dvd.actors->setMultipleLinks("dvd_search", "actors",
				       dvd.actors->toPlainText());
	  dvd.directors->setMultipleLinks("dvd_search", "directors",
					  dvd.directors->toPlainText());
	  dvd.studio->setMultipleLinks("dvd_search", "studio",
				       dvd.studio->toPlainText());
	  dvd.category->setMultipleLinks("dvd_search", "category",
					 dvd.category->toPlainText());
	  dvd.keyword->setMultipleLinks("dvd_search", "keyword",
					dvd.keyword->toPlainText());
	  QApplication::restoreOverrideCursor();

	  if(m_engWindowTitle.contains("Modify"))
	    {
	      str = QString(tr("BiblioteQ: Modify DVD Entry (")) +
		dvd.id->text() + tr(")");
	      setWindowTitle(str);
	      m_engWindowTitle = "Modify";

	      if((qmain->getTypeFilterString() == "All" ||
		  qmain->getTypeFilterString() == "All Available" ||
		  qmain->getTypeFilterString() == "All Overdue" ||
		  qmain->getTypeFilterString() == "All Requested" ||
		  qmain->getTypeFilterString() == "All Reserved" ||
		  qmain->getTypeFilterString() == "DVDs") &&
		 m_oid == biblioteq_misc_functions::getColumnString
		 (qmain->getUI().table,
		  m_row, qmain->getUI().table->columnNumber("MYOID")) &&
		 biblioteq_misc_functions::getColumnString
		 (qmain->getUI().table,
		  m_row, qmain->getUI().table->columnNumber("Type")) == "DVD")
		{
		  qmain->getUI().table->setSortingEnabled(false);

		  QStringList names(qmain->getUI().table->columnNames());

		  for(i = 0; i < names.size(); i++)
		    {
		      if(names.at(i) == "UPC" ||
			 names.at(i) == "ID Number")
			qmain->getUI().table->item(m_row, i)->setText
			  (dvd.id->text());
		      else if(names.at(i) == "Rating")
			qmain->getUI().table->item(m_row, i)->setText
			  (dvd.rating->currentText().trimmed());
		      else if(names.at(i) == "Number of Discs")
			qmain->getUI().table->item(m_row, i)->setText
			  (dvd.no_of_discs->text());
		      else if(names.at(i) == "Runtime")
			qmain->getUI().table->item(m_row, i)->setText
			  (dvd.runtime->text());
		      else if(names.at(i) == "Format")
			qmain->getUI().table->item(m_row, i)->setText
			  (dvd.format->text());
		      else if(names.at(i) == "Region")
			qmain->getUI().table->item(m_row, i)->setText
			  (dvd.region->currentText().trimmed());
		      else if(names.at(i) == "Aspect Ratio")
			qmain->getUI().table->item(m_row, i)->setText
			  (dvd.aspectratio->currentText().trimmed());
		      else if(names.at(i) == "Title")
			qmain->getUI().table->item(m_row, i)->setText
			  (dvd.title->text());
		      else if(names.at(i) == "Release Date" ||
			      names.at(i) == "Publication Date")
			qmain->getUI().table->item(m_row, i)->setText
			  (dvd.release_date->date().toString
			   (Qt::ISODate));
		      else if(names.at(i) == "Studio" ||
			      names.at(i) == "Publisher")
			qmain->getUI().table->item(m_row, i)->setText
			  (dvd.studio->toPlainText());
		      else if(names.at(i) == "Categories")
			qmain->getUI().table->item(m_row, i)->setText
			  (dvd.category->toPlainText());
		      else if(names.at(i) == "Price")
			qmain->getUI().table->item(m_row, i)->setText
			  (dvd.price->cleanText());
		      else if(names.at(i) == "Language")
			qmain->getUI().table->item(m_row, i)->setText
			  (dvd.language->currentText().trimmed());
		      else if(names.at(i) == "Monetary Units")
			qmain->getUI().table->item(m_row, i)->setText
			  (dvd.monetary_units->currentText().trimmed());
		      else if(names.at(i) == "Quantity")
			qmain->getUI().table->item(m_row, i)->setText
			  (dvd.quantity->text());
		      else if(names.at(i) == "Location")
			qmain->getUI().table->item(m_row, i)->setText
			  (dvd.location->currentText().trimmed());
		      else if(names.at(i) == "Availability")
			{
			  qmain->getUI().table->item(m_row, i)->setText
			    (biblioteq_misc_functions::getAvailability
			     (m_oid, qmain->getDB(), "DVD", errorstr));

			  if(!errorstr.isEmpty())
			    qmain->addError
			      (QString(tr("Database Error")),
			       QString(tr("Retrieving availability.")),
			       errorstr, __FILE__, __LINE__);
			}
		      else if(names.at(i) == "Accession Number")
			qmain->getUI().table->item(m_row, i)->setText
			  (dvd.accession_number->text().trimmed());
		    }

		  qmain->getUI().table->setSortingEnabled(true);

		  foreach(QLineEdit *textfield, findChildren<QLineEdit *> ())
		    textfield->setCursorPosition(0);

		  qmain->slotResizeColumns();
		}

	      qmain->slotDisplaySummary();
	      qmain->updateSceneItem(m_oid, "DVD", dvd.front_image->m_image);
	    }
	  else
	    {
	      QApplication::setOverrideCursor(Qt::WaitCursor);
	      m_oid = biblioteq_misc_functions::getOID(dvd.id->text(),
						       "DVD",
						       qmain->getDB(),
						       errorstr);
	      QApplication::restoreOverrideCursor();

	      if(!errorstr.isEmpty())
		{
		  qmain->addError(QString(tr("Database Error")),
				  QString(tr("Unable to retrieve the DVD's "
					     "OID.")),
				  errorstr, __FILE__, __LINE__);
		  QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
					tr("Unable to retrieve the DVD's "
					   "OID."));
		}
	      else
		qmain->replaceDVD(m_oid, this);

	      updateWindow(biblioteq::EDITABLE);

	      if(qmain->getUI().actionAutoPopulateOnCreation->isChecked())
		(void) qmain->populateTable
		  (biblioteq::POPULATE_ALL, "DVDs", QString(""));

	      raise();
	    }

	  storeData(this);
	}

      return;

    db_rollback:

      QApplication::setOverrideCursor(Qt::WaitCursor);

      if(m_engWindowTitle.contains("Create"))
	m_oid.clear();

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
  else if(m_engWindowTitle.contains("Search"))
    {
      searchstr = "SELECT DISTINCT dvd.title, "
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
	"COUNT(item_borrower_vw.item_oid) "
	"AS availability, "
	"COUNT(item_borrower_vw.item_oid) AS total_reserved, "
	"dvd.accession_number, "
	"dvd.type, "
	"dvd.myoid, "
	"dvd.front_cover "
	"FROM "
	"dvd LEFT JOIN item_borrower_vw ON "
	"dvd.myoid = item_borrower_vw.item_oid "
	"AND item_borrower_vw.type = 'DVD' "
	"WHERE ";
      searchstr.append("LOWER(id) LIKE LOWER('%").append
	(dvd.id->text().trimmed()).append("%') AND ");

      QString ESCAPE("");
      QString UNACCENT(qmain->unaccent());

      if(qmain->getDB().driverName() != "QSQLITE")
	ESCAPE = "E";

      searchstr.append
	(UNACCENT + "(LOWER(dvdformat)) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%").
	append(biblioteq_myqstring::escape(dvd.format->text().trimmed())).
	append("%')) AND ");

      if(dvd.aspectratio->currentIndex() != 0)
	searchstr.append
	  (UNACCENT + "(dvdaspectratio) = " + UNACCENT + "(" + ESCAPE + "'" +
	   dvd.aspectratio->currentText().trimmed() +
	   "') AND ");

      searchstr.append
	(UNACCENT + "(LOWER(dvdactor)) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%").
	append(biblioteq_myqstring::escape(dvd.actors->toPlainText().
					   trimmed())).
	append("%')) AND ");
      searchstr.append
	(UNACCENT + "(LOWER(dvddirector)) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%").
	append(biblioteq_myqstring::escape(dvd.directors->toPlainText().
					   trimmed())).
	append("%')) AND ");

      if(dvd.no_of_discs->value() > 0)
	searchstr.append("dvddiskcount = ").append
	  (dvd.no_of_discs->text()).append(" AND ");

      if(dvd.runtime->text() != "00:00:00")
	searchstr.append("dvdruntime = '" + dvd.runtime->text() +
			 "' AND ");

      if(dvd.rating->currentIndex() != 0)
	searchstr.append
	  (UNACCENT + "(dvdrating) = " + UNACCENT + "(" + ESCAPE + "'" +
	   biblioteq_myqstring::escape(dvd.rating->currentText().trimmed()) +
	   "') AND ");

      if(dvd.region->currentIndex() != 0)
	searchstr.append
	  (UNACCENT + "(dvdregion) = " + UNACCENT + "(" + ESCAPE + "'" +
	   biblioteq_myqstring::escape(dvd.region->currentText().
				       trimmed()) +
	   "') AND ");

      searchstr.append
	(UNACCENT + "(LOWER(title)) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%").append
	(biblioteq_myqstring::escape(dvd.title->text().trimmed())).
	append("%')) AND ");

      if(dvd.publication_date_enabled->isChecked())
	searchstr.append("SUBSTR(rdate, 7) = '" +
			 dvd.release_date->date().toString("yyyy") +
			 "' AND ");

      searchstr.append
	(UNACCENT + "(LOWER(studio)) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%" +
	 biblioteq_myqstring::escape(dvd.studio->toPlainText().trimmed()) +
	 "%')) AND ");
      searchstr.append
	(UNACCENT + "(LOWER(category)) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%" +
	 biblioteq_myqstring::escape(dvd.category->toPlainText().trimmed()) +
	 "%')) AND ");

      if(dvd.price->value() > -0.01)
	{
	  searchstr.append("price = ");
	  searchstr.append(QString::number(dvd.price->value()));
	  searchstr.append(" AND ");
	}

      if(dvd.language->currentIndex() != 0)
	searchstr.append
	  (UNACCENT + "(language) = " + UNACCENT + "(" + ESCAPE + "'" +
	   biblioteq_myqstring::escape
	   (dvd.language->currentText().trimmed()) +
	   "') AND ");

      if(dvd.monetary_units->currentIndex() != 0)
	searchstr.append
	  (UNACCENT + "(monetary_units) = " + UNACCENT + "(" + ESCAPE + "'" +
	   biblioteq_myqstring::escape(dvd.monetary_units->currentText().
				       trimmed()) +
	   "') AND ");

      searchstr.append
	(UNACCENT + "(LOWER(description)) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%" +
	 biblioteq_myqstring::escape(dvd.description->toPlainText().
				     trimmed()) + "%')) ");
      searchstr.append
	("AND " + UNACCENT + "(LOWER(COALESCE(keyword, ''))) LIKE " +
	 UNACCENT + "(LOWER(" + ESCAPE + "'%" +
	 biblioteq_myqstring::escape(dvd.keyword->toPlainText().trimmed()) +
	 "%')) ");

      if(dvd.quantity->value() != 0)
	searchstr.append("AND quantity = " + dvd.quantity->text() + " ");

      if(dvd.location->currentIndex() != 0)
	searchstr.append
	  ("AND " + UNACCENT + "(location) = " + UNACCENT + "("
	   + ESCAPE + "'" +
	   biblioteq_myqstring::escape(dvd.location->currentText().
				       trimmed()) + "') ");

      searchstr.append
	("AND " + UNACCENT + "(LOWER(COALESCE(accession_number, '')))" +
	 "LIKE " + UNACCENT + "(LOWER(" +
	 ESCAPE + "'%" +
	 biblioteq_myqstring::escape(dvd.accession_number->
				     text().trimmed()) + "%')) ");

      /*
      ** Search the database.
      */

      (void) qmain->populateTable
	(biblioteq::POPULATE_SEARCH, "DVDs", searchstr);
    }
}

void biblioteq_dvd::search(const QString &field, const QString &value)
{
  dvd.coverImages->setVisible(false);
  dvd.id->clear();
  dvd.actors->clear();
  dvd.directors->clear();
  dvd.format->clear();
  dvd.title->clear();
  dvd.studio->clear();
  dvd.category->clear();
  dvd.description->clear();
  dvd.keyword->clear();
  dvd.copiesButton->setVisible(false);
  dvd.queryButton->setVisible(false);
  dvd.showUserButton->setVisible(false);
  dvd.okButton->setText(tr("&Search"));
  dvd.publication_date_enabled->setVisible(true);
  dvd.release_date->setDate(QDate::fromString("2001", "yyyy"));
  dvd.release_date->setDisplayFormat("yyyy");
  dvd.runtime->setTime(QTime(0, 0, 0));
  dvd.runtime->setMinimumTime(QTime(0, 0, 0));
  dvd.price->setMinimum(-0.01);
  dvd.price->setValue(-0.01);
  dvd.quantity->setMinimum(0);
  dvd.quantity->setValue(0);
  dvd.no_of_discs->setMinimum(0);
  dvd.no_of_discs->setValue(0);
  dvd.language->insertItem(0, tr("Any"));
  dvd.monetary_units->insertItem(0, tr("Any"));
  dvd.location->insertItem(0, tr("Any"));
  dvd.rating->insertItem(0, tr("Any"));
  dvd.region->insertItem(0, tr("Any"));
  dvd.aspectratio->insertItem(0, tr("Any"));
  dvd.location->setCurrentIndex(0);
  dvd.language->setCurrentIndex(0);
  dvd.monetary_units->setCurrentIndex(0);
  dvd.rating->setCurrentIndex(0);
  dvd.region->setCurrentIndex(0);
  dvd.aspectratio->setCurrentIndex(0);
  dvd.accession_number->clear();

  if(field.isEmpty() && value.isEmpty())
    {
      QList<QAction *> actions = dvd.resetButton->menu()->actions();

      if(actions.size() >= 2)
	{
	  actions[0]->setVisible(false);
	  actions[1]->setVisible(false);
	}

      actions.clear();
      setWindowTitle(tr("BiblioteQ: Database DVD Search"));
      m_engWindowTitle = "Search";
      dvd.id->setFocus();
      biblioteq_misc_functions::center(this, m_parentWid);
      showNormal();
      activateWindow();
      raise();
    }
  else
    {
      if(field == "actors")
	dvd.actors->setPlainText(value);
      else if(field == "directors")
	dvd.directors->setPlainText(value);
      else if(field == "studio")
	dvd.studio->setPlainText(value);
      else if(field == "category")
	dvd.category->setPlainText(value);
      else if(field == "keyword")
	dvd.keyword->setPlainText(value);

      slotGo();
    }
}

void biblioteq_dvd::updateWindow(const int state)
{
  QString str = "";

  if(state == biblioteq::EDITABLE)
    {
      dvd.showUserButton->setEnabled(true);
      dvd.copiesButton->setEnabled(true);
      dvd.okButton->setVisible(true);
      dvd.queryButton->setVisible(m_isQueryEnabled);
      dvd.resetButton->setVisible(true);
      dvd.frontButton->setVisible(true);
      dvd.backButton->setVisible(true);
      str = QString(tr("BiblioteQ: Modify DVD Entry (")) +
	dvd.id->text() + tr(")");
      m_engWindowTitle = "Modify";
    }
  else
    {
      if(qmain->isGuest())
	dvd.showUserButton->setVisible(false);
      else
	dvd.showUserButton->setEnabled(true);

      dvd.copiesButton->setVisible(false);
      dvd.okButton->setVisible(false);
      dvd.queryButton->setVisible(false);
      dvd.resetButton->setVisible(false);
      dvd.frontButton->setVisible(false);
      dvd.backButton->setVisible(false);
      str = QString(tr("BiblioteQ: View DVD Details (")) +
	dvd.id->text() + tr(")");
      m_engWindowTitle = "View";
    }

  dvd.coverImages->setVisible(true);
  setReadOnlyFields(this, state != biblioteq::EDITABLE);
  setWindowTitle(str);
}

void biblioteq_dvd::modify(const int state)
{
  int i = 0;
  QString str = "";
  QString fieldname = "";
  QVariant var;
  QSqlQuery query(qmain->getDB());

  if(state == biblioteq::EDITABLE)
    {
      setReadOnlyFields(this, false);
      setWindowTitle(tr("BiblioteQ: Modify DVD Entry"));
      m_engWindowTitle = "Modify";
      dvd.showUserButton->setEnabled(true);
      dvd.copiesButton->setEnabled(true);
      dvd.okButton->setVisible(true);
      dvd.queryButton->setVisible(m_isQueryEnabled);
      dvd.resetButton->setVisible(true);
      dvd.frontButton->setVisible(true);
      dvd.backButton->setVisible(true);
      biblioteq_misc_functions::highlightWidget
	(dvd.id, QColor(255, 248, 220));
      biblioteq_misc_functions::highlightWidget
	(dvd.title, QColor(255, 248, 220));
      biblioteq_misc_functions::highlightWidget
	(dvd.actors->viewport(), QColor(255, 248, 220));
      biblioteq_misc_functions::highlightWidget
	(dvd.directors->viewport(), QColor(255, 248, 220));
      biblioteq_misc_functions::highlightWidget
	(dvd.description->viewport(), QColor(255, 248, 220));
      biblioteq_misc_functions::highlightWidget
	(dvd.studio->viewport(), QColor(255, 248, 220));
      biblioteq_misc_functions::highlightWidget
	(dvd.format, QColor(255, 248, 220));
      biblioteq_misc_functions::highlightWidget
	(dvd.category->viewport(), QColor(255, 248, 220));
    }
  else
    {
      setReadOnlyFields(this, true);
      setWindowTitle(tr("BiblioteQ: View DVD Details"));
      m_engWindowTitle = "View";

      if(qmain->isGuest())
	dvd.showUserButton->setVisible(false);
      else
	dvd.showUserButton->setEnabled(true);

      dvd.copiesButton->setVisible(false);
      dvd.okButton->setVisible(false);
      dvd.queryButton->setVisible(false);
      dvd.resetButton->setVisible(false);
      dvd.frontButton->setVisible(false);
      dvd.backButton->setVisible(false);

      QList<QAction *> actions = dvd.resetButton->menu()->actions();

      if(actions.size() >= 2)
	{
	  actions[0]->setVisible(false);
	  actions[1]->setVisible(false);
	}

      actions.clear();
    }

  dvd.queryButton->setEnabled(true);
  dvd.okButton->setText(tr("&Save"));
  dvd.runtime->setMinimumTime(QTime(0, 0, 1));
  dvd.price->setMinimum(0.00);
  dvd.quantity->setMinimum(1);
  dvd.quantity->setValue(1);
  dvd.no_of_discs->setMinimum(1);
  dvd.no_of_discs->setValue(1);
  str = m_oid;
  query.prepare("SELECT id, "
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
		"front_cover, "
		"back_cover, "
		"keyword, "
		"accession_number "
		"FROM "
		"dvd "
		"WHERE myoid = ?");
  query.bindValue(0, str);
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!query.exec() || !query.next())
    {
      QApplication::restoreOverrideCursor();
      qmain->addError
	(QString(tr("Database Error")),
	 QString(tr("Unable to retrieve the selected DVD's data.")),
	 query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to retrieve the selected DVD's "
			       "data."));
      close();
      return;
    }
  else
    {
      QApplication::restoreOverrideCursor();
      showNormal();
      activateWindow();
      raise();

      QSqlRecord record(query.record());

      for(i = 0; i < record.count(); i++)
	{
	  var = record.field(i).value();
	  fieldname = record.fieldName(i);

	  if(fieldname == "title")
	    dvd.title->setText(var.toString());
	  else if(fieldname == "studio")
	    dvd.studio->setMultipleLinks
	      ("dvd_search", "studio",
	       var.toString());
	  else if(fieldname == "rdate")
	    dvd.release_date->setDate
	      (QDate::fromString(var.toString(), "MM/dd/yyyy"));
	  else if(fieldname == "price")
	    dvd.price->setValue(var.toDouble());
	  else if(fieldname == "category")
	    dvd.category->setMultipleLinks("dvd_search", "category",
					   var.toString());
	  else if(fieldname == "language")
	    {
	      if(dvd.language->findText(var.toString()) > -1)
		dvd.language->setCurrentIndex
		  (dvd.language->findText(var.toString()));
	      else
		dvd.language->setCurrentIndex
		  (dvd.language->findText(tr("UNKNOWN")));
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
		  (dvd.monetary_units->findText(tr("UNKNOWN")));
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
		  (dvd.location->findText(tr("UNKNOWN")));
	    }
	  else if(fieldname == "id")
	    {
	      if(state == biblioteq::EDITABLE)
		{
		  str = QString(tr("BiblioteQ: Modify DVD Entry (")) +
		    var.toString() + tr(")");
		  m_engWindowTitle = "Modify";
		}
	      else
		{
		  str = QString(tr("BiblioteQ: View DVD Details (")) +
		    var.toString() + tr(")");
		  m_engWindowTitle = "View";
		}

	      setWindowTitle(str);
	      dvd.id->setText(var.toString());
	    }
	  else if(fieldname == "description")
	    dvd.description->setPlainText(var.toString());
	  else if(fieldname == "keyword")
	    dvd.keyword->setMultipleLinks("dvd_search",
					  "keyword",
					  var.toString());
	  else if(fieldname == "dvdformat")
	    dvd.format->setText(var.toString());
	  else if(fieldname == "dvdactor")
	    dvd.actors->setMultipleLinks("dvd_search", "actors",
					 var.toString());
	  else if(fieldname == "dvddirector")
	    dvd.directors->setMultipleLinks("dvd_search", "directors",
					    var.toString());
	  else if(fieldname == "dvdrating")
	    {
	      if(dvd.rating->findText(var.toString()) > -1)
		dvd.rating->setCurrentIndex
		  (dvd.rating->findText(var.toString()));
	      else
		dvd.rating->setCurrentIndex
		  (dvd.rating->findText(tr("UNKNOWN")));
	    }
	  else if(fieldname == "dvdregion")
	    {
	      if(dvd.region->findText(var.toString()) > -1)
		dvd.region->setCurrentIndex
		  (dvd.region->findText(var.toString()));
	      else
		dvd.region->setCurrentIndex
		  (dvd.region->findText(tr("UNKNOWN")));
	    }
	  else if(fieldname == "dvdaspectratio")
	    {
	      if(dvd.aspectratio->findText(var.toString()) > -1)
		dvd.aspectratio->setCurrentIndex
		  (dvd.aspectratio->findText(var.toString()));
	      else
		dvd.aspectratio->setCurrentIndex
		  (dvd.aspectratio->findText(tr("UNKNOWN")));
	    }
	  else if(fieldname == "front_cover")
	    {
	      if(!record.field(i).isNull())
		{
		  dvd.front_image->loadFromData
		    (QByteArray::fromBase64(var.toByteArray()));

		  if(dvd.front_image->m_image.isNull())
		    dvd.front_image->loadFromData(var.toByteArray());
		}
	    }
	  else if(fieldname == "back_cover")
	    {
	      if(!record.field(i).isNull())
		{
		  dvd.back_image->loadFromData
		    (QByteArray::fromBase64(var.toByteArray()));

		  if(dvd.back_image->m_image.isNull())
		    dvd.back_image->loadFromData(var.toByteArray());
		}
	    }
	  else if(fieldname == "accession_number")
	    dvd.accession_number->setText(var.toString());
	}

      foreach(QLineEdit *textfield, findChildren<QLineEdit *> ())
	textfield->setCursorPosition(0);

      storeData(this);
    }

  dvd.id->setFocus();
  raise();
}

void biblioteq_dvd::insert(void)
{
  slotReset();
  dvd.id->clear();
  dvd.actors->setPlainText("N/A");
  dvd.directors->setPlainText("N/A");
  dvd.format->setText("N/A");
  dvd.title->clear();
  dvd.studio->setPlainText("N/A");
  dvd.description->setPlainText("N/A");
  dvd.keyword->clear();
  dvd.category->setPlainText("N/A");
  dvd.copiesButton->setEnabled(false);
  dvd.showUserButton->setEnabled(false);
  dvd.queryButton->setEnabled(true);
  dvd.okButton->setText(tr("&Save"));
  dvd.release_date->setDate(QDate::fromString("01/01/2000",
					      "MM/dd/yyyy"));
  dvd.runtime->setTime(QTime(0, 0, 1));
  dvd.runtime->setMinimumTime(QTime(0, 0, 1));
  dvd.price->setMinimum(0.00);
  dvd.price->setValue(0.00);
  dvd.quantity->setMinimum(1);
  dvd.quantity->setValue(1);
  dvd.no_of_discs->setMinimum(1);
  dvd.no_of_discs->setValue(1);
  dvd.location->setCurrentIndex(0);
  dvd.language->setCurrentIndex(0);
  dvd.monetary_units->setCurrentIndex(0);
  dvd.rating->setCurrentIndex(0);
  dvd.region->setCurrentIndex(0);
  dvd.aspectratio->setCurrentIndex(0);
  dvd.accession_number->clear();
  biblioteq_misc_functions::highlightWidget
    (dvd.id, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (dvd.title, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (dvd.actors->viewport(), QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (dvd.directors->viewport(), QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (dvd.description->viewport(), QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (dvd.studio->viewport(), QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (dvd.format, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (dvd.category->viewport(), QColor(255, 248, 220));
  setWindowTitle(tr("BiblioteQ: Create DVD Entry"));
  m_engWindowTitle = "Create";
  dvd.id->setFocus();
  storeData(this);
  showNormal();
  activateWindow();
  raise();
}

void biblioteq_dvd::slotReset(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(action != 0)
    {
      QList<QAction *> actions = dvd.resetButton->menu()->actions();

      if(actions.size() < 23)
	{
	  // Error.
	}
      else if(action == actions[0])
	dvd.front_image->clear();
      else if(action == actions[1])
	dvd.back_image->clear();
      else if(action == actions[2])
	{
	  dvd.id->clear();
	  dvd.id->setFocus();
	}
      else if(action == actions[3])
	{
	  dvd.rating->setCurrentIndex(0);
	  dvd.rating->setFocus();
	}
      else if(action == actions[4])
	{
	  if(m_engWindowTitle.contains("Search"))
	    dvd.actors->clear();
	  else
	    dvd.actors->setPlainText("N/A");

	  dvd.actors->setFocus();
	}
      else if(action == actions[5])
	{
	  if(m_engWindowTitle.contains("Search"))
	    dvd.directors->clear();
	  else
	    dvd.directors->setPlainText("N/A");

	  dvd.directors->setFocus();
	}
      else if(action == actions[6])
	{
	  dvd.no_of_discs->setValue(dvd.no_of_discs->minimum());
	  dvd.no_of_discs->setFocus();
	}
      else if(action == actions[7])
	{
	  if(m_engWindowTitle.contains("Search"))
	    dvd.runtime->setTime(QTime(0, 0, 0));
	  else
	    dvd.runtime->setTime(QTime(0, 0, 1));

	  dvd.runtime->setFocus();
	}
      else if(action == actions[8])
	{
	  if(m_engWindowTitle.contains("Search"))
	    dvd.format->clear();
	  else
	    dvd.format->setText("N/A");

	  dvd.format->setFocus();
	}
      else if(action == actions[9])
	{
	  dvd.region->setCurrentIndex(0);
	  dvd.region->setFocus();
	}
      else if(action == actions[10])
	{
	  dvd.aspectratio->setCurrentIndex(0);
	  dvd.aspectratio->setFocus();
	}
      else if(action == actions[11])
	{
	  dvd.title->clear();
	  dvd.title->setFocus();
	}
      else if(action == actions[12])
	{
	  if(m_engWindowTitle.contains("Search"))
	    {
	      dvd.publication_date_enabled->setChecked(false);
	      dvd.release_date->setDate(QDate::fromString("2001", "yyyy"));
	    }
	  else
	    dvd.release_date->setDate
	      (QDate::fromString("01/01/2000", "MM/dd/yyyy"));

	  dvd.release_date->setFocus();
	}
      else if(action == actions[13])
	{
	  if(m_engWindowTitle.contains("Search"))
	    dvd.studio->clear();
	  else
	    dvd.studio->setPlainText("N/A");

	  dvd.studio->setFocus();
	}
      else if(action == actions[14])
	{
	  if(m_engWindowTitle.contains("Search"))
	    dvd.category->clear();
	  else
	    dvd.category->setPlainText("N/A");

	  dvd.category->setFocus();
	}
      else if(action == actions[15])
	{
	  dvd.price->setValue(dvd.price->minimum());
	  dvd.price->setFocus();
	}
      else if(action == actions[16])
	{
	  dvd.language->setCurrentIndex(0);
	  dvd.language->setFocus();
	}
      else if(action == actions[17])
	{
	  dvd.monetary_units->setCurrentIndex(0);
	  dvd.monetary_units->setFocus();
	}
      else if(action == actions[18])
	{
	  dvd.quantity->setValue(dvd.quantity->minimum());
	  dvd.quantity->setFocus();
	}
      else if(action == actions[19])
	{
	  dvd.location->setCurrentIndex(0);
	  dvd.location->setFocus();
	}
      else if(action == actions[20])
	{
	  if(m_engWindowTitle.contains("Search"))
	    dvd.description->clear();
	  else
	    dvd.description->setPlainText("N/A");

	  dvd.description->setFocus();
	}
      else if(action == actions[21])
	{
	  dvd.keyword->clear();
	  dvd.keyword->setFocus();
	}
      else if(action == actions[22])
	{
	  dvd.accession_number->clear();
	  dvd.accession_number->setFocus();
	}

      actions.clear();
    }
  else
    {
      /*
      ** Reset all.
      */

      dvd.title->clear();

      if(m_engWindowTitle.contains("Search"))
	dvd.actors->clear();
      else
	dvd.actors->setPlainText("N/A");

      if(m_engWindowTitle.contains("Search"))
	dvd.directors->clear();
      else
	dvd.directors->setPlainText("N/A");

      if(m_engWindowTitle.contains("Search"))
	dvd.format->clear();
      else
	dvd.format->setText("N/A");

      if(m_engWindowTitle.contains("Search"))
	dvd.studio->clear();
      else
	dvd.studio->setPlainText("N/A");

      if(m_engWindowTitle.contains("Search"))
	dvd.category->clear();
      else
	dvd.category->setPlainText("N/A");

      if(m_engWindowTitle.contains("Search"))
	dvd.description->clear();
      else
	dvd.description->setPlainText("N/A");

      if(m_engWindowTitle.contains("Search"))
	{
	  dvd.publication_date_enabled->setChecked(false);
	  dvd.release_date->setDate(QDate::fromString("2001", "yyyy"));
	  dvd.runtime->setTime(QTime(0, 0, 0));
	}
      else
	{
	  dvd.release_date->setDate(QDate::fromString("01/01/2000",
						      "MM/dd/yyyy"));
	  dvd.runtime->setTime(QTime(0, 0, 1));
	}

      dvd.id->clear();
      dvd.price->setValue(dvd.price->minimum());
      dvd.quantity->setValue(dvd.quantity->minimum());
      dvd.no_of_discs->setValue(dvd.no_of_discs->minimum());
      dvd.location->setCurrentIndex(0);
      dvd.language->setCurrentIndex(0);
      dvd.monetary_units->setCurrentIndex(0);
      dvd.rating->setCurrentIndex(0);
      dvd.region->setCurrentIndex(0);
      dvd.aspectratio->setCurrentIndex(0);
      dvd.front_image->clear();
      dvd.back_image->clear();
      dvd.keyword->clear();
      dvd.accession_number->clear();
      dvd.id->setFocus();
    }
}

void biblioteq_dvd::closeEvent(QCloseEvent *e)
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

  qmain->removeDVD(this);
}

void biblioteq_dvd::slotCancel(void)
{
  close();
}

void biblioteq_dvd::slotPopulateCopiesEditor(void)
{
  biblioteq_copy_editor *copyeditor = 0;

  if((copyeditor = new(std::nothrow) biblioteq_copy_editor
      (qobject_cast<QWidget *> (this),
       static_cast<biblioteq_item *> (this),
       false,
       dvd.quantity->value(), m_oid,
       dvd.quantity, font(), "DVD", dvd.id->text().trimmed())) != 0)
    copyeditor->populateCopiesEditor();
}

void biblioteq_dvd::slotShowUsers(void)
{
  int state = 0;
  biblioteq_borrowers_editor *borrowerseditor = 0;

  if(!dvd.okButton->isHidden())
    state = biblioteq::EDITABLE;
  else
    state = biblioteq::VIEW_ONLY;

  if((borrowerseditor = new(std::nothrow) biblioteq_borrowers_editor
      (qobject_cast<QWidget *> (this), static_cast<biblioteq_item *> (this),
       dvd.quantity->value(), m_oid, dvd.id->text(), font(), "DVD",
       state)) != 0)
    borrowerseditor->showUsers();
}

void biblioteq_dvd::slotQuery(void)
{
}

void biblioteq_dvd::slotPrint(void)
{
  m_html = "<html>";
  m_html += "<b>" + tr("UPC:") + "</b> " + dvd.id->text().trimmed() + "<br>";
  m_html += "<b>" + tr("Rating:") + "</b> " + dvd.rating->currentText() +
    "<br>";
  m_html += "<b>" + tr("Actors:") + "</b> " +
    dvd.actors->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Directors:") + "</b> " +
    dvd.directors->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Number of Discs:") + "</b> " +
    dvd.no_of_discs->text() + "<br>";
  m_html += "<b>" + tr("Runtime:") + "</b> " + dvd.runtime->text() + "<br>";
  m_html += "<b>" + tr("Format:") + "</b> " +
    dvd.format->text().trimmed() + "<br>";
  m_html += "<b>" + tr("Region:") + "</b> " + dvd.region->currentText() +
    "<br>";
  m_html += "<b>" + tr("Aspect Ratio:") + "</b> " +
    dvd.aspectratio->currentText() + "<br>";

  /*
  ** General information.
  */

  m_html += "<b>" + tr("Title:") + "</b> " +
    dvd.title->text().trimmed() + "<br>";
  m_html += "<b>" + tr("Release Date:") + "</b> " + dvd.release_date->date().
    toString(Qt::ISODate) + "<br>";
  m_html += "<b>" + tr("Studio:") + "</b> " +
    dvd.studio->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Category:") + "</b> " +
    dvd.category->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Price:") + "</b> " + dvd.price->cleanText() + "<br>";
  m_html += "<b>" + tr("Language:") + "</b> " +
    dvd.language->currentText() + "<br>";
  m_html += "<b>" + tr("Monetary Units:") + "</b> " +
    dvd.monetary_units->currentText() + "<br>";
  m_html += "<b>" + tr("Copies:") + "</b> " + dvd.quantity->text() + "<br>";
  m_html += "<b>" + tr("Location:") + "</b> " +
    dvd.location->currentText() + "<br>";
  m_html += "<b>" + tr("Abstract:") + "</b> " +
    dvd.description->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Keywords:") + "</b> " +
    dvd.keyword->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Accession Number:") + "</b> " +
    dvd.accession_number->text().trimmed();
  m_html += "</html>";
  print(this);
}

void biblioteq_dvd::slotSelectImage(void)
{
  QFileDialog dialog(this);
  QPushButton *button = qobject_cast<QPushButton *> (sender());

  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setDirectory(QDir::homePath());
  dialog.setOption(QFileDialog::DontUseNativeDialog);

  if(button == dvd.frontButton)
    dialog.setWindowTitle(tr("BiblioteQ: Front Cover Image Selection"));
  else
    dialog.setWindowTitle(tr("BiblioteQ: Back Cover Image Selection"));

  dialog.exec();

  if(dialog.result() == QDialog::Accepted)
    {
      if(button == dvd.frontButton)
	{
	  dvd.front_image->clear();
	  dvd.front_image->m_image = QImage(dialog.selectedFiles().value(0));

	  if(dialog.selectedFiles().value(0).lastIndexOf(".") > -1)
	    dvd.front_image->m_imageFormat = dialog.selectedFiles().value(0).mid
	      (dialog.selectedFiles().value(0).lastIndexOf(".") + 1).
	      toUpper();

	  dvd.front_image->scene()->addPixmap
	    (QPixmap::fromImage(dvd.front_image->m_image));

	  if(dvd.front_image->scene()->items().size() > 0)
	    dvd.front_image->scene()->items().at(0)->setFlags
	      (QGraphicsItem::ItemIsSelectable);

	  dvd.front_image->scene()->setSceneRect
	    (dvd.front_image->scene()->itemsBoundingRect());
	}
      else
	{
	  dvd.back_image->clear();
	  dvd.back_image->m_image = QImage(dialog.selectedFiles().value(0));

	  if(dialog.selectedFiles().value(0).lastIndexOf(".") > -1)
	    dvd.back_image->m_imageFormat = dialog.selectedFiles().value(0).mid
	      (dialog.selectedFiles().value(0).lastIndexOf(".") + 1).
	      toUpper();

	  dvd.back_image->scene()->addPixmap
	    (QPixmap::fromImage(dvd.back_image->m_image));

	  if(dvd.back_image->scene()->items().size() > 0)
	    dvd.back_image->scene()->items().at(0)->setFlags
	      (QGraphicsItem::ItemIsSelectable);

	  dvd.back_image->scene()->setSceneRect
	    (dvd.back_image->scene()->itemsBoundingRect());
	}
    }
}

void biblioteq_dvd::duplicate(const QString &p_oid, const int state)
{
  modify(state); // Initial population.
  dvd.copiesButton->setEnabled(false);
  dvd.showUserButton->setEnabled(false);
  m_oid = p_oid;
  setWindowTitle(tr("BiblioteQ: Duplicate DVD Entry"));
  m_engWindowTitle = "Create";
}

void biblioteq_dvd::changeEvent(QEvent *event)
{
  if(event)
    switch(event->type())
      {
      case QEvent::LanguageChange:
	{
	  dvd.retranslateUi(this);
	  break;
	}
      default:
	break;
      }

  QMainWindow::changeEvent(event);
}

void biblioteq_dvd::slotPublicationDateEnabled(bool state)
{
  dvd.release_date->setEnabled(state);

  if(!state)
    dvd.release_date->setDate(QDate::fromString("2001", "yyyy"));
}
