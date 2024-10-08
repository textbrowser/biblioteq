/*
** Copyright (c) 2006 - present, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from BiblioteQ without specific prior written permission.
**
** BIBLIOTEQ IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** BIBLIOTEQ, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <QScrollBar>
#include <QSettings>

#include "biblioteq.h"
#include "biblioteq_dbenumerations.h"

biblioteq_dbenumerations::biblioteq_dbenumerations(biblioteq *parent):
  QMainWindow(parent)
{
  m_ui.setupUi(this);
  qmain = parent;
  connect(m_ui.addBookBinding,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(m_ui.addBookCondition,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(m_ui.addBookOriginality,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(m_ui.addBookTargetAudience,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(m_ui.addCdFormat,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(m_ui.addDvdAspectRatio,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(m_ui.addDvdRating,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(m_ui.addDvdRegion,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(m_ui.addGreyLiteratureType,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(m_ui.addLanguage,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(m_ui.addLocation,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(m_ui.addMonetaryUnit,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(m_ui.addVideoGamePlatform,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(m_ui.addVideoGameRating,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(m_ui.cancelButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
  connect(m_ui.reloadButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotReload(void)));
  connect(m_ui.removeBookBinding,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(m_ui.removeBookCondition,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(m_ui.removeBookOriginality,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(m_ui.removeBookTargetAudience,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(m_ui.removeCdFormat,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(m_ui.removeDvdAspectRatio,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(m_ui.removeDvdRating,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(m_ui.removeDvdRegion,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(m_ui.removeGreyLiteratureType,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(m_ui.removeLanguage,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(m_ui.removeLocation,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(m_ui.removeMonetaryUnit,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(m_ui.removeVideoGamePlatform,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(m_ui.removeVideoGameRating,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemove(void)));
  connect(m_ui.saveButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSave(void)));
  connect(m_ui.section,
	  SIGNAL(activated(int)),
	  this,
	  SLOT(slotScrollToSection(int)));

  if(qmain)
    connect(qmain,
	    SIGNAL(fontChanged(const QFont &)),
	    this,
	    SLOT(setGlobalFonts(const QFont &)));

  m_ui.locationsTable->setItemDelegateForColumn
    (0, new biblioteq_dbenumerations_item_delegate(this));
  m_ui.locationsTable->verticalHeader()->setSectionResizeMode
    (QHeaderView::Fixed);
  m_ui.minimumDaysTable->verticalHeader()->setSectionResizeMode
    (QHeaderView::Fixed);

  for(int i = 0; i < m_ui.minimumDaysTable->rowCount(); i++)
    {
      auto lineEdit = new QLineEdit(this);

      lineEdit->setToolTip("[1, 1000]");
      lineEdit->setValidator(new QIntValidator(1, 1000, this));
      m_ui.minimumDaysTable->setCellWidget
	(i, static_cast<int> (MinimumDaysTable::Days), lineEdit);
    }

  prepareIcons();
  setAttribute(Qt::WA_DeleteOnClose, true);
}

biblioteq_dbenumerations::~biblioteq_dbenumerations()
{
  isVisible() ?
    QSettings().setValue("dbenumerations_geometry", saveGeometry()) : (void) 0;
}

void biblioteq_dbenumerations::changeEvent(QEvent *event)
{
  if(event)
    switch(event->type())
      {
      case QEvent::LanguageChange:
	{
	  m_ui.retranslateUi(this);
	  break;
	}
      default:
	break;
      }

  QMainWindow::changeEvent(event);
}

void biblioteq_dbenumerations::clear(void)
{
  m_listData.clear();
  m_tableData.clear();
  m_ui.bbt_favorite->clear();
  m_ui.bc_favorite->clear();
  m_ui.bo_favorite->clear();
  m_ui.bta_favorite->clear();
  m_ui.cf_favorite->clear();
  m_ui.dar_favorite->clear();
  m_ui.dr_favorite->clear();
  m_ui.drs_favorite->clear();
  m_ui.gldt_favorite->clear();
  m_ui.language_favorite->clear();
  m_ui.locationsTable->clearContents();
  m_ui.locationsTable->setRowCount(0);
  m_ui.minimumDaysTable->clearSelection();
  m_ui.monetary_units_favorite->clear();
  m_ui.vgp_favorite->clear();
  m_ui.vgr_favorite->clear();

  foreach(auto listWidget, findChildren<QListWidget *> ())
    listWidget->clear();
}

void biblioteq_dbenumerations::closeEvent(QCloseEvent *event)
{
  QHash<QWidget *, QMap<QString, QString> > tableData;
  QHash<QWidget *, QStringList> listData;

  saveData(listData, tableData);

  if(listData != m_listData || m_tableData != tableData)
    {
      if(QMessageBox::
	 question(this,
		  tr("BiblioteQ: Question"),
		  tr("Your changes have not been saved. Continue?"),
		  QMessageBox::No | QMessageBox::Yes,
		  QMessageBox::No) == QMessageBox::No)
	{
	  QApplication::processEvents();

	  if(event)
	    event->ignore();

	  return;
	}

      QApplication::processEvents();
    }

  isVisible() ?
    QSettings().setValue("dbenumerations_geometry", saveGeometry()) : (void) 0;
  QMainWindow::closeEvent(event);
}

void biblioteq_dbenumerations::populateWidgets(void)
{
  if(!qmain)
    return;

  clear();

  QSettings settings;
  QString errorstr("");
  QStringList list;
  QStringList tables;

  tables << "book_binding_types"
	 << "book_conditions"
	 << "book_originality"
	 << "book_target_audiences"
	 << "cd_formats"
	 << "dvd_aspect_ratios"
	 << "dvd_ratings"
	 << "dvd_regions"
	 << "grey_literature_types"
	 << "languages"
	 << "locations"
	 << "minimum_days"
	 << "monetary_units"
	 << "videogame_platforms"
	 << "videogame_ratings";

  for(int i = 0; i < tables.size(); i++)
    {
      QComboBox *comboBox = nullptr;
      QList<QPair<QString, QString> > pairList;
      QListWidget *listWidget = nullptr;
      QTableWidget *tableWidget = nullptr;
      auto const str(tables.at(i));

      QApplication::setOverrideCursor(Qt::WaitCursor);

      if(str == "book_binding_types")
	{
	  comboBox = m_ui.bbt_favorite;
	  list = biblioteq_misc_functions::getBookBindingTypes
	    (qmain->getDB(), errorstr);
	  listWidget = m_ui.bookBindingsList;
	}
      else if(str == "book_conditions")
	{
	  comboBox = m_ui.bc_favorite;
	  list = biblioteq_misc_functions::getBookConditions
	    (qmain->getDB(), errorstr);
	  listWidget = m_ui.bookConditionsList;
	}
      else if(str == "book_originality")
	{
	  comboBox = m_ui.bo_favorite;
	  list = biblioteq_misc_functions::getBookOriginality
	    (qmain->getDB(), errorstr);
	  listWidget = m_ui.bookOriginalityList;
	}
      else if(str == "book_target_audiences")
	{
	  comboBox = m_ui.bta_favorite;
	  list = biblioteq_misc_functions::getBookTargetAudiences
	    (qmain->getDB(), errorstr);
	  listWidget = m_ui.bookTargetAudiences;
	}
      else if(str == "cd_formats")
	{
	  comboBox = m_ui.cf_favorite;
	  list = biblioteq_misc_functions::getCDFormats
	    (qmain->getDB(), errorstr);
	  listWidget = m_ui.cdFormatsList;
	}
      else if(str == "dvd_aspect_ratios")
	{
	  comboBox = m_ui.dar_favorite;
	  list = biblioteq_misc_functions::getDVDAspectRatios
	    (qmain->getDB(), errorstr);
	  listWidget = m_ui.dvdAspectRatiosList;
	}
      else if(str == "dvd_ratings")
	{
	  comboBox = m_ui.dr_favorite;
	  list = biblioteq_misc_functions::getDVDRatings
	    (qmain->getDB(), errorstr);
	  listWidget = m_ui.dvdRatingsList;
	}
      else if(str == "dvd_regions")
	{
	  comboBox = m_ui.drs_favorite;
	  list = biblioteq_misc_functions::getDVDRegions
	    (qmain->getDB(), errorstr);
	  listWidget = m_ui.dvdRegionsList;
	}
      else if(str == "grey_literature_types")
	{
	  comboBox = m_ui.gldt_favorite;
	  list = biblioteq_misc_functions::getGreyLiteratureTypes
	    (qmain->getDB(), errorstr);
	  listWidget = m_ui.greyLiteratureTypes;
	}
      else if(str == "languages")
	{
	  comboBox = m_ui.language_favorite;
	  list = biblioteq_misc_functions::getLanguages
	    (qmain->getDB(), errorstr);
	  listWidget = m_ui.languagesList;
	}
      else if(str == "locations")
	{
	  pairList = biblioteq_misc_functions::getLocations
	    (qmain->getDB(), errorstr);
	  tableWidget = m_ui.locationsTable;
	}
      else if(str == "minimum_days")
	{
	  list = biblioteq_misc_functions::getMinimumDays
	    (qmain->getDB(), errorstr);
	  tableWidget = m_ui.minimumDaysTable;
	}
      else if(str == "monetary_units")
	{
	  comboBox = m_ui.monetary_units_favorite;
	  list = biblioteq_misc_functions::getMonetaryUnits
	    (qmain->getDB(), errorstr);
	  listWidget = m_ui.monetaryUnitsList;
	}
      else if(str == "videogame_platforms")
	{
	  comboBox = m_ui.vgp_favorite;
	  list = biblioteq_misc_functions::getVideoGamePlatforms
	    (qmain->getDB(), errorstr);
	  listWidget = m_ui.videoGamePlatformsList;
	}
      else if(str == "videogame_ratings")
	{
	  comboBox = m_ui.vgr_favorite;
	  list = biblioteq_misc_functions::getVideoGameRatings
	    (qmain->getDB(), errorstr);
	  listWidget = m_ui.videoGameRatingsList;
	}

      QApplication::restoreOverrideCursor();

      if(!errorstr.isEmpty())
	qmain->addError
	  (tr("Database Error"),
	   tr("Unable to retrieve the contents of ") + tables.at(i) + tr("."),
	   errorstr,
	   __FILE__,
	   __LINE__);
      else if(listWidget)
	{
	  for(int i = 0; i < list.size(); i++)
	    {
	      QListWidgetItem *item = nullptr;

	      item = new QListWidgetItem(list.at(i));
	      item->setFlags
		(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	      listWidget->addItem(item);
	    }

	  if(comboBox)
	    {
	      comboBox->addItems(list);
	      comboBox->insertItem(0, "");
	      comboBox->setCurrentIndex
		(comboBox->
		 findText(settings.value(QString("%1_favorite").arg(str)).
			  toString()));

	      if(comboBox->currentIndex() < 0)
		{
		  comboBox->setCurrentIndex(0);
		  settings.remove(QString("%1_favorite").arg(str));
		}
	    }
	}
      else if(m_ui.locationsTable == tableWidget && tableWidget)
	{
	  m_ui.locationsTable->setRowCount(pairList.size());

	  for(int j = 0; j < pairList.size(); j++)
	    {
	      auto item1 = new QTableWidgetItem(pairList.at(j).first);
	      auto item2 = new QTableWidgetItem(pairList.at(j).second);

	      item1->setFlags
		(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	      item2->setFlags
		(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	      m_ui.locationsTable->setItem
		(j, static_cast<int> (LocationsTable::Location), item2);
	      m_ui.locationsTable->setItem
		(j, static_cast<int> (LocationsTable::Type), item1);
	    }

	  m_ui.locationsTable->resizeColumnToContents(0);
	  m_ui.locationsTable->resizeRowsToContents();
	}
      else if(tableWidget == m_ui.minimumDaysTable && tableWidget)
	{
	  for(int j = 0; j < list.size(); j++)
	    {
	      auto lineEdit = qobject_cast<QLineEdit *>
		(tableWidget->
		 cellWidget(j, static_cast<int> (MinimumDaysTable::Days)));

	      if(lineEdit)
		lineEdit->setText
		  (QString::number(qBound(1, list.at(j).toInt(), 1000)));
	    }

	  m_ui.minimumDaysTable->resizeColumnToContents(0);
	}
    }

  m_listData.clear();
  m_tableData.clear();
  saveData(m_listData, m_tableData);
  m_ui.bookBindingsList->setFocus();
}

void biblioteq_dbenumerations::prepareIcons(void)
{
  QSettings setting;
  auto const index = setting.value
    ("otheroptions/display_icon_set_index", 0).toInt();

  if(index == 1)
    {
      // System.

      foreach(auto toolButton, findChildren<QToolButton *> ())
	if(toolButton->objectName().startsWith("add"))
	  toolButton->setIcon
	    (QIcon::fromTheme("list-add", QIcon(":/16x16/add.png")));
	else if(toolButton->objectName().startsWith("remove"))
	  toolButton->setIcon
	    (QIcon::fromTheme("list-remove", QIcon(":/16x16/eraser.png")));

      m_ui.cancelButton->setIcon
	(QIcon::fromTheme("window-close", QIcon(":/32x32/cancel.png")));
      m_ui.reloadButton->setIcon
	(QIcon::fromTheme("view-refresh", QIcon(":/32x32/reload.png")));
      m_ui.saveButton->setIcon
	(QIcon::fromTheme("document-save", QIcon(":/32x32/save.png")));
    }
  else
    {
      // Faenza.

      foreach(auto toolButton, findChildren<QToolButton *> ())
	if(toolButton->objectName().startsWith("add"))
	  toolButton->setIcon(QIcon(":/16x16/add.png"));
	else if(toolButton->objectName().startsWith("remove"))
	  toolButton->setIcon(QIcon(":/16x16/eraser.png"));

      m_ui.cancelButton->setIcon(QIcon(":/32x32/cancel.png"));
      m_ui.reloadButton->setIcon(QIcon(":/32x32/reload.png"));
      m_ui.saveButton->setIcon(QIcon(":/32x32/save.png"));
    }
}

void biblioteq_dbenumerations::saveData
(QHash<QWidget *, QStringList> &listData,
 QHash<QWidget *, QMap<QString, QString> > &tableData)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  foreach(auto widget, findChildren<QListWidget *> ())
    {
      QStringList list;

      for(int i = 0; i < widget->count(); i++)
	{
	  auto item = widget->item(i);

	  if(item)
	    list << item->text();
	}

      listData[widget] = list;
    }

  foreach(auto table, findChildren<QTableWidget *> ())
    {
      QMap<QString, QString> map;

      for(int i = 0; i < table->rowCount(); i++)
	{
	  QString text("");
	  auto item = table->item(i, 1);

	  if(!item)
	    continue;
	  else
	    {
	      auto widget = table->cellWidget(i, 0);

	      if(widget)
		{
		  auto comboBox = widget->findChild<QComboBox *> ();

		  if(comboBox)
		    text = comboBox->currentText();
		  else
		    continue;
		}
	      else if(table->item(i, 0))
		text = table->item(i, 0)->text();
	      else
		continue;
	    }

	  map[text] = item->text();
	}

      tableData[table] = map;
    }

  QApplication::restoreOverrideCursor();
}

void biblioteq_dbenumerations::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(auto widget, findChildren<QWidget *> ())
    {
      auto f(font);

      f.setBold(widget->font().bold());
      widget->setFont(f);
      widget->update();
    }

  m_ui.locationsTable->resizeRowsToContents();
  m_ui.minimumDaysTable->resizeRowsToContents();
  update();
}

void biblioteq_dbenumerations::setPage(const Page page)
{
  if(page == Page::ReservationMinimumDays)
    {
      m_ui.section->setCurrentIndex
	(m_ui.section->findText(tr("Reservations Minimum Days")));
      slotScrollToSection(m_ui.section->currentIndex());
    }
}

void biblioteq_dbenumerations::show(QMainWindow *parent, const bool populate)
{
  restoreGeometry(QSettings().value("dbenumerations_geometry").toByteArray());

  auto const wasVisible = isVisible();

#ifdef Q_OS_ANDROID
  Q_UNUSED(parent);
  showMaximized();
#else
  biblioteq_misc_functions::center(this, parent, false);
  showNormal();
#endif
  activateWindow();
  raise();

  if(populate)
    populateWidgets();
  else
    {
      if(!wasVisible)
	{
	  m_listData.clear();
	  m_tableData.clear();
	  saveData(m_listData, m_tableData);
	}

      m_ui.bookBindingsList->setFocus();
    }
}

void biblioteq_dbenumerations::slotAdd(void)
{
  QListWidget *list = nullptr;
  QListWidgetItem *listItem = nullptr;
  auto toolButton = qobject_cast<QToolButton *> (sender());

  if(m_ui.addBookBinding == toolButton)
    {
      list = m_ui.bookBindingsList;
      listItem = new QListWidgetItem(tr("Book Binding"));
    }
  else if(m_ui.addBookCondition == toolButton)
    {
      list = m_ui.bookConditionsList;
      listItem = new QListWidgetItem(tr("Book Condition"));
    }
  else if(m_ui.addBookOriginality == toolButton)
    {
      list = m_ui.bookOriginalityList;
      listItem = new QListWidgetItem(tr("Book Originality"));
    }
  else if(m_ui.addBookTargetAudience == toolButton)
    {
      list = m_ui.bookTargetAudiences;
      listItem = new QListWidgetItem(tr("Book Target Audience"));
    }
  else if(m_ui.addCdFormat == toolButton)
    {
      list = m_ui.cdFormatsList;
      listItem = new QListWidgetItem(tr("CD Format"));
    }
  else if(m_ui.addDvdAspectRatio == toolButton)
    {
      list = m_ui.dvdAspectRatiosList;
      listItem = new QListWidgetItem(tr("DVD Aspect Ratio"));
    }
  else if(m_ui.addDvdRating == toolButton)
    {
      list = m_ui.dvdRatingsList;
      listItem = new QListWidgetItem(tr("DVD Rating"));
    }
  else if(m_ui.addDvdRegion == toolButton)
    {
      list = m_ui.dvdRegionsList;
      listItem = new QListWidgetItem(tr("DVD Region"));
    }
  else if(m_ui.addGreyLiteratureType == toolButton)
    {
      list = m_ui.greyLiteratureTypes;
      listItem = new QListWidgetItem(tr("Document Type"));
    }
  else if(m_ui.addLanguage == toolButton)
    {
      list = m_ui.languagesList;
      listItem = new QListWidgetItem(tr("Language"));
    }
  else if(m_ui.addLocation == toolButton)
    {
      auto item1 = new QTableWidgetItem("Book");
      auto item2 = new QTableWidgetItem();

      item1->setFlags
	(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      item2->setFlags
	(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.locationsTable->setRowCount(m_ui.locationsTable->rowCount() + 1);
      m_ui.locationsTable->setItem
	(m_ui.locationsTable->rowCount() - 1,
	 static_cast<int> (LocationsTable::Location),
	 item2);
      m_ui.locationsTable->setItem
	(m_ui.locationsTable->rowCount() - 1,
	 static_cast<int> (LocationsTable::Type),
	 item1);
      m_ui.locationsTable->resizeColumnToContents(0);
      m_ui.locationsTable->resizeRowsToContents();
      m_ui.locationsTable->scrollToBottom();
    }
  else if(m_ui.addMonetaryUnit == toolButton)
    {
      list = m_ui.monetaryUnitsList;
      listItem = new QListWidgetItem(tr("Monetary Unit"));
    }
  else if(m_ui.addVideoGamePlatform == toolButton)
    {
      list = m_ui.videoGamePlatformsList;
      listItem = new QListWidgetItem(tr("Video Game Platform"));
    }
  else if(m_ui.addVideoGameRating == toolButton)
    {
      list = m_ui.videoGameRatingsList;
      listItem = new QListWidgetItem(tr("Video Game Rating"));
    }

  if(list && listItem)
    {
      listItem->setFlags
	(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      list->addItem(listItem);
      list->setCurrentItem(listItem);
      list->editItem(listItem);
      list->scrollToBottom();
    }
  else
    delete listItem;
}

void biblioteq_dbenumerations::slotClose(void)
{
  close();
}

void biblioteq_dbenumerations::slotReload(void)
{
  QHash<QWidget *, QMap<QString, QString> > tableData;
  QHash<QWidget *, QStringList> listData;

  saveData(listData, tableData);

  if(listData != m_listData || m_tableData != tableData)
    {
      if(QMessageBox::
	 question(this,
		  tr("BiblioteQ: Question"),
		  tr("Your changes have not been saved. Continue?"),
		  QMessageBox::No | QMessageBox::Yes,
		  QMessageBox::No) == QMessageBox::No)
	{
	  QApplication::processEvents();
	  return;
	}

      QApplication::processEvents();
    }

  populateWidgets();
}

void biblioteq_dbenumerations::slotRemove(void)
{
  QListWidget *list = nullptr;
  auto toolButton = qobject_cast<QToolButton *> (sender());

  if(m_ui.removeBookBinding == toolButton)
    list = m_ui.bookBindingsList;
  else if(m_ui.removeBookCondition == toolButton)
    list = m_ui.bookConditionsList;
  else if(m_ui.removeBookOriginality == toolButton)
    list = m_ui.bookOriginalityList;
  else if(m_ui.removeBookTargetAudience == toolButton)
    list = m_ui.bookTargetAudiences;
  else if(m_ui.removeCdFormat == toolButton)
    list = m_ui.cdFormatsList;
  else if(m_ui.removeDvdAspectRatio == toolButton)
    list = m_ui.dvdAspectRatiosList;
  else if(m_ui.removeDvdRating == toolButton)
    list = m_ui.dvdRatingsList;
  else if(m_ui.removeDvdRegion == toolButton)
    list = m_ui.dvdRegionsList;
  else if(m_ui.removeGreyLiteratureType == toolButton)
    list = m_ui.greyLiteratureTypes;
  else if(m_ui.removeLanguage == toolButton)
    list = m_ui.languagesList;
  else if(m_ui.removeLocation == toolButton)
    m_ui.locationsTable->removeRow(m_ui.locationsTable->currentRow());
  else if(m_ui.removeMonetaryUnit == toolButton)
    list = m_ui.monetaryUnitsList;
  else if(m_ui.removeVideoGamePlatform == toolButton)
    list = m_ui.videoGamePlatformsList;
  else if(m_ui.removeVideoGameRating == toolButton)
    list = m_ui.videoGameRatingsList;

  if(list && list->item(list->currentRow()))
    delete list->takeItem(list->currentRow());
}

void biblioteq_dbenumerations::slotSave(void)
{
  if(!qmain)
    return;

  QComboBox *comboBox = nullptr;
  QListWidget *listWidget = nullptr;
  QSettings settings;
  QSqlQuery query(qmain->getDB());
  QString querystr("");
  QStringList tables;
  QTableWidget *tableWidget = nullptr;
  auto error = false;

  QApplication::setOverrideCursor(Qt::WaitCursor);
  tables << "book_binding_types"
	 << "book_conditions"
	 << "book_originality"
	 << "book_target_audiences"
	 << "cd_formats"
	 << "dvd_aspect_ratios"
	 << "dvd_ratings"
	 << "dvd_regions"
	 << "grey_literature_types"
	 << "languages"
	 << "locations"
	 << "minimum_days"
	 << "monetary_units"
	 << "videogame_platforms"
	 << "videogame_ratings";

  for(int i = 0; i < tables.size(); i++)
    {
      listWidget = nullptr;
      querystr = QString("DELETE FROM %1").arg(tables.at(i));
      tableWidget = nullptr;

      if(!qmain->getDB().transaction())
	{
	  error = true;
	  qmain->addError
	    (tr("Database Error"),
	     tr("Unable to create a database transaction."),
	     qmain->getDB().lastError().text(),
	     __FILE__,
	     __LINE__);
	  continue;
	}

      if(!query.exec(querystr))
	{
	  qmain->addError
	    (tr("Database Error"),
	     tr("An error occurred while attempting to "
		"remove entries from the %1 table.").arg(tables.at(i)),
	     query.lastError().text(),
	     __FILE__,
	     __LINE__);
	  goto db_rollback;
	}

      if(tables.at(i) == "book_binding_types")
	{
	  comboBox = m_ui.bbt_favorite;
	  listWidget = m_ui.bookBindingsList;
	}
      else if(tables.at(i) == "book_conditions")
	{
	  comboBox = m_ui.bc_favorite;
	  listWidget = m_ui.bookConditionsList;
	}
      else if(tables.at(i) == "book_originality")
	{
	  comboBox = m_ui.bo_favorite;
	  listWidget = m_ui.bookOriginalityList;
	}
      else if(tables.at(i) == "book_target_audiences")
	{
	  comboBox = m_ui.bta_favorite;
	  listWidget = m_ui.bookTargetAudiences;
	}
      else if(tables.at(i) == "cd_formats")
	{
	  comboBox = m_ui.cf_favorite;
	  listWidget = m_ui.cdFormatsList;
	}
      else if(tables.at(i) == "dvd_aspect_ratios")
	{
	  comboBox = m_ui.dar_favorite;
	  listWidget = m_ui.dvdAspectRatiosList;
	}
      else if(tables.at(i) == "dvd_ratings")
	{
	  comboBox = m_ui.dr_favorite;
	  listWidget = m_ui.dvdRatingsList;
	}
      else if(tables.at(i) == "dvd_regions")
	{
	  comboBox = m_ui.drs_favorite;
	  listWidget = m_ui.dvdRegionsList;
	}
      else if(tables.at(i) == "grey_literature_types")
	{
	  comboBox = m_ui.gldt_favorite;
	  listWidget = m_ui.greyLiteratureTypes;
	}
      else if(tables.at(i) == "languages")
	{
	  comboBox = m_ui.language_favorite;
	  listWidget = m_ui.languagesList;
	}
      else if(tables.at(i) == "locations")
	tableWidget = m_ui.locationsTable;
      else if(tables.at(i) == "minimum_days")
	tableWidget = m_ui.minimumDaysTable;
      else if(tables.at(i) == "monetary_units")
	{
	  comboBox = m_ui.monetary_units_favorite;
	  listWidget = m_ui.monetaryUnitsList;
	}
      else if(tables.at(i) == "videogame_platforms")
	{
	  comboBox = m_ui.vgp_favorite;
	  listWidget = m_ui.videoGamePlatformsList;
	}
      else if(tables.at(i) == "videogame_ratings")
	{
	  comboBox = m_ui.vgr_favorite;
	  listWidget = m_ui.videoGameRatingsList;
	}

      if(listWidget)
	{
	  for(int j = 0; j < listWidget->count(); j++)
	    if(listWidget->item(j))
	      {
		query.prepare
		  (QString("INSERT INTO %1 VALUES (?)").arg(tables.at(i)));
		query.bindValue(0, listWidget->item(j)->text().trimmed());

		if(!query.exec())
		  {
		    qmain->addError
		      (tr("Database Error"),
		       tr("Unable to create an entry in ") +
		       tables.at(i) + tr("for ") +
		       listWidget->item(j)->text().trimmed() +
		       tr("."),
		       query.lastError().text(),
		       __FILE__,
		       __LINE__);
		    goto db_rollback;
		  }
	      }

	  if(comboBox)
	    settings.setValue
	      (QString("%1_favorite").arg(tables.at(i)),
	       comboBox->currentText());
	}
      else if(m_ui.locationsTable == tableWidget && tableWidget)
	{
	  for(int j = 0; j < tableWidget->rowCount(); j++)
	    if(tableWidget->item(j, 0) && tableWidget->item(j, 1))
	      {
		auto const locationIndex = static_cast<int>
		  (LocationsTable::Location);
		auto const typeIndex = static_cast<int>(LocationsTable::Type);

		query.prepare
		  ("INSERT INTO locations (location, type) VALUES (?, ?)");
		query.addBindValue
		  (tableWidget->item(j, locationIndex)->text().trimmed());
		query.addBindValue
		  (tableWidget->item(j, typeIndex)->text().trimmed());

		if(!query.exec())
		  {
		    qmain->addError
		      (tr("Database Error"),
		       tr("Unable to create the location (") +
		       tableWidget->item(j, typeIndex)->text().trimmed() +
		       tr(", ") +
		       tableWidget->item(j, locationIndex)->text().trimmed() +
		       tr(")."),
		       query.lastError().text(),
		       __FILE__,
		       __LINE__);
		    goto db_rollback;
		  }
	      }
	}
      else if(m_ui.minimumDaysTable == tableWidget && tableWidget)
	{
	  for(int j = 0; j < tableWidget->rowCount(); j++)
	    {
	      auto item = tableWidget->item
		(j, static_cast<int> (MinimumDaysTable::Type));
	      auto lineEdit = qobject_cast<QLineEdit *>
		(tableWidget->
		 cellWidget(j, static_cast<int> (MinimumDaysTable::Days)));

	      if(item && lineEdit)
		{
		  query.prepare("INSERT INTO minimum_days "
				"(days, type) VALUES "
				"(?, ?)");
		  query.addBindValue(lineEdit->text().toInt());
		  query.addBindValue(item->text());

		  if(!query.exec())
		    {
		      if(tableWidget->
			 item(j, static_cast<int> (MinimumDaysTable::Type)))
			qmain->addError
			  (tr("Database Error"),
			   tr("Unable to create the minimum day (") +
			   tableWidget->
			   item(j, static_cast<int> (MinimumDaysTable::Type))->
			   text() +
			   tr(", ") +
			   lineEdit->text() +
			   tr(")."),
			   query.lastError().text(),
			   __FILE__,
			   __LINE__);

		      goto db_rollback;
		    }
		}
	    }
	}

      if(!qmain->getDB().commit())
	qmain->addError
	  (tr("Database Error"),
	   tr("Unable to commit the current database transaction."),
	   qmain->getDB().lastError().text(),
	   __FILE__,
	   __LINE__);
      else
	continue;

    db_rollback:

      error = true;

      if(!qmain->getDB().rollback())
	qmain->addError(tr("Database Error"),
			tr("Rollback failure."),
			qmain->getDB().lastError().text(),
			__FILE__,
			__LINE__);
    }

  QApplication::restoreOverrideCursor();

  if(error)
    {
      QMessageBox::critical(this,
			    tr("BiblioteQ: Database Error"),
			    tr("An error occurred while attempting to save "
			       "the database enumerations."));
      QApplication::processEvents();
    }
  else
    {
      populateWidgets();
      emit committed();
    }
}

void biblioteq_dbenumerations::slotScrollToSection(int index)
{
  QList<QWidget *> widgets;

  widgets << m_ui.bookBindingsListLabel
	  << m_ui.bookConditionsListLabel
	  << m_ui.bookOriginalityListLabel
	  << m_ui.bookTargetAudiencesListLabel
	  << m_ui.cdFormatsListLabel
	  << m_ui.dvdAspectRatiosListLabel
	  << m_ui.dvdRatingsListLabel
	  << m_ui.dvdRegionsListLabel
	  << m_ui.greyLiteratureTypesLabel
	  << m_ui.languagesListLabel
	  << m_ui.locationsTableLabel
	  << m_ui.monetaryUnitsListLabel
    	  << m_ui.minimumDaysTableLabel
	  << m_ui.videoGamePlatformsListLabel
	  << m_ui.videoGameRatingsListLabel;
  m_ui.scrollArea->verticalScrollBar()->setValue(widgets.at(index)->pos().y());
}
