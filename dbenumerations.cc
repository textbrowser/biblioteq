/*
** -- Local Includes --
*/

#include "dbenumerations.h"
#include "misc_functions.h"

extern qtbook *qmain;
extern QApplication *qapp;

/*
** -- dbenumerations() --
*/

dbenumerations::dbenumerations(void):QMainWindow()
{
  ui.setupUi(this);
  connect(ui.saveButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSave(void)));
  connect(ui.cancelButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
  connect(ui.reloadButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotReload(void)));
  connect(ui.addCdFormat,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(ui.removeCdFormat,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(ui.addDvdAspectRatio,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(ui.removeDvdAspectRatio,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(ui.addDvdRating,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(ui.removeDvdRating,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(ui.addDvdRegion,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(ui.removeDvdRegion,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(ui.addLanguage,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(ui.removeLanguage,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(ui.addMonetaryUnit,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(ui.removeMonetaryUnit,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(ui.addVideoGamePlatform,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(ui.removeVideoGamePlatform,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(ui.addVideoGameRating,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(ui.removeVideoGameRating,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  ui.locationsTable->verticalHeader()->setResizeMode(QHeaderView::Fixed);
  ui.minimumDaysTable->verticalHeader()->setResizeMode(QHeaderView::Fixed);
}

/*
** -- show() --
*/

void dbenumerations::show(QMainWindow *parent, const bool populate)
{
  misc_functions::center(this, parent);
  raise();
  QMainWindow::show();
  ui.emptyLabel->setMinimumHeight(ui.addCdFormat->height());

  if(populate)
    populateWidgets();
  else
    ui.cdFormatsList->setFocus();
}

/*
** -- slotClose() --
*/

void dbenumerations::slotClose(void)
{
  close();
}

/*
** -- populateWidgets()
*/

void dbenumerations::populateWidgets(void)
{
  QString errorstr("");
  QString forerror("");
  QStringList list;

  for(int i = 0; i < 10; i++)
    {
      QListWidget *listwidget = 0;
      QTableWidget *tablewidget = 0;

      if(i == 0)
	listwidget = ui.cdFormatsList;
      else if(i == 1)
	listwidget = ui.dvdAspectRatiosList;
      else if(i == 2)
	listwidget = ui.dvdRatingsList;
      else if(i == 3)
	listwidget = ui.dvdRegionsList;
      else if(i == 4)
	listwidget = ui.languagesList;
      else if(i == 6)
	tablewidget = ui.minimumDaysTable;
      else if(i == 7)
	listwidget = ui.monetaryUnitsList;
      else if(i == 8)
	listwidget = ui.videoGamePlatformsList;
      else if(i == 9)
	listwidget = ui.videoGameRatingsList;

      if(listwidget)
	listwidget->clear();
      else if(tablewidget)
	tablewidget->clearSelection();

      qapp->setOverrideCursor(Qt::WaitCursor);

      if(i == 0)
	{
	  forerror = "cd formats";
	  list = misc_functions::getCDFormats(qmain->getDB(),
					      errorstr);
	}
      else if(i == 1)
	{
	  forerror = "dvd aspect ratios";
	  list = misc_functions::getDVDAspectRatios(qmain->getDB(),
						    errorstr);
	}
      else if(i == 2)
	{
	  forerror = "dvd ratings";
	  list = misc_functions::getDVDRatings(qmain->getDB(),
					       errorstr);
	}
      else if(i == 3)
	{
	  forerror = "dvd regions";
	  list = misc_functions::getDVDRegions(qmain->getDB(),
					       errorstr);
	}
      else if(i == 4)
	{
	  forerror = "languages";
	  list = misc_functions::getLanguages(qmain->getDB(),
					      errorstr);
	}
      else if(i == 6)
	{
	  forerror = "minimum days";

	  int days = 0;
	  QString lasterror("");

	  for(int j = 0; j < 6; j++)
	    {
	      if(j == 0)
		days = misc_functions::getMinimumDays(qmain->getDB(),
						      "Book",
						      lasterror);
	      else if(j == 1)
		days = misc_functions::getMinimumDays(qmain->getDB(),
						      "DVD",
						      lasterror);
	      else if(j == 2)
		days = misc_functions::getMinimumDays(qmain->getDB(),
						      "Journal",
						      lasterror);
	      else if(j == 3)
		days = misc_functions::getMinimumDays(qmain->getDB(),
						      "Magazine",
						      lasterror);
	      else if(j == 4)
		days = misc_functions::getMinimumDays(qmain->getDB(),
						      "CD",
						      lasterror);
	      else if(j == 5)
		days = misc_functions::getMinimumDays(qmain->getDB(),
						      "Video Game",
						      lasterror);

	      if(errorstr.isEmpty())
		errorstr = lasterror;

	      list.append(QString::number(days));
	    }
	}
      else if(i == 7)
	{
	  forerror = "monetary units";
	  list = misc_functions::getMonetaryUnits(qmain->getDB(),
						  errorstr);
	}
      else if(i == 8)
	{
	  forerror = "video game platforms";
	  list = misc_functions::getVideoGamePlatforms(qmain->getDB(),
						       errorstr);
	}
      else if(i == 9)
	{
	  forerror = "video game ratings";
	  list = misc_functions::getVideoGameRatings(qmain->getDB(),
						     errorstr);
	}

      qapp->restoreOverrideCursor();

      if(!errorstr.isEmpty())
	qmain->addError
	  (QString(tr("Database Error")),
	   QString(tr("Unable to retrieve the ")) +
	   forerror + tr("."),
	   errorstr, __FILE__, __LINE__);
      else if(listwidget)
	while(!list.isEmpty())
	  {
	    QListWidgetItem *item = 0;

	    item = new(std::nothrow) QListWidgetItem(list.takeFirst());

	    if(item)
	      {
		item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled |
			       Qt::ItemIsSelectable) ;
		listwidget->addItem(item);
	      }
	  }
      else if(tablewidget == ui.minimumDaysTable)
	{
	  for(int j = 0; j < list.size(); j++)
	    if(tablewidget->item(j, 1))
	      tablewidget->item(j, 1)->setText(list.at(j));
	}

      list.clear();
    }

  ui.cdFormatsList->setFocus();
}

/*
** -- slotReload() --
*/

void dbenumerations::slotReload(void)
{
  populateWidgets();
}

/*
** -- slotAdd() --
*/

void dbenumerations::slotAdd(void)
{
  QToolButton *toolButton = qobject_cast<QToolButton *>
    (sender());
  QListWidget *list = 0;
  QListWidgetItem *listItem = 0;

  if(toolButton == ui.addCdFormat)
    {
      list = ui.cdFormatsList;
      listItem = new(std::nothrow) QListWidgetItem(tr("CD Format"));
    }
  else if(toolButton == ui.addDvdAspectRatio)
    {
      list = ui.dvdAspectRatiosList;
      listItem = new(std::nothrow) QListWidgetItem(tr("DVD Aspect Ratio"));
    }
  else if(toolButton == ui.addDvdRating)
    {
      list = ui.dvdRatingsList;
      listItem = new(std::nothrow) QListWidgetItem(tr("DVD Rating"));
    }
  else if(toolButton == ui.addDvdRegion)
    {
      list = ui.dvdRegionsList;
      listItem = new(std::nothrow) QListWidgetItem(tr("DVD Region"));
    }
  else if(toolButton == ui.addLanguage)
    {
      list = ui.languagesList;
      listItem = new(std::nothrow) QListWidgetItem(tr("Language"));
    }
  else if(toolButton == ui.addMonetaryUnit)
    {
      list = ui.monetaryUnitsList;
      listItem = new(std::nothrow) QListWidgetItem(tr("Monetary Unit"));
    }
  else if(toolButton == ui.addVideoGamePlatform)
    {
      list = ui.videoGamePlatformsList;
      listItem = new(std::nothrow) QListWidgetItem(tr("Video Game Platform"));
    }
  else if(toolButton == ui.addVideoGameRating)
    {
      list = ui.videoGameRatingsList;
      listItem = new(std::nothrow) QListWidgetItem(tr("Video Game Rating"));
    }

  if(list && listItem)
    {
      listItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled |
			 Qt::ItemIsSelectable) ;
      list->addItem(listItem);
    }
}

/*
** -- slotRemove() --
*/

void dbenumerations::slotRemove(void)
{
  QListWidget *list = 0;
  QToolButton *toolButton = qobject_cast<QToolButton *>
    (sender());

  if(toolButton == ui.removeCdFormat)
    list = ui.cdFormatsList;
  else if(toolButton == ui.removeDvdAspectRatio)
    list = ui.dvdAspectRatiosList;
  else if(toolButton == ui.removeDvdRating)
    list = ui.dvdRatingsList;
  else if(toolButton == ui.removeDvdRegion)
    list = ui.dvdRegionsList;
  else if(toolButton == ui.removeLanguage)
    list = ui.languagesList;
  else if(toolButton == ui.removeMonetaryUnit)
    list = ui.monetaryUnitsList;
  else if(toolButton == ui.removeVideoGamePlatform)
    list = ui.videoGamePlatformsList;
  else if(toolButton == ui.removeVideoGameRating)
    list = ui.videoGameRatingsList;

  if(list)
    delete list->takeItem(list->currentRow());
}

/*
** -- slotSave() --
*/

void dbenumerations::slotSave(void)
{
  QString querystr("");
  QSqlQuery query(qmain->getDB());

  qapp->setOverrideCursor(Qt::WaitCursor);

  for(int i = 0; i < 10; i++)
    {
      if(i == 0)
	querystr = "DELETE FROM cd_formats";
      else if(i == 1)
	querystr = "DELETE FROM dvd_aspect_ratios";
      else if(i == 2)
	querystr = "DELETE FROM dvd_ratings";
      else if(i == 3)
	querystr = "DELETE FROM dvd_regions";
      else if(i == 4)
	querystr = "DELETE FROM languages";
      else if(i == 6)
	querystr = "DELETE FROM minimum_days";
      else if(i == 7)
	querystr = "DELETE FROM monetary_units";
      else if(i == 8)
	querystr = "DELETE FROM videogame_platforms";
      else if(i == 9)
	querystr = "DELETE FROM videogame_ratings";
      else
	continue;

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

      if(!query.exec(querystr))
	{
	  qapp->restoreOverrideCursor();

	  if(i == 0)
	    qmain->addError
	      (QString(tr("Database Error")),
	       QString(tr("An error occurred while attempting to "
			  "remove the cd formats.")),
	       query.lastError().text(), __FILE__, __LINE__);
	  else if(i == 1)
	    qmain->addError
	      (QString(tr("Database Error")),
	       QString(tr("An error occurred while attempting to "
			  "remove the dvd aspect ratios.")),
	       query.lastError().text(), __FILE__, __LINE__);
	  else if(i == 2)
	    qmain->addError
	      (QString(tr("Database Error")),
	       QString(tr("An error occurred while attempting to "
			  "remove the dvd ratings.")),
	       query.lastError().text(), __FILE__, __LINE__);
	  else if(i == 3)
	    qmain->addError
	      (QString(tr("Database Error")),
	       QString(tr("An error occurred while attempting to "
			  "remove the dvd regions.")),
	       query.lastError().text(), __FILE__, __LINE__);
	  else if(i == 4)
	    qmain->addError
	      (QString(tr("Database Error")),
	       QString(tr("An error occurred while attempting to "
			  "remove the languages.")),
	       query.lastError().text(), __FILE__, __LINE__);
	  else if(i == 5)
	    qmain->addError
	      (QString(tr("Database Error")),
	       QString(tr("An error occurred while attempting to "
			  "remove the minimum days.")),
	       query.lastError().text(), __FILE__, __LINE__);
	  else if(i == 7)
	    qmain->addError
	      (QString(tr("Database Error")),
	       QString(tr("An error occurred while attempting to "
			  "remove the monetary units.")),
	       query.lastError().text(), __FILE__, __LINE__);
	  else if(i == 8)
	    qmain->addError
	      (QString(tr("Database Error")),
	       QString(tr("An error occurred while attempting to "
			  "remove the video game platforms.")),
	       query.lastError().text(), __FILE__, __LINE__);
	  else if(i == 9)
	    qmain->addError
	      (QString(tr("Database Error")),
	       QString(tr("An error occurred while attempting to "
			  "remove the video game ratings.")),
	       query.lastError().text(), __FILE__, __LINE__);

	  goto db_rollback;
	}

      QString table("");
      QString forerror("");
      QListWidget *listwidget = 0;
      QTableWidget *tablewidget = 0;

      if(i == 0)
	{
	  table = "cd_formats";
	  forerror = tr("cd format");
	  listwidget = ui.cdFormatsList;
	}
      else if(i == 1)
	{
	  table = "dvd_aspect_ratios";
	  forerror = tr("dvd aspect ratio");
	  listwidget = ui.dvdAspectRatiosList;
	}
      else if(i == 2)
	{
	  table = "dvd_ratings";
	  forerror = tr("dvd rating");
	  listwidget = ui.dvdRatingsList;
	}
      else if(i == 3)
	{
	  table = "dvd_regions";
	  forerror = tr("dvd region");
	  listwidget = ui.dvdRegionsList;
	}
      else if(i == 4)
	{
	  table = "languages";
	  forerror = tr("language");
	  listwidget = ui.languagesList;
	}
      else if(i == 6)
	tablewidget = ui.minimumDaysTable;
      else if(i == 7)
	{
	  table = "monetary_units";
	  forerror = tr("monetary unit");
	  listwidget = ui.monetaryUnitsList;
	}
      else if(i == 8)
	{
	  table = "videogame_platforms";
	  forerror = tr("video game platform");
	  listwidget = ui.videoGamePlatformsList;
	}
      else if(i == 9)
	{
	  table = "videogame_ratings";
	  forerror = tr("video game rating");
	  listwidget = ui.videoGameRatingsList;
	}

      if(listwidget)
	{
	  for(int j = 0; j < listwidget->count(); j++)
	    if(listwidget->item(j))
	      {
		querystr = QString("INSERT INTO %1 VALUES ('%2')").arg
		  (table).arg
		  (listwidget->item(j)->text());

		if(!query.exec(querystr))
		  {
		    qapp->restoreOverrideCursor();
		    qmain->addError
		      (QString(tr("Database Error")),
		       QString(tr("Unable to create the ")) +
		       forerror + tr(" ") +
		       listwidget->item(j)->text() + QString(tr(".")),
		       query.lastError().text(), __FILE__, __LINE__);
		    goto db_rollback;
		  }
	      }
	}
      else if(tablewidget == ui.minimumDaysTable)
	{
	  for(int j = 0; j < tablewidget->rowCount(); j++)
	    if(tablewidget->item(j, 1))
	      {
		if(j == 0)
		  querystr = QString("INSERT INTO minimum_days "
				     "(days, type) VALUES "
				     "(%1, 'Book')").arg
		    (tablewidget->item(j, 1)->text());
		else if(j == 1)
		  querystr = QString("INSERT INTO minimum_days "
				     "(days, type) VALUES "
				     "(%1, 'DVD')").arg
		    (tablewidget->item(j, 1)->text());
		else if(j == 2)
		  querystr = QString("INSERT INTO minimum_days "
				     "(days, type) VALUES "
				     "(%1, 'Journal')").arg
		    (tablewidget->item(j, 1)->text());
		else if(j == 3)
		  querystr = QString("INSERT INTO minimum_days "
				     "(days, type) VALUES "
				     "(%1, 'Magazine')").arg
		    (tablewidget->item(j, 1)->text());
		else if(j == 4)
		  querystr = QString("INSERT INTO minimum_days "
				     "(days, type) VALUES "
				     "(%1, 'CD')").arg
		    (tablewidget->item(j, 1)->text());
		else if(j == 5)
		  querystr = QString("INSERT INTO minimum_days "
				     "(days, type) VALUES "
				     "(%1, 'Video Game')").arg
		    (tablewidget->item(j, 1)->text());

		if(!query.exec(querystr))
		  {
		    qapp->restoreOverrideCursor();
		    qmain->addError
		      (QString(tr("Database Error")),
		       QString(tr("Unable to create the minimum day (Book, ")) +
		       tablewidget->item(j, 1)->text() +
		       QString(tr(").")),
		       query.lastError().text(), __FILE__, __LINE__);
		    goto db_rollback;
		  }
	      }
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
	  QMessageBox::critical(this,
				tr("BiblioteQ: Database Error"),
				tr("Unable to commit the current "
				   "database transaction."));
	  return;
	}
    }

  qapp->restoreOverrideCursor();
  populateWidgets();
  return;

 db_rollback:

  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!qmain->getDB().rollback())
    qmain->addError(QString(tr("Database Error")),
		    QString(tr("Rollback failure.")),
		    qmain->getDB().lastError().text(), __FILE__, __LINE__);

  qapp->restoreOverrideCursor();
  QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			tr("An error occurred while attempting to save "
			   "the database enumerations."));
}
