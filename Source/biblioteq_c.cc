/*
** -- Qt Includes --
*/

#include <QSettings>
#include <QSqlField>
#include <QSqlRecord>

/*
** -- Local Includes --
*/

#include "biblioteq.h"
#include "biblioteq_book.h"
#include "biblioteq_grey_literature.h"
#include "biblioteq_otheroptions.h"
#include "biblioteq_pdfreader.h"

void biblioteq::slotShowOtherOptions(void)
{
  biblioteq_misc_functions::center(m_otheroptions, this);
  m_otheroptions->showNormal();
  m_otheroptions->activateWindow();
  m_otheroptions->raise();
}

void biblioteq::slotPreviewCanvasBackgroundColor(const QColor &color)
{
  ui.graphicsView->scene()->setBackgroundBrush(color);
}

void biblioteq::slotMainWindowCanvasBackgroundColorChanged(const QColor &color)
{
  QSettings settings;

  if(color.isValid())
    {
      settings.setValue("mainwindow_canvas_background_color", color.name());
      ui.graphicsView->scene()->setBackgroundBrush(color);
    }
  else
    {
      QColor color(settings.value("mainwindow_canvas_background_color").
		   toString().trimmed());

      if(!color.isValid())
	color = Qt::white;

      ui.graphicsView->scene()->setBackgroundBrush(color);
    }
}

QString biblioteq::publicationDateFormat(const QString &itemType) const
{
  return m_otheroptions->publicationDateFormat(itemType);
}

void biblioteq::slotOtherOptionsSaved(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  foreach(QWidget *widget, QApplication::topLevelWidgets())
    if(qobject_cast<biblioteq_book *> (widget))
      qobject_cast<biblioteq_book *> (widget)->setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("books"));
    else if(qobject_cast<biblioteq_cd *> (widget))
      qobject_cast<biblioteq_cd *> (widget)->setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("musiccds"));
    else if(qobject_cast<biblioteq_dvd *> (widget))
      qobject_cast<biblioteq_dvd *> (widget)->setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("dvds"));
    else if(qobject_cast<biblioteq_grey_literature *> (widget))
      qobject_cast<biblioteq_grey_literature *> (widget)->
	setPublicationDateFormat(m_otheroptions->
				 publicationDateFormat("greyliterature"));
    else if(qobject_cast<biblioteq_journal *> (widget))
      qobject_cast<biblioteq_journal *> (widget)->setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("journals"));
    else if(qobject_cast<biblioteq_magazine *> (widget))
      qobject_cast<biblioteq_magazine *> (widget)->setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("magazines"));
    else if(qobject_cast<biblioteq_photographcollection *> (widget))
      qobject_cast<biblioteq_photographcollection *> (widget)->
	setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("photographcollections"));
    else if(qobject_cast<biblioteq_videogame *> (widget))
      qobject_cast<biblioteq_videogame *> (widget)->setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("videogames"));

  QApplication::restoreOverrideCursor();
}

void biblioteq::slotOpenPDFFile(void)
{
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  QFileDialog dialog(this);

  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter("PDF File (*.pdf)");
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowTitle(tr("BiblioteQ: Open PDF File"));

  if(dialog.exec() == QDialog::Accepted)
    {
#ifndef Q_OS_MAC
      QApplication::processEvents();
#endif

      biblioteq_pdfreader *reader = new(std::nothrow) biblioteq_pdfreader(0);

      if(reader)
	{
	  QApplication::setOverrideCursor(Qt::WaitCursor);
	  reader->load(dialog.selectedFiles().value(0));
	  biblioteq_misc_functions::center(reader, this);
	  reader->show();
	  QApplication::restoreOverrideCursor();
	}
    }
#endif
}

void biblioteq::slotGeneralSearchPublicationDateEnabled(bool state)
{
  al.publication_date->setEnabled(state);

  if(!state)
    al.publication_date->setDate(QDate::fromString("2001", "yyyy"));
}

void biblioteq::slotInsertGreyLiterature(void)
{
  QString id("");
  biblioteq_grey_literature *gl = 0;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  gl = new(std::nothrow) biblioteq_grey_literature(this, id, -1);

  if(gl)
    gl->insert();
}

void biblioteq::slotGreyLiteratureSearch(void)
{
  biblioteq_grey_literature *gl = 0;

  foreach(QWidget *w, QApplication::topLevelWidgets())
    {
      biblioteq_grey_literature *g = qobject_cast
	<biblioteq_grey_literature *> (w);

      if(g && g->getID() == "search")
	{
	  gl = g;
	  break;
	}
    }

  if(!gl)
    {
      gl = new(std::nothrow) biblioteq_grey_literature(this, "search", -1);

      if(gl)
	gl->search();
    }

  if(gl)
    {
      gl->showNormal();
      gl->activateWindow();
      gl->raise();
    }
}

void biblioteq::greyLiteratureSearch
(const QString &field, const QString &value)
{
  biblioteq_grey_literature *gl =
    new(std::nothrow) biblioteq_grey_literature(this, "", -1);

  if(gl)
    {
      gl->search(field, value);
      gl->deleteLater();
    }
}

void biblioteq::slotRefreshCustomQuery(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  QSqlField field;
  QSqlRecord rec;
  QStringList list;
  QTreeWidgetItem *item1 = 0;
  QTreeWidgetItem *item2 = 0;
  int i = 0;
  int j = 0;

  cq.tables_t->clear();

  if(m_db.driverName() == "QSQLITE")
    list << "book"
	 << "book_binding_types"
	 << "book_copy_info"
	 << "cd"
	 << "cd_copy_info"
	 << "cd_formats"
	 << "cd_songs"
	 << "dvd"
	 << "dvd_aspect_ratios"
	 << "dvd_copy_info"
	 << "dvd_ratings"
	 << "dvd_regions"
	 << "grey_literature"
	 << "grey_literature_types"
	 << "item_borrower"
	 << "item_borrower_vw"
	 << "journal"
	 << "journal_copy_info"
	 << "languages"
	 << "locations"
	 << "magazine"
	 << "magazine_copy_info"
	 << "member"
	 << "member_history"
	 << "minimum_days"
	 << "monetary_units"
	 << "photograph"
	 << "photograph_collection"
	 << "videogame"
	 << "videogame_copy_info"
	 << "videogame_platforms"
	 << "videogame_ratings";
  else
    list << "admin"
	 << "book"
	 << "book_binding_types"
	 << "book_copy_info"
	 << "cd"
	 << "cd_copy_info"
	 << "cd_formats"
	 << "cd_songs"
	 << "dvd"
	 << "dvd_aspect_ratios"
	 << "dvd_copy_info"
	 << "dvd_ratings"
	 << "dvd_regions"
	 << "grey_literature"
	 << "grey_literature_types"
	 << "item_borrower"
	 << "item_borrower_vw"
	 << "item_request"
	 << "journal"
	 << "journal_copy_info"
	 << "languages"
	 << "locations"
	 << "magazine"
	 << "magazine_copy_info"
	 << "member"
	 << "member_history"
	 << "minimum_days"
	 << "monetary_units"
	 << "photograph"
	 << "photograph_collection"
	 << "videogame"
	 << "videogame_copy_info"
	 << "videogame_platforms"
	 << "videogame_ratings";

  list.sort();
  cq.tables_t->setSortingEnabled(false);
  cq.tables_t->setColumnCount(3);
  cq.tables_t->setHeaderLabels(QStringList()
			       << tr("Table Name")
			       << tr("Column")
			       << tr("Column Type")
			       << tr("NULL"));

  for(i = 0; i < list.size(); i++)
    if((item1 = new(std::nothrow) QTreeWidgetItem(cq.tables_t)) != 0)
      {
	item1->setText(0, list[i]);
	rec = m_db.record(list[i]);

	for(j = 0; j < rec.count(); j++)
	  {
	    if((item2 = new(std::nothrow) QTreeWidgetItem(item1)) == 0)
	      {
		addError(QString(tr("Memory Error")),
			 QString(tr("Unable to allocate "
				    "memory for the \"item2\" "
				    "object. "
				    "This is a serious "
				    "problem!")), QString(""),
			 __FILE__, __LINE__);
		continue;
	      }

	    field = rec.field(rec.fieldName(j));
	    item2->setText(1, rec.fieldName(j));
	    item2->setText(2, QVariant::typeToName(field.type()));

	    if(field.requiredStatus() == QSqlField::Required)
	      item2->setText(3, tr("No"));
	    else
	      item2->setText(3, "");
	  }
      }
    else
      addError(QString(tr("Memory Error")),
	       QString(tr("Unable to allocate "
			  "memory for the \"item1\" "
			  "object. "
			  "This is a serious "
			  "problem!")), QString(""),
	       __FILE__, __LINE__);

  for(i = 0; i < cq.tables_t->columnCount() - 1; i++)
    cq.tables_t->resizeColumnToContents(i);

  cq.tables_t->setSortingEnabled(true);
  cq.tables_t->sortByColumn(0, Qt::AscendingOrder);
  QApplication::restoreOverrideCursor();
}
