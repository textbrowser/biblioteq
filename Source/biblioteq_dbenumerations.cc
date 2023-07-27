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
#include "biblioteq_misc_functions.h"

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

  m_ui.locationsTable->verticalHeader()->setSectionResizeMode
    (QHeaderView::Fixed);
  m_ui.minimumDaysTable->verticalHeader()->setSectionResizeMode
    (QHeaderView::Fixed);

  for(int i = 0; i < m_ui.minimumDaysTable->rowCount(); i++)
    {
      auto lineEdit = new QLineEdit(this);

      lineEdit->setToolTip("[1, 1000]");
      lineEdit->setValidator(new QIntValidator(1, 1000, this));
      m_ui.minimumDaysTable->setCellWidget(i, 1, lineEdit);
    }

  setAttribute(Qt::WA_DeleteOnClose, true);
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
  m_ui.cdf_favorite->clear();
  m_ui.dar_favorite->clear();
  m_ui.dr_favorite->clear();
  m_ui.drs_favorite->clear();

  foreach(auto listwidget, findChildren<QListWidget *> ())
    listwidget->clear();

  while(m_ui.locationsTable->rowCount() > 0)
    m_ui.locationsTable->removeRow(0);

  m_ui.minimumDaysTable->clearSelection();
}

void biblioteq_dbenumerations::closeEvent(QCloseEvent *event)
{
  QHash<QWidget *, QMap<QString, QString> > tableData;
  QHash<QWidget *, QStringList> listData;

  saveData(listData, tableData);

  if(listData != m_listData || tableData != m_tableData)
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

  QMainWindow::closeEvent(event);
}

void biblioteq_dbenumerations::populateWidgets(void)
{
  if(!qmain)
    return;

  clear();

  QList<QPair<QString, QString> > pairList;
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
      QComboBox *combobox = nullptr;
      QListWidget *listwidget = nullptr;
      QTableWidget *tablewidget = nullptr;
      const auto &str(tables.at(i));

      QApplication::setOverrideCursor(Qt::WaitCursor);

      if(str == "book_binding_types")
	{
	  combobox = m_ui.bbt_favorite;
	  list = biblioteq_misc_functions::getBookBindingTypes
	    (qmain->getDB(), errorstr);
	  listwidget = m_ui.bookBindingsList;
	}
      else if(str == "book_conditions")
	{
	  combobox = m_ui.bc_favorite;
	  list = biblioteq_misc_functions::getBookConditions
	    (qmain->getDB(), errorstr);
	  listwidget = m_ui.bookConditionsList;
	}
      else if(str == "book_originality")
	{
	  combobox = m_ui.bo_favorite;
	  list = biblioteq_misc_functions::getBookOriginality
	    (qmain->getDB(), errorstr);
	  listwidget = m_ui.bookOriginalityList;
	}
      else if(str == "book_target_audiences")
	{
	  combobox = m_ui.bta_favorite;
	  list = biblioteq_misc_functions::getBookTargetAudiences
	    (qmain->getDB(), errorstr);
	  listwidget = m_ui.bookTargetAudiences;
	}
      else if(str == "cd_formats")
	{
	  combobox = m_ui.cdf_favorite;
	  list = biblioteq_misc_functions::getCDFormats
	    (qmain->getDB(), errorstr);
	  listwidget = m_ui.cdFormatsList;
	}
      else if(str == "dvd_aspect_ratios")
	{
	  combobox = m_ui.dar_favorite;
	  list = biblioteq_misc_functions::getDVDAspectRatios
	    (qmain->getDB(), errorstr);
	  listwidget = m_ui.dvdAspectRatiosList;
	}
      else if(str == "dvd_ratings")
	{
	  combobox = m_ui.dr_favorite;
	  list = biblioteq_misc_functions::getDVDRatings
	    (qmain->getDB(), errorstr);
	  listwidget = m_ui.dvdRatingsList;
	}
      else if(str == "dvd_regions")
	{
	  combobox = m_ui.drs_favorite;
	  list = biblioteq_misc_functions::getDVDRegions
	    (qmain->getDB(), errorstr);
	  listwidget = m_ui.dvdRegionsList;
	}
      else if(str == "grey_literature_types")
	{
	  list = biblioteq_misc_functions::getGreyLiteratureTypes
	    (qmain->getDB(), errorstr);
	  listwidget = m_ui.greyLiteratureTypes;
	}
      else if(str == "languages")
	{
	  list = biblioteq_misc_functions::getLanguages
	    (qmain->getDB(), errorstr);
	  listwidget = m_ui.languagesList;
	}
      else if(str == "locations")
	{
	  pairList = biblioteq_misc_functions::getLocations
	    (qmain->getDB(), errorstr);
	  tablewidget = m_ui.locationsTable;
	}
      else if(str == "minimum_days")
	{
	  list = biblioteq_misc_functions::getMinimumDays
	    (qmain->getDB(), errorstr);
	  tablewidget = m_ui.minimumDaysTable;
	}
      else if(str == "monetary_units")
	{
	  list = biblioteq_misc_functions::getMonetaryUnits
	    (qmain->getDB(), errorstr);
	  listwidget = m_ui.monetaryUnitsList;
	}
      else if(str == "videogame_platforms")
	{
	  list = biblioteq_misc_functions::getVideoGamePlatforms
	    (qmain->getDB(), errorstr);
	  listwidget = m_ui.videoGamePlatformsList;
	}
      else if(str == "videogame_ratings")
	{
	  list = biblioteq_misc_functions::getVideoGameRatings
	    (qmain->getDB(), errorstr);
	  listwidget = m_ui.videoGameRatingsList;
	}

      QApplication::restoreOverrideCursor();

      if(!errorstr.isEmpty())
	qmain->addError
	  (QString(tr("Database Error")),
	   QString(tr("Unable to retrieve the contents of ")) +
	   tables.at(i) + tr("."),
	   errorstr, __FILE__, __LINE__);
      else if(listwidget)
	{
	  for(int i = 0; i < list.size(); i++)
	    {
	      QListWidgetItem *item = nullptr;

	      item = new QListWidgetItem(list.at(i));
	      item->setFlags
		(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	      listwidget->addItem(item);
	    }

	  if(combobox)
	    {
	      combobox->addItems(list);
	      combobox->insertItem(0, "");
	      combobox->setCurrentIndex
		(combobox->
		 findText(settings.value(QString("%1_favorite").arg(str)).
			  toString()));

	      if(combobox->currentIndex() < 0)
		{
		  combobox->setCurrentIndex(0);
		  settings.remove(QString("%1_favorite").arg(str));
		}
	    }
	}
      else if(m_ui.locationsTable == tablewidget && tablewidget)
	{
	  m_ui.locationsTable->setRowCount(pairList.size());

	  for(int j = 0; j < pairList.size(); j++)
	    {
	      QStringList list;
	      auto comboBox = new QComboBox();
	      auto item = new QTableWidgetItem(pairList.at(j).second);
	      auto layout = new QHBoxLayout();
	      auto spacer = new QSpacerItem
		(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
	      auto widget = new QWidget();

	      layout->addWidget(comboBox);
	      layout->addSpacerItem(spacer);
	      layout->setContentsMargins(0, 0, 0, 0);
	      list << tr("Book")
		   << tr("DVD")
		   << tr("Grey Literature")
		   << tr("Journal")
		   << tr("Magazine")
		   << tr("Music CD")
		   << tr("Photograph Collection")
		   << tr("Video Game");
	      comboBox->addItems(list);

	      if(pairList.at(j).first == "Book")
		comboBox->setCurrentIndex(0);
	      else if(pairList.at(j).first == "CD")
		comboBox->setCurrentIndex(5);
	      else if(pairList.at(j).first == "DVD")
		comboBox->setCurrentIndex(1);
	      else if(pairList.at(j).first == "Grey Literature")
		comboBox->setCurrentIndex(2);
	      else if(pairList.at(j).first == "Journal")
		comboBox->setCurrentIndex(3);
	      else if(pairList.at(j).first == "Magazine")
		comboBox->setCurrentIndex(4);
	      else if(pairList.at(j).first == "Photograph Collection")
		comboBox->setCurrentIndex(6);
	      else if(pairList.at(j).first == "Video Game")
		comboBox->setCurrentIndex(7);

	      comboBox->setSizePolicy
		(QSizePolicy::Preferred, QSizePolicy::Minimum);
	      list.clear();
	      item->setFlags
		(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	      widget->setLayout(layout);
	      m_ui.locationsTable->setCellWidget(j, 0, widget);
	      m_ui.locationsTable->setItem(j, 1, item);
	    }

	  m_ui.locationsTable->resizeColumnToContents(0);
	  m_ui.locationsTable->resizeRowsToContents();
	}
      else if(tablewidget == m_ui.minimumDaysTable && tablewidget)
	{
	  for(int j = 0; j < list.size(); j++)
	    {
	      auto lineEdit = qobject_cast<QLineEdit *>
		(tablewidget->cellWidget(j, 1));

	      if(lineEdit)
		lineEdit->setText
		  (QString::number(qBound(1, list.at(j).toInt(), 1000)));
	    }

	  m_ui.minimumDaysTable->resizeColumnToContents(0);
	}

      list.clear();
      pairList.clear();
    }

  m_listData.clear();
  m_tableData.clear();
  saveData(m_listData, m_tableData);
  m_ui.bookBindingsList->setFocus();
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

void biblioteq_dbenumerations::show(QMainWindow *parent, const bool populate)
{
  auto wasVisible = isVisible();

#ifdef Q_OS_ANDROID
  Q_UNUSED(parent);
  showMaximized();
#else
  biblioteq_misc_functions::center(this, parent);
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

  if(toolButton == m_ui.addBookBinding)
    {
      list = m_ui.bookBindingsList;
      listItem = new QListWidgetItem(tr("Book Binding"));
    }
  else if(toolButton == m_ui.addBookCondition)
    {
      list = m_ui.bookConditionsList;
      listItem = new QListWidgetItem(tr("Book Condition"));
    }
  else if(toolButton == m_ui.addBookOriginality)
    {
      list = m_ui.bookOriginalityList;
      listItem = new QListWidgetItem(tr("Book Originality"));
    }
  else if(toolButton == m_ui.addBookTargetAudience)
    {
      list = m_ui.bookTargetAudiences;
      listItem = new QListWidgetItem(tr("Book Target Audience"));
    }
  else if(toolButton == m_ui.addCdFormat)
    {
      list = m_ui.cdFormatsList;
      listItem = new QListWidgetItem(tr("CD Format"));
    }
  else if(toolButton == m_ui.addDvdAspectRatio)
    {
      list = m_ui.dvdAspectRatiosList;
      listItem = new QListWidgetItem(tr("DVD Aspect Ratio"));
    }
  else if(toolButton == m_ui.addDvdRating)
    {
      list = m_ui.dvdRatingsList;
      listItem = new QListWidgetItem(tr("DVD Rating"));
    }
  else if(toolButton == m_ui.addDvdRegion)
    {
      list = m_ui.dvdRegionsList;
      listItem = new QListWidgetItem(tr("DVD Region"));
    }
  else if(toolButton == m_ui.addGreyLiteratureType)
    {
      list = m_ui.greyLiteratureTypes;
      listItem = new QListWidgetItem(tr("Document Type"));
    }
  else if(toolButton == m_ui.addLanguage)
    {
      list = m_ui.languagesList;
      listItem = new QListWidgetItem(tr("Language"));
    }
  else if(toolButton == m_ui.addLocation)
    {
      QStringList list;
      auto comboBox = new QComboBox();
      auto item = new QTableWidgetItem();
      auto layout = new QHBoxLayout();
      auto spacer = new QSpacerItem
	(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
      auto widget = new QWidget();

      layout->addWidget(comboBox);
      layout->addSpacerItem(spacer);
      layout->setContentsMargins(0, 0, 0, 0);
      list << tr("Book")
	   << tr("DVD")
	   << tr("Grey Literature")
	   << tr("Journal")
	   << tr("Magazine")
	   << tr("Music CD")
	   << tr("Photograph Collection")
	   << tr("Video Game");
      comboBox->addItems(list);
      comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
      list.clear();
      item->setFlags
	(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      widget->setLayout(layout);
      m_ui.locationsTable->setRowCount(m_ui.locationsTable->rowCount() + 1);
      m_ui.locationsTable->setCellWidget
	(m_ui.locationsTable->rowCount() - 1, 0, widget);
      m_ui.locationsTable->setItem(m_ui.locationsTable->rowCount() - 1,
				   1,
				   item);
      m_ui.locationsTable->setCurrentCell
	(m_ui.locationsTable->rowCount() - 1, 0);
      m_ui.locationsTable->resizeColumnToContents(0);
      m_ui.locationsTable->resizeRowsToContents();
    }
  else if(toolButton == m_ui.addMonetaryUnit)
    {
      list = m_ui.monetaryUnitsList;
      listItem = new QListWidgetItem(tr("Monetary Unit"));
    }
  else if(toolButton == m_ui.addVideoGamePlatform)
    {
      list = m_ui.videoGamePlatformsList;
      listItem = new QListWidgetItem(tr("Video Game Platform"));
    }
  else if(toolButton == m_ui.addVideoGameRating)
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
  QHash<QWidget *, QStringList> listData;
  QHash<QWidget *, QMap<QString, QString> > tableData;

  saveData(listData, tableData);

  if(listData != m_listData ||
     tableData != m_tableData)
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

  if(toolButton == m_ui.removeBookBinding)
    list = m_ui.bookBindingsList;
  else if(toolButton == m_ui.removeBookCondition)
    list = m_ui.bookConditionsList;
  else if(toolButton == m_ui.removeBookOriginality)
    list = m_ui.bookOriginalityList;
  else if(toolButton == m_ui.removeBookTargetAudience)
    list = m_ui.bookTargetAudiences;
  else if(toolButton == m_ui.removeCdFormat)
    list = m_ui.cdFormatsList;
  else if(toolButton == m_ui.removeDvdAspectRatio)
    list = m_ui.dvdAspectRatiosList;
  else if(toolButton == m_ui.removeDvdRating)
    list = m_ui.dvdRatingsList;
  else if(toolButton == m_ui.removeDvdRegion)
    list = m_ui.dvdRegionsList;
  else if(toolButton == m_ui.removeGreyLiteratureType)
    list = m_ui.greyLiteratureTypes;
  else if(toolButton == m_ui.removeLanguage)
    list = m_ui.languagesList;
  else if(toolButton == m_ui.removeLocation)
    m_ui.locationsTable->removeRow(m_ui.locationsTable->currentRow());
  else if(toolButton == m_ui.removeMonetaryUnit)
    list = m_ui.monetaryUnitsList;
  else if(toolButton == m_ui.removeVideoGamePlatform)
    list = m_ui.videoGamePlatformsList;
  else if(toolButton == m_ui.removeVideoGameRating)
    list = m_ui.videoGameRatingsList;

  if(list && list->item(list->currentRow()))
    delete list->takeItem(list->currentRow());
}

void biblioteq_dbenumerations::slotSave(void)
{
  if(!qmain)
    return;

  QComboBox *combobox = nullptr;
  QListWidget *listwidget = nullptr;
  QSettings settings;
  QSqlQuery query(qmain->getDB());
  QString querystr("");
  QStringList tables;
  QTableWidget *tablewidget = nullptr;
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
      listwidget = nullptr;
      querystr = QString("DELETE FROM %1").arg(tables.at(i));
      tablewidget = nullptr;

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
	  qmain->addError
	    (QString(tr("Database Error")),
	     QString(tr("An error occurred while attempting to "
			"remove entries from the %1 table.").
		     arg(tables.at(i))),
	     query.lastError().text(), __FILE__, __LINE__);
	  goto db_rollback;
	}

      if(tables.at(i) == "book_binding_types")
	{
	  combobox = m_ui.bbt_favorite;
	  listwidget = m_ui.bookBindingsList;
	}
      else if(tables.at(i) == "book_conditions")
	{
	  combobox = m_ui.bc_favorite;
	  listwidget = m_ui.bookConditionsList;
	}
      else if(tables.at(i) == "book_originality")
	{
	  combobox = m_ui.bo_favorite;
	  listwidget = m_ui.bookOriginalityList;
	}
      else if(tables.at(i) == "book_target_audiences")
	{
	  combobox = m_ui.bta_favorite;
	  listwidget = m_ui.bookTargetAudiences;
	}
      else if(tables.at(i) == "cd_formats")
	{
	  combobox = m_ui.cdf_favorite;
	  listwidget = m_ui.cdFormatsList;
	}
      else if(tables.at(i) == "dvd_aspect_ratios")
	{
	  combobox = m_ui.dar_favorite;
	  listwidget = m_ui.dvdAspectRatiosList;
	}
      else if(tables.at(i) == "dvd_ratings")
	{
	  combobox = m_ui.dr_favorite;
	  listwidget = m_ui.dvdRatingsList;
	}
      else if(tables.at(i) == "dvd_regions")
	{
	  combobox = m_ui.drs_favorite;
	  listwidget = m_ui.dvdRegionsList;
	}
      else if(tables.at(i) == "grey_literature_types")
	listwidget = m_ui.greyLiteratureTypes;
      else if(tables.at(i) == "languages")
	listwidget = m_ui.languagesList;
      else if(tables.at(i) == "locations")
	tablewidget = m_ui.locationsTable;
      else if(tables.at(i) == "minimum_days")
	tablewidget = m_ui.minimumDaysTable;
      else if(tables.at(i) == "monetary_units")
	listwidget = m_ui.monetaryUnitsList;
      else if(tables.at(i) == "videogame_platforms")
	listwidget = m_ui.videoGamePlatformsList;
      else if(tables.at(i) == "videogame_ratings")
	listwidget = m_ui.videoGameRatingsList;

      if(listwidget)
	{
	  for(int j = 0; j < listwidget->count(); j++)
	    if(listwidget->item(j))
	      {
		query.prepare
		  (QString("INSERT INTO %1 VALUES (?)").arg(tables.at(i)));
		query.bindValue(0, listwidget->item(j)->text().trimmed());

		if(!query.exec())
		  {
		    qmain->addError
		      (QString(tr("Database Error")),
		       QString(tr("Unable to create an entry in ")) +
		       tables.at(i) + tr("for ") +
		       listwidget->item(j)->text().trimmed() +
		       QString(tr(".")),
		       query.lastError().text(), __FILE__, __LINE__);
		    goto db_rollback;
		  }
	      }

	  if(combobox)
	    settings.setValue
	      (QString("%1_favorite").arg(tables.at(i)),
	       combobox->currentText());
	}
      else if(tablewidget && tablewidget == m_ui.locationsTable)
	{
	  for(int j = 0; j < tablewidget->rowCount(); j++)
	    if(tablewidget->cellWidget(j, 0) && tablewidget->item(j, 1))
	      {
		QString currentText("");
		auto comboBox = tablewidget->cellWidget(j, 0)->
		  findChild<QComboBox *> ();
		auto index = comboBox ? comboBox->currentIndex() : -1;

		currentText = comboBox ? comboBox->currentText() : "N/A";

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
				  "(?, 'Grey Literature')");
		    query.bindValue
		      (0, tablewidget->item(j, 1)->text().trimmed());
		  }
		else if(index == 3)
		  {
		    query.prepare("INSERT INTO locations "
				  "(location, type) VALUES "
				  "(?, 'Journal')");
		    query.bindValue(0,
				    tablewidget->item(j, 1)->text().trimmed());
		  }
		else if(index == 4)
		  {
		    query.prepare("INSERT INTO locations "
				  "(location, type) VALUES "
				  "(?, 'Magazine')");
		    query.bindValue(0,
				    tablewidget->item(j, 1)->text().trimmed());
		  }
		else if(index == 5)
		  {
		    query.prepare("INSERT INTO locations "
				  "(location, type) VALUES "
				  "(?, 'CD')");
		    query.bindValue(0,
				    tablewidget->item(j, 1)->text().trimmed());
		  }
		else if(index == 6)
		  {
		    query.prepare("INSERT INTO locations "
				  "(location, type) VALUES "
				  "(?, 'Photograph Collection')");
		    query.bindValue(0,
				    tablewidget->item(j, 1)->text().trimmed());
		  }
		else if(index == 7)
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
		       currentText +
		       tr(", ") +
		       tablewidget->item(j, 1)->text().trimmed() +
		       QString(tr(").")),
		       query.lastError().text(), __FILE__, __LINE__);
		    goto db_rollback;
		  }
	      }
	}
      else if(tablewidget && tablewidget == m_ui.minimumDaysTable)
	{
	  for(int j = 0; j < tablewidget->rowCount(); j++)
	    {
	      auto lineEdit = qobject_cast<QLineEdit *>
		(tablewidget->cellWidget(j, 1));

	      if(lineEdit)
		{
		  if(j == 0)
		    query.prepare("INSERT INTO minimum_days "
				  "(days, type) VALUES "
				  "(?, 'Book')");
		  else if(j == 1)
		    query.prepare("INSERT INTO minimum_days "
				  "(days, type) VALUES "
				  "(?, 'DVD')");
		  else if(j == 2)
		    query.prepare("INSERT INTO minimum_days "
				  "(days, type) VALUES "
				  "(?, 'Grey Literature')");
		  else if(j == 3)
		    query.prepare("INSERT INTO minimum_days "
				  "(days, type) VALUES "
				  "(?, 'Journal')");
		  else if(j == 4)
		    query.prepare("INSERT INTO minimum_days "
				  "(days, type) VALUES "
				  "(?, 'Magazine')");
		  else if(j == 5)
		    query.prepare("INSERT INTO minimum_days "
				  "(days, type) VALUES "
				  "(?, 'CD')");
		  else if(j == 6)
		    query.prepare("INSERT INTO minimum_days "
				  "(days, type) VALUES "
				  "(?, 'Video Game')");

		  query.bindValue(0, lineEdit->text());

		  if(!query.exec())
		    {
		      if(tablewidget->item(j, 0))
			qmain->addError
			  (QString(tr("Database Error")),
			   QString(tr("Unable to create the minimum day (")) +
			   tablewidget->item(j, 0)->text() +
			   tr(", ") +
			   lineEdit->text() +
			   QString(tr(").")),
			   query.lastError().text(), __FILE__, __LINE__);

		      goto db_rollback;
		    }
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
    populateWidgets();
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
