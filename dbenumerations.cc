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
  QStringList list;

  for(int i = 0; i < 10; i++)
    {
      QListWidget *listwidget = 0;

      if(i == 0)
	listwidget = ui.cdFormatsList;
      else if(i == 1)
	listwidget = ui.dvdAspectRatiosList;

      if(listwidget)
	listwidget->clear();

      qapp->setOverrideCursor(Qt::WaitCursor);

      if(i == 0)
	list = misc_functions::getCDFormats(qmain->getDB(),
					    errorstr);
      else if(i == 1)
	list = misc_functions::getDVDAspectRatios(qmain->getDB(),
						  errorstr);

      qapp->restoreOverrideCursor();

      if(!errorstr.isEmpty())
	qmain->addError
	  (QString(tr("Database Error")),
	   QString(tr("Unable to retrieve the cd formats.")),
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
    }
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

  for(int i = 0; i < 2; i++)
    {
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

      if(i == 0)
	querystr = "DELETE FROM cd_formats";
      else if(i == 1)
	querystr = "DELETE FROM dvd_aspect_ratios";

      if(!query.exec(querystr))
	{
	  qapp->restoreOverrideCursor();

	  if(i == 0)
	    qmain->addError(QString(tr("Database Error")),
			    QString(tr("An error occurred while attempting to "
				       "remove the cd formats.")),
			    query.lastError().text(), __FILE__, __LINE__);
	  else if(i == 1)
	    qmain->addError(QString(tr("Database Error")),
			    QString(tr("An error occurred while attempting to "
				       "remove the dvd aspect_ratios.")),
			    query.lastError().text(), __FILE__, __LINE__);

	  goto db_rollback;
	}

      QString table("");
      QString forerror("");
      QListWidget *list = 0;

      if(i == 0)
	{
	  table = "cd_formats";
	  forerror = tr("cd format");
	  list = ui.cdFormatsList;
	}
      else if(i == 1)
	{
	  table = "dvd_aspect_ratios";
	  forerror = tr("dvd aspect ratio");
	  list = ui.dvdAspectRatiosList;
	}

      if(list)
	for(int j = 0; j < list->count(); j++)
	  {
	    if(list->item(j))
	      {
		querystr = QString("INSERT INTO %1 VALUES ('%2')").arg
		  (table).arg
		  (list->item(j)->text());

		if(!query.exec(querystr))
		  {
		    qapp->restoreOverrideCursor();
		    qmain->addError
		      (QString(tr("Database Error")),
		       QString(tr("Unable to create the ")) +
		       forerror +
		       list->item(j)->text() + QString(tr(" .")),
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
