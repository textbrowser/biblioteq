/*
** -- Local Includes --
*/

#include "qtbook.h"
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
#ifdef Q_OS_MAC
  setAttribute(Qt::WA_MacMetalStyle, true);
#endif
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
  connect(ui.addLocation,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(ui.removeLocation,
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
  static bool resized = false;

  if(!resized)
    resize(0.95 * parent->size().width(),
	   0.95 * parent->size().height());

  resized = true;
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
** -- clear() --
*/

void dbenumerations::clear(void)
{
  foreach(QListWidget *listwidget, findChildren<QListWidget *> ())
    listwidget->clear();

  while(ui.locationsTable->rowCount() > 0)
    ui.locationsTable->removeRow(0);

  ui.minimumDaysTable->clearSelection();
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
  clear();

  QString errorstr("");
  QString forerror("");
  QStringList list;
  QList<QPair<QString, QString> > pairList;

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
      else if(i == 5)
	tablewidget = ui.locationsTable;
      else if(i == 6)
	tablewidget = ui.minimumDaysTable;
      else if(i == 7)
	listwidget = ui.monetaryUnitsList;
      else if(i == 8)
	listwidget = ui.videoGamePlatformsList;
      else if(i == 9)
	listwidget = ui.videoGameRatingsList;

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
      else if(i == 5)
	{
	  forerror = "locations";
	  pairList = misc_functions::getLocations(qmain->getDB(),
						  errorstr);
	}
      else if(i == 6)
	{
	  forerror = "minimum days";
	  list = misc_functions::getMinimumDays(qmain->getDB(),
						errorstr);
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
			       Qt::ItemIsSelectable);
		listwidget->addItem(item);
	      }
	  }
      else if(tablewidget == ui.locationsTable)
	{
	  ui.locationsTable->setRowCount(pairList.size());

	  for(int j = 0; j < pairList.size(); j++)
	    {
	      QComboBox *item1 = new(std::nothrow) QComboBox();
	      QTableWidgetItem *item2 = new(std::nothrow) QTableWidgetItem
		(pairList.at(j).second);

	      if(item1 && item2)
		{
		  QStringList list;

		  list << tr("Book")
		       << tr("DVD")
		       << tr("Journal")
		       << tr("Magazine")
		       << tr("Music CD")
		       << tr("Photograph Collection")
		       << tr("Video Game");
		  item1->addItems(list);

		  if(pairList.at(j).first == "Book")
		    item1->setCurrentIndex(0);
		  else if(pairList.at(j).first == "CD")
		    item1->setCurrentIndex(4);
		  else if(pairList.at(j).first == "DVD")
		    item1->setCurrentIndex(1);
		  else if(pairList.at(j).first == "Journal")
		    item1->setCurrentIndex(2);
		  else if(pairList.at(j).first == "Magazine")
		    item1->setCurrentIndex(3);
		  else if(pairList.at(j).first == "Photograph Collection")
		    item1->setCurrentIndex(5);
		  else if(pairList.at(j).first == "Video Game")
		    item1->setCurrentIndex(6);

		  list.clear();
		  item2->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled |
				  Qt::ItemIsSelectable);
		  ui.locationsTable->setCellWidget(j, 0, item1);
		  ui.locationsTable->setItem(j, 1, item2);
		}
	      else
		{
		  if(item1)
		    item1->deleteLater();

		  if(item2)
		    delete item2;
		}
	    }

	  ui.locationsTable->resizeColumnToContents(0);
	}
      else if(tablewidget == ui.minimumDaysTable)
	{
	  for(int j = 0; j < list.size(); j++)
	    if(tablewidget->item(j, 1))
	      tablewidget->item(j, 1)->setText(list.at(j));

	  ui.minimumDaysTable->resizeColumnToContents(0);
	}

      list.clear();
      pairList.clear();
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
  else if(toolButton == ui.addLocation)
    {
      QComboBox *item1 = new(std::nothrow) QComboBox();
      QTableWidgetItem *item2 = new(std::nothrow) QTableWidgetItem();

      if(item1 && item2)
	{
	  QStringList list;

	  list << tr("Book")
	       << tr("DVD")
	       << tr("Journal")
	       << tr("Magazine")
	       << tr("Music CD")
	       << tr("Photograph Collection")
	       << tr("Video Game");
	  item1->addItems(list);
	  list.clear();
	  item2->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled |
			  Qt::ItemIsSelectable);
	  ui.locationsTable->setRowCount(ui.locationsTable->rowCount() + 1);
	  ui.locationsTable->setCellWidget(ui.locationsTable->rowCount() - 1,
					   0,
					   item1);
	  ui.locationsTable->setItem(ui.locationsTable->rowCount() - 1,
				     1,
				     item2);
	  ui.locationsTable->setCurrentCell(ui.locationsTable->rowCount() - 1,
					    0);
	}
      else
	{
	  if(item1)
	    item1->deleteLater();

	  if(item2)
	    delete item2;
	}
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
			 Qt::ItemIsSelectable);
      list->addItem(listItem);
      list->setCurrentItem(listItem);
      list->editItem(listItem);
    }
  else if(listItem)
    delete listItem;
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
  else if(toolButton == ui.removeLocation)
    ui.locationsTable->removeRow(ui.locationsTable->currentRow());
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
  bool error = false;
  QString table("");
  QString forerror("");
  QString querystr("");
  QSqlQuery query(qmain->getDB());
  QListWidget *listwidget = 0;
  QTableWidget *tablewidget = 0;

  qapp->setOverrideCursor(Qt::WaitCursor);

  for(int i = 0; i < 10; i++)
    {
      table = "";
      forerror = "";
      listwidget = 0;
      tablewidget = 0;

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
      else if(i == 5)
	querystr = "DELETE FROM locations";
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
	  error = true;
	  qmain->addError
	    (QString(tr("Database Error")),
	     QString(tr("Unable to create a database transaction.")),
	     qmain->getDB().lastError().text(), __FILE__, __LINE__);
	  continue;
	}

      if(!query.exec(querystr))
	{
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
			  "remove the locations.")),
	       query.lastError().text(), __FILE__, __LINE__);
	  else if(i == 6)
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
      else if(i == 5)
	tablewidget = ui.locationsTable;
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
		query.prepare(QString("INSERT INTO %1 VALUES (?)").arg
			      (table));
		query.bindValue(0,
				listwidget->item(j)->text().trimmed());

		if(!query.exec())
		  {
		    qmain->addError
		      (QString(tr("Database Error")),
		       QString(tr("Unable to create the ")) +
		       forerror + tr(" ") +
		       listwidget->item(j)->text().trimmed() +
		       QString(tr(".")),
		       query.lastError().text(), __FILE__, __LINE__);
		    goto db_rollback;
		  }
	      }
	}
      else if(tablewidget == ui.locationsTable)
	{
	  for(int j = 0; j < tablewidget->rowCount(); j++)
	    if(tablewidget->cellWidget(j, 0) &&
	       tablewidget->item(j, 1))
	      {
		int index = qobject_cast<QComboBox *>
		  (tablewidget->cellWidget(j, 0))->currentIndex();

		if(index == 0)
		  {
		    query.prepare("INSERT INTO locations "
				  "(location, type) VALUES "
				  "(?, 'Book')");
		    query.bindValue(0,
				    tablewidget->item(j, 1)->text().trimmed());
		  }
		else if(index == 1)
		  {
		    query.prepare("INSERT INTO locations "
				  "(location, type) VALUES "
				  "(?, 'DVD')");
		    query.bindValue(0,
				    tablewidget->item(j, 1)->text().trimmed());
		  }
		else if(index == 2)
		  {
		    query.prepare("INSERT INTO locations "
				  "(location, type) VALUES "
				  "(?, 'Journal')");
		    query.bindValue(0,
				    tablewidget->item(j, 1)->text().trimmed());
		  }
		else if(index == 3)
		  {
		    query.prepare("INSERT INTO locations "
				  "(location, type) VALUES "
				  "(?, 'Magazine')");
		    query.bindValue(0,
				    tablewidget->item(j, 1)->text().trimmed());
		  }
		else if(index == 4)
		  {
		    query.prepare("INSERT INTO locations "
				  "(location, type) VALUES "
				  "(?, 'CD')");
		    query.bindValue(0,
				    tablewidget->item(j, 1)->text().trimmed());
		  }
		else if(index == 5)
		  {
		    query.prepare("INSERT INTO locations "
				  "(location, type) VALUES "
				  "(?, 'Photograph Collection')");
		    query.bindValue(0,
				    tablewidget->item(j, 1)->text().trimmed());
		  }
		else if(index == 6)
		  {
		    query.prepare("INSERT INTO locations "
				  "(location, type) VALUES "
				  "(?, 'Video Game')");
		    query.bindValue(0,
				    tablewidget->item(j, 1)->text().trimmed());
		  }

		if(!query.exec())
		  {
		    qmain->addError
		      (QString(tr("Database Error")),
		       QString(tr("Unable to create the location (")) +
		       qobject_cast<QComboBox *>
		       (tablewidget->cellWidget(j, 0))->currentText() +
		       tr(", ") +
		       tablewidget->item(j, 1)->text().trimmed() +
		       QString(tr(").")),
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
		  {
		    query.prepare("INSERT INTO minimum_days "
				  "(days, type) VALUES "
				  "(?, 'Book')");
		    query.bindValue(0,
				    tablewidget->item(j, 1)->text().trimmed());
		  }
		else if(j == 1)
		  {
		    query.prepare("INSERT INTO minimum_days "
				  "(days, type) VALUES "
				  "(?, 'DVD')");
		    query.bindValue(0,
				    tablewidget->item(j, 1)->text().trimmed());
		  }
		else if(j == 2)
		  {
		    query.prepare("INSERT INTO minimum_days "
				  "(days, type) VALUES "
				  "(?, 'Journal')");
		    query.bindValue(0,
				    tablewidget->item(j, 1)->text().trimmed());
		  }
		else if(j == 3)
		  {
		    query.prepare("INSERT INTO minimum_days "
				  "(days, type) VALUES "
				  "(?, 'Magazine')");
		    query.bindValue(0,
				    tablewidget->item(j, 1)->text().trimmed());
		  }
		else if(j == 4)
		  {
		    query.prepare("INSERT INTO minimum_days "
				  "(days, type) VALUES "
				  "(?, 'CD')");
		    query.bindValue(0,
				    tablewidget->item(j, 1)->text().trimmed());
		  }
		else if(j == 5)
		  {
		    query.prepare("INSERT INTO minimum_days "
				  "(days, type) VALUES "
				  "(?, 'Video Game')");
		    query.bindValue(0,
				    tablewidget->item(j, 1)->text().trimmed());
		  }

		if(!query.exec())
		  {
		    qmain->addError
		      (QString(tr("Database Error")),
		       QString(tr("Unable to create the minimum day (")) +
		       tablewidget->item(j, 0)->text() +
		       tr(", ") +
		       tablewidget->item(j, 1)->text().trimmed() +
		       QString(tr(").")),
		       query.lastError().text(), __FILE__, __LINE__);
		    goto db_rollback;
		  }
	      }
	}

      if(!qmain->getDB().commit())
	qmain->addError
	  (QString(tr("Database Error")),
	   QString(tr("Unable to commit the current database "
		      "transaction.")),
	   qmain->getDB().lastError().text(), __FILE__,
	   __LINE__);
      else
	continue;

    db_rollback:

      error = true;

      if(!qmain->getDB().rollback())
	qmain->addError(QString(tr("Database Error")),
			QString(tr("Rollback failure.")),
			qmain->getDB().lastError().text(),
			__FILE__, __LINE__);
    }

  qapp->restoreOverrideCursor();

  if(error)
    QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			  tr("An error occurred while attempting to save "
			     "the database enumerations."));
  else
    populateWidgets();
}

/*
** -- changeEvent() --
*/

void dbenumerations::changeEvent(QEvent *event)
{
  if(event)
    switch(event->type())
      {
      case QEvent::LanguageChange:
	{
	  ui.retranslateUi(this);
	  break;
	}
      default:
	break;
      }

  QMainWindow::changeEvent(event);
}
