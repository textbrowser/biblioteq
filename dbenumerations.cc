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
  ui.cdFormatsList->clear();

  QString errorstr("");
  QStringList list;

  qapp->setOverrideCursor(Qt::WaitCursor);
  list = misc_functions::getCDFormats(qmain->getDB(),
				      errorstr);
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the cd formats.")),
       errorstr, __FILE__, __LINE__);
  else
    while(!list.isEmpty())
      {
	QListWidgetItem *item = 0;

	item = new(std::nothrow) QListWidgetItem(list.takeFirst());

	if(item)
	  {
	    item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled |
			   Qt::ItemIsSelectable) ;
	    ui.cdFormatsList->addItem(item);
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
  QListWidgetItem *listItem = 0;

  if(toolButton == ui.addCdFormat)
    {
      listItem = new(std::nothrow) QListWidgetItem(tr("CD Format"));

      if(listItem)
	{
	  listItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled |
			     Qt::ItemIsSelectable) ;
	  ui.cdFormatsList->addItem(listItem);
	}
    }
}

/*
** -- slotRemove() --
*/

void dbenumerations::slotRemove(void)
{
  QToolButton *toolButton = qobject_cast<QToolButton *>
    (sender());

  if(toolButton == ui.removeCdFormat)
    delete ui.cdFormatsList->takeItem(ui.cdFormatsList->currentRow());
}

/*
** -- slotSave() --
*/

void dbenumerations::slotSave(void)
{
  QString querystr("");
  QSqlQuery query(qmain->getDB());

  /*
  ** CD Formats
  */

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

  querystr = "DELETE FROM cd_formats";

  if(!query.exec(querystr))
    {
      qapp->restoreOverrideCursor();
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("An error occurred while attempting to "
				 "remove the cd formats.")),
		      query.lastError().text(), __FILE__, __LINE__);
      goto db_rollback;
    }

  for(int i = 0; i < ui.cdFormatsList->count(); i++)
    {
      if(ui.cdFormatsList->item(i))
	{
	  querystr = QString("INSERT INTO cd_formats VALUES ('%1')").arg
	    (ui.cdFormatsList->item(i)->text());

	  if(!query.exec(querystr))
	    {
	      qapp->restoreOverrideCursor();
	      qmain->addError
		(QString(tr("Database Error")),
		 QString(tr("Unable to create the cd format ")) +
		 ui.cdFormatsList->item(i)->text() + QString(tr(" .")),
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
