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

#include "biblioteq_otheroptions.h"

biblioteq_otheroptions::biblioteq_otheroptions(biblioteq *parent):
  QMainWindow(parent)
{
  m_qmain = parent;
  m_ui.setupUi(this);
  biblioteq_misc_functions::sortCombinationBox(m_ui.books_accession_number);
  connect(m_qmain,
	  SIGNAL(fontChanged(const QFont &)),
	  this,
	  SLOT(setGlobalFonts(const QFont &)));
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
  connect(m_ui.custom_query_reset,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotResetCustomQueryColors(void)));
  connect(m_ui.item_mandatory_field_color,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectColor(void)));
  connect(m_ui.item_query_result_color,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectColor(void)));
  connect(m_ui.main_window_canvas_background_color,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectMainwindowCanvasBackgroundColor(void)));
  connect(m_ui.members_mandatory_field_color,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectColor(void)));
  connect(m_ui.reset,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(m_ui.save,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSave(void)));
  m_keywordsItemDelegate = new biblioteq_otheroptions_item_delegate
    (biblioteq_otheroptions_item_delegate::ParentTypes::Keywords, this);
  m_shortcutsItemDelegate = new biblioteq_otheroptions_item_delegate
    (biblioteq_otheroptions_item_delegate::ParentTypes::Shortcuts, this);
  m_specialValueColorsItemDelegate = new biblioteq_otheroptions_item_delegate
    (biblioteq_otheroptions_item_delegate::ParentTypes::SpecialValueColors,
     this);
  connect(m_shortcutsItemDelegate,
	  SIGNAL(changed(void)),
	  this,
	  SLOT(slotMainWindowShortcutChanged(void)));
  m_ui.custom_query->setItemDelegateForColumn(1, m_keywordsItemDelegate);
  m_ui.date_format->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  m_ui.shortcuts->setItemDelegateForColumn(1, m_shortcutsItemDelegate);
  m_ui.special_value_colors->setItemDelegateForColumn
    (static_cast<int> (SpecialColorsColumns::Color),
     m_specialValueColorsItemDelegate);
  m_ui.special_value_colors->setItemDelegateForColumn
    (static_cast<int> (SpecialColorsColumns::Reset),
     m_specialValueColorsItemDelegate);
#ifndef BIBLIOTEQ_SQLITE3_INCLUDE_FILE_EXISTS
  m_ui.sqlite_runtime_loadable_extensions->setEnabled(false);
  m_ui.sqlite_runtime_loadable_extensions->setToolTip
    (tr("BiblioteQ was created without a required SQLite file (sqlite3.h).");
#endif
  prepareSQLKeywords();
  prepareSettings();
  prepareShortcuts();
  prepareSpecialColors();
  setWindowFlags(Qt::WindowStaysOnTopHint | windowFlags());
}

biblioteq_otheroptions::~biblioteq_otheroptions()
{
#ifndef Q_OS_ANDROID
  isVisible() ?
    QSettings().setValue("otheroptions_geometry", saveGeometry()) : (void) 0;
#endif
}

QColor biblioteq_otheroptions::availabilityColor(const QString &it) const
{
  QSettings settings;
  QString value("white");
  auto const itemType(QString(it).remove(' ').toLower().trimmed());

  if(itemType.contains("book"))
    value = settings.value
      ("otheroptions/book_availability_color").toString();
  else if(itemType.contains("cd") || itemType.contains("music"))
    value = settings.value
      ("otheroptions/cd_availability_color").toString();
  else if(itemType.contains("dvd"))
    value = settings.value
      ("otheroptions/dvd_availability_color").toString();
  else if(itemType.contains("grey"))
    value = settings.value
      ("otheroptions/grey_literature_availability_color").toString();
  else if(itemType.contains("journal"))
    value = settings.value
      ("otheroptions/journal_availability_color").toString();
  else if(itemType.contains("magazine"))
    value = settings.value
      ("otheroptions/magazine_availability_color").toString();
  else if(itemType.contains("video"))
    value = settings.value
      ("otheroptions/videogame_availability_color").toString();

  auto color(QColor(value.remove('&')));

  if(!color.isValid())
    color = QColor(Qt::white);

  return color;
}

QColor biblioteq_otheroptions::itemMandatoryFieldColor(void) const
{
  auto color
    (QColor(QSettings().value("otheroptions/item_mandatory_field_color").
	    toString().remove('&').trimmed()));

  if(!color.isValid())
    color = QColor(255, 248, 220);

  return color;
}

QColor biblioteq_otheroptions::itemQueryResultColor(void) const
{
  auto color(QColor(QSettings().value("otheroptions/item_query_result_color").
		    toString().remove('&').trimmed()));

  if(!color.isValid())
    color = QColor(162, 205, 90);

  return color;
}

QColor biblioteq_otheroptions::membersMandatoryFieldColor(void) const
{
  auto color
    (QColor(QSettings().value("otheroptions/members_mandatory_field_color").
	    toString().remove('&').trimmed()));

  if(!color.isValid())
    color = QColor(255, 248, 220);

  return color;
}

QMap<QString, QColor> biblioteq_otheroptions::customQueryColors(void) const
{
  QMap<QString, QColor> map;

  for(int i = 0; i < m_ui.custom_query->rowCount(); i++)
    {
      auto item1 = m_ui.custom_query->item(i, 0);
      auto item2 = m_ui.custom_query->item(i, 1);

      if(item1 && item2)
	map[item1->text()] = QColor(item2->text());
    }

  return map;
}

QString biblioteq_otheroptions::dateFormat(const QString &it) const
{
  QSettings settings;
  QString format("");
  auto const itemType(it.toLower().trimmed());

  if(itemType.contains("book"))
    format = settings.value("otheroptions/book_date_format").toString();
  else if(itemType.contains("cd") || itemType.contains("musiccds"))
    format = settings.value("otheroptions/cd_date_format").toString();
  else if(itemType.contains("dvd"))
    format = settings.value("otheroptions/dvd_date_format").toString();
  else if(itemType.contains("greyliterature"))
    format = settings.value
      ("otheroptions/grey_literature_date_format").toString();
  else if(itemType.contains("journal"))
    format = settings.value("otheroptions/journal_date_format").toString();
  else if(itemType.contains("magazine"))
    format = settings.value("otheroptions/magazine_date_format").toString();
  else if(itemType.contains("photographcollection"))
    format = settings.value("otheroptions/photograph_date_format").toString();
  else if(itemType.contains("videogame"))
    format = settings.value("otheroptions/videogame_date_format").toString();

  format = format.trimmed();

  if(format.isEmpty())
    return biblioteq::s_databaseDateFormat;
  else
    return format;
}

QString biblioteq_otheroptions::sqliteDatabaseEncoding(void) const
{
  auto const index = qBound
    (0,
     QSettings().value("otheroptions/sqlite_database_encoding_index").toInt(),
     m_ui.sqlite_database_encoding->count() - 1);

  switch(index)
    {
    case 0:
      {
	return "";
      }
    case 1:
      {
	return "UTF-8";
      }
    case 2:
      {
	return "UTF-16";
      }
    default:
      {
	return "";
      }
    }

  return "";
}

bool biblioteq_otheroptions::isMembersColumnVisible(const QString &text) const
{
  for(int i = 0; i < m_ui.members_visible_columns->count(); i++)
    {
      auto item = m_ui.members_visible_columns->item(i);

      if(item && item->text() == text)
	return item->checkState() == Qt::Checked;
    }

  return false;
}

bool biblioteq_otheroptions::showBookReadStatus(void) const
{
  return QSettings().value("otheroptions/book_read_status", false).toBool();
}

bool biblioteq_otheroptions::showMainTableImages(void) const
{
  return QSettings().value("show_maintable_images", true).toBool();
}

bool biblioteq_otheroptions::showMainTableProgressDialogs(void) const
{
  return QSettings().value("show_maintable_progress_dialogs", true).toBool();
}

int biblioteq_otheroptions::booksAccessionNumberIndex(void) const
{
  return qBound
    (0,
     QSettings().value("otheroptions/books_accession_number_index").toInt(),
     m_ui.books_accession_number->count() - 1);
}

int biblioteq_otheroptions::iconsViewColumnCount(void) const
{
  return qBound
    (m_ui.icons_view_column_count->minimum(),
     QSettings().value("otheroptions/icons_view_column_count").toInt(),
     m_ui.icons_view_column_count->maximum());
}

void biblioteq_otheroptions::changeEvent(QEvent *event)
{
  if(event)
    switch(event->type())
      {
      case QEvent::LanguageChange:
	{
	  m_ui.retranslateUi(this);
	  biblioteq_misc_functions::sortCombinationBox
	    (m_ui.books_accession_number);
	  break;
	}
      default:
	break;
      }

  QMainWindow::changeEvent(event);
}

void biblioteq_otheroptions::closeEvent(QCloseEvent *event)
{
#ifndef Q_OS_ANDROID
  isVisible() ?
    QSettings().setValue("otheroptions_geometry", saveGeometry()) : (void) 0;
#endif
  QMainWindow::closeEvent(event);
}

void biblioteq_otheroptions::keyPressEvent(QKeyEvent *event)
{
  if(event && event->key() == Qt::Key_Escape)
#ifdef Q_OS_ANDROID
    hide();
#else
    close();
#endif

  QMainWindow::keyPressEvent(event);
}

void biblioteq_otheroptions::prepareAvailability(void)
{
  QSettings settings;
  QStringList list1;
  QStringList list2;
  QStringList list3;

  list1 << tr("Books")
	<< tr("DVDs")
	<< tr("Grey Literature")
	<< tr("Journals")
	<< tr("Magazines")
	<< tr("Music CDs")
	<< tr("Video Games");
  list2 << settings.value("otheroptions/book_availability_color").toString()
	<< settings.value("otheroptions/dvd_availability_color").toString()
	<< settings.value("otheroptions/grey_literature_availability_color").
           toString()
	<< settings.value("otheroptions/journal_availability_color").toString()
	<< settings.value("otheroptions/magazine_availability_color").toString()
	<< settings.value("otheroptions/cd_availability_color").toString()
	<< settings.value("otheroptions/videogame_availability_color").
           toString();
  list3 << "otheroptions/book_availability_color"
	<< "otheroptions/dvd_availability_color"
	<< "otheroptions/grey_literature_availability_color"
	<< "otheroptions/journal_availability_color"
	<< "otheroptions/magazine_availability_color"
	<< "otheroptions/cd_availability_color"
	<< "otheroptions/videogame_availability_color";
  m_ui.availability_color->setRowCount(list1.size());
  m_ui.availability_color->setSortingEnabled(false);
  m_ui.availability_colors->setChecked
    (settings.value("otheroptions/availability_colors", false).toBool());

  for(int i = 0; i < list1.size(); i++)
    {
      auto item = new QTableWidgetItem(list1.at(i));
      auto layout = new QHBoxLayout();
      auto pushButton = new QPushButton();
      auto spacer1 = new QSpacerItem
	(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
      auto spacer2 = new QSpacerItem
	(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
      auto widget = new QWidget();

      connect(pushButton,
	      SIGNAL(clicked(void)),
	      this,
	      SLOT(slotSelectAvailabilityColor(void)));
      layout->addSpacerItem(spacer1);
      layout->addWidget(pushButton);
      layout->addSpacerItem(spacer2);
      layout->setContentsMargins(0, 0, 0, 0);
      item->setData(Qt::UserRole, list3.at(i));
      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.availability_color->setCellWidget
	(i, static_cast<int> (ItemsColumns::AVAILABILITY_COLOR), widget);
      m_ui.availability_color->setItem
	(i, static_cast<int> (ItemsColumns::ITEM_TYPE), item);
      biblioteq_misc_functions::assignImage
	(pushButton, QColor(QString(list2.at(i)).remove('&')));
      pushButton->setMinimumWidth(135);
      pushButton->setProperty("key", list3.at(i));
      pushButton->setText(QString(list2.at(i)).remove('&'));
      widget->setLayout(layout);
    }

  m_ui.availability_color->resizeColumnToContents
    (static_cast<int> (ItemsColumns::ITEM_TYPE));
  m_ui.availability_color->resizeRowsToContents();
  m_ui.availability_color->setSortingEnabled(true);
  m_ui.availability_color->sortByColumn(0, Qt::AscendingOrder);
}

void biblioteq_otheroptions::prepareEnvironmentVariables(void)
{
  auto const variable
    (QSettings().value("otheroptions/QT_STYLE_OVERRIDE").
     toByteArray().trimmed());

  qputenv("QT_STYLE_OVERRIDE", variable);
}

void biblioteq_otheroptions::prepareIcons(void)
{
  if(m_ui.display_icon_set->currentIndex() == 1)
    {
      // System.

      m_ui.close->setIcon
	(QIcon::fromTheme("window-close", QIcon(":/16x16/cancel.png")));
      m_ui.custom_query_reset->setIcon
	(QIcon::fromTheme("edit-reset", QIcon(":/16x16/reset.png")));
      m_ui.reset->setIcon
	(QIcon::fromTheme("edit-reset", QIcon(":/16x16/reset.png")));
      m_ui.save->setIcon
	(QIcon::fromTheme("document-save", QIcon(":/16x16/ok.png")));
    }
  else
    {
      // Faenza.

      m_ui.close->setIcon(QIcon(":/16x16/cancel.png"));
      m_ui.custom_query_reset->setIcon(QIcon(":/16x16/reset.png"));
      m_ui.reset->setIcon(QIcon(":/16x16/reset.png"));
      m_ui.save->setIcon(QIcon(":/16x16/ok.png"));
    }
}

void biblioteq_otheroptions::prepareMembersVisibleColumns(QTableWidget *table)
{
  if(!table)
    return;

  m_ui.members_visible_columns->clear();

  QMap<QString, bool> map;
  QSettings settings;

  foreach(auto const &string,
	  settings.value("otheroptions/members_visible_columns", "").
	  toString().split(','))
    {
      auto const list(string.split('='));

      if(list.size() == 2)
	map[list.at(0).mid(0, 128).trimmed()] =
	  QVariant(list.at(1).mid(0, 5).trimmed()).toBool();
    }

  for(int i = 0; i < table->columnCount(); i++)
    if(table->horizontalHeaderItem(i))
      {
	auto item = new QListWidgetItem
	  (table->horizontalHeaderItem(i)->text());

	if(map.contains(item->text()))
	  item->setCheckState
	    (map.value(item->text()) ? Qt::Checked : Qt::Unchecked);
	else
	  item->setCheckState(Qt::Checked);

	item->setFlags
	  (Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
	m_ui.members_visible_columns->addItem(item);
      }
}

void biblioteq_otheroptions::prepareSQLKeywords(void)
{
  QStringList list;

  /*
  ** Please do not translate the contents of list.
  */

  list << "/* A COMMENT. */"
       << "ALL"
       << "AND"
       << "ANY"
       << "AS"
       << "ASC"
       << "BETWEEN"
       << "CASE"
       << "DEFAULT"
       << "DELETE"
       << "DESC"
       << "DISTINCT"
       << "EXISTS"
       << "FROM"
       << "FULL OUTER JOIN"
       << "GROUP BY"
       << "HAVING"
       << "IN"
       << "INNER JOIN"
       << "IS NOT NULL"
       << "IS NULL"
       << "JOIN"
       << "LEFT JOIN"
       << "LIKE"
       << "LIMIT"
       << "MATCH"
       << "NOT"
       << "NOT NULL"
       << "OFFSET"
       << "ON"
       << "OR"
       << "ORDER BY"
       << "OUTER JOIN"
       << "RIGHT JOIN"
       << "ROWNUM"
       << "SELECT"
       << "SELECT DISTINCT"
       << "SELECT TOP"
       << "SET"
       << "TOP"
       << "UNION"
       << "UNION ALL"
       << "UNIQUE"
       << "UPDATE"
       << "VALUES"
       << "WHERE";
  m_ui.custom_query->setRowCount(list.size());
  m_ui.custom_query->setSortingEnabled(false);

  for(int i = 0; i < list.size(); i++)
    {
      auto item = new QTableWidgetItem(list.at(i));

      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.custom_query->setItem(i, 0, item);
      item = new QTableWidgetItem(QColor(Qt::black).name());
      item->setData(Qt::DecorationRole, QColor(Qt::black));
      item->setFlags
	(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.custom_query->setItem(i, 1, item);
    }

  m_ui.custom_query->setSortingEnabled(true);
  m_ui.custom_query->sortByColumn(0, Qt::AscendingOrder);
}

void biblioteq_otheroptions::prepareSettings(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  prepareAvailability();
  prepareIcons();
  prepareShortcuts();

  QSettings settings;
  QStringList list1;
  QStringList list2;
  QStringList list3;

  {
    QMap<QString, QColor> map;

    foreach(auto const &string,
	    settings.value("otheroptions/custom_query_colors", "").
	    toString().split(','))
      {
	auto const list(string.split('='));

	if(list.size() == 2)
	  map[list.at(0).mid(0, 64).toUpper().trimmed()] =
	    QColor(list.at(1).mid(0, 64).trimmed());
      }

    for(int i = 0; i < m_ui.custom_query->rowCount(); i++)
      {
	auto item1 = m_ui.custom_query->item(i, 0);
	auto item2 = m_ui.custom_query->item(i, 1);

	if(item1 && item2)
	  {
	    item2->setData(Qt::DecorationRole, map.value(item1->text()));
	    item2->setText(map.value(item1->text()).name());
	  }
      }
  }

  {
    QMap<QString, bool> map;

    foreach(auto const &string,
	    settings.value("otheroptions/members_visible_columns", "").
	    toString().split(','))
      {
	auto const list(string.split('='));

	if(list.size() == 2)
	  map[list.at(0).mid(0, 128).trimmed()] =
	    QVariant(list.at(1).mid(0, 5).trimmed()).toBool();
      }

    for(int i = 0; i < m_ui.members_visible_columns->count(); i++)
      {
	auto item = m_ui.members_visible_columns->item(i);

	if(item)
	  {
	    if(map.contains(item->text()))
	      item->setCheckState
		(map.value(item->text()) ? Qt::Checked : Qt::Unchecked);
	    else
	      item->setCheckState(Qt::Checked);
	  }
      }
  }

  list1 << tr("Books")
	<< tr("DVDs")
	<< tr("Grey Literature")
	<< tr("Journals")
	<< tr("Magazines")
	<< tr("Music CDs")
	<< tr("Photograph Collections")
	<< tr("Video Games");
  list2 << settings.value("otheroptions/book_date_format").toString()
	<< settings.value("otheroptions/dvd_date_format").toString()
	<< settings.value("otheroptions/grey_literature_date_format").toString()
	<< settings.value("otheroptions/journal_date_format").toString()
	<< settings.value("otheroptions/magazine_date_format").toString()
    	<< settings.value("otheroptions/cd_date_format").toString()
	<< settings.value("otheroptions/photograph_date_format").toString()
	<< settings.value("otheroptions/videogame_date_format").toString();
  list3 << "otheroptions/book_date_format"
	<< "otheroptions/dvd_date_format"
	<< "otheroptions/grey_literature_date_format"
	<< "otheroptions/journal_date_format"
	<< "otheroptions/magazine_date_format"
    	<< "otheroptions/cd_date_format"
	<< "otheroptions/photograph_date_format"
	<< "otheroptions/videogame_date_format";
  m_ui.book_read_status->setChecked
    (settings.value("otheroptions/book_read_status", false).toBool());
  m_ui.books_accession_number->setCurrentIndex
    (qBound(0,
	    settings.value("otheroptions/books_accession_number_index").toInt(),
	    m_ui.books_accession_number->count() - 1));
  m_ui.date_format->setRowCount(list1.size());
  m_ui.date_format->setSortingEnabled(false);

  for(int i = 0; i < list1.size(); i++)
    {
      auto str(list2.at(i).trimmed());

      if(str.isEmpty())
	str = biblioteq::s_databaseDateFormat;

      auto item = new QTableWidgetItem(list1.at(i));

      item->setData(Qt::UserRole, list3.at(i));
      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.date_format->setItem
	(i, static_cast<int> (ItemsColumns::ITEM_TYPE), item);
      item = new QTableWidgetItem(str);
      item->setData(Qt::UserRole, list3.at(i));
      item->setFlags
	(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.date_format->setItem
	(i, static_cast<int> (ItemsColumns::DATE_FORMAT), item);
    }

  m_ui.date_format->resizeColumnToContents
    (static_cast<int> (ItemsColumns::ITEM_TYPE));
  m_ui.date_format->resizeRowsToContents();
  m_ui.date_format->setSortingEnabled(true);
  m_ui.date_format->sortByColumn(0, Qt::AscendingOrder);
  m_ui.display_icon_set->setCurrentIndex
    (qBound(0,
	    settings.value("otheroptions/display_icon_set_index", 0).toInt(),
	    m_ui.display_icon_set->count() - 1));
  m_ui.generated_letter->setPlainText
    (QString::
     fromUtf8(QByteArray::
	      fromBase64(settings.value("otheroptions/generated_letter").
			 toByteArray()).constData()));
  m_ui.icons_view_column_count->setValue
    (settings.value("otheroptions/icons_view_column_count").toInt());
  m_ui.isbn10_display_format->setCurrentIndex
    (qBound(0,
	    settings.value("otheroptions/isbn10_display_format_index").toInt(),
	    m_ui.isbn10_display_format->count() - 1));
  m_ui.isbn13_display_format->setCurrentIndex
    (qBound(0,
	    settings.value("otheroptions/isbn13_display_format_index").toInt(),
	    m_ui.isbn13_display_format->count() - 1));

  QColor color
    (settings.value("otheroptions/item_mandatory_field_color").
     toString().remove('&').trimmed());

  if(!color.isValid())
    color = QColor(255, 248, 220);

  biblioteq_misc_functions::assignImage
    (m_ui.item_mandatory_field_color, color);
  m_ui.item_mandatory_field_color->setText(color.name());
  color = QColor
    (settings.value("otheroptions/item_query_result_color").
     toString().remove('&').trimmed());

  if(!color.isValid())
    color = QColor(162, 205, 90);

  biblioteq_misc_functions::assignImage(m_ui.item_query_result_color, color);
  m_ui.item_query_result_color->setText(color.name());
  color = QColor
    (settings.value("mainwindow_canvas_background_color").
     toString().remove('&').trimmed());

  if(!color.isValid())
    color = QColor(Qt::white);

  biblioteq_misc_functions::assignImage
    (m_ui.main_window_canvas_background_color, color);
  m_ui.main_window_canvas_background_color->setText(color.name());
  color = QColor
    (settings.value("otheroptions/members_mandatory_field_color").
     toString().remove('&').trimmed());

  if(!color.isValid())
    color = QColor(255, 248, 220);

  biblioteq_misc_functions::assignImage
    (m_ui.members_mandatory_field_color, color);
  m_ui.members_mandatory_field_color->setText(color.name());
  m_ui.only_utf8_printable_text->setChecked
    (settings.value("otheroptions/only_utf8_printable_text", false).toBool());
  m_ui.scripts->setPlainText
    (settings.value("otheroptions/scripts", "").toString().trimmed());
  m_ui.show_maintable_images->setChecked
    (settings.value("show_maintable_images", true).toBool());
  m_ui.show_maintable_progress_dialogs->setChecked
    (settings.value("show_maintable_progress_dialogs", true).toBool());
  m_ui.show_maintable_tooltips->setChecked
    (settings.value("show_maintable_tooltips", false).toBool());
  m_ui.sqlite_database_encoding->setCurrentIndex
    (qBound(0,
	    settings.value("otheroptions/sqlite_database_encoding_index", 0).
	    toInt(),
	    m_ui.sqlite_database_encoding->count() - 1));
  m_ui.sqlite_reminders->setPlainText
    (biblioteq_misc_functions::sqliteReturnReminders(m_qmain->getDB()));
#ifdef BIBLIOTEQ_SQLITE3_INCLUDE_FILE_EXISTS
  m_ui.sqlite_runtime_loadable_extensions->setPlainText
    (settings.value("otheroptions/sqlite_runtime_loadable_extensions").
     toString());
#else
  m_ui.sqlite_runtime_loadable_extensions->clear();
#endif
  m_ui.style_override->setText
    (settings.value("otheroptions/QT_STYLE_OVERRIDE").toString().trimmed());
  QApplication::restoreOverrideCursor();
}

void biblioteq_otheroptions::prepareShortcuts(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_ui.shortcuts->setRowCount(0);
  m_ui.shortcuts->setRowCount(m_qmain->shortcuts().size());
  m_ui.shortcuts->setSortingEnabled(false);

  auto const shortcut
    (QSettings().value("custom_query_favorite_shortcut").toString().trimmed());
  auto map(m_qmain->shortcuts());

  map[tr("Custom Query Favorite")] = shortcut;

  QMapIterator<QString, QKeySequence> it(map);
  int i = -1;

  while(it.hasNext())
    {
      i += 1;
      it.next();

      auto item = new QTableWidgetItem(it.key());

      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.shortcuts->setItem(i, 0, item);
      item = new QTableWidgetItem(it.value().toString());

      if(it.key() != tr("Custom Query Favorite"))
	item->setFlags(Qt::ItemIsSelectable);
      else
	item->setFlags
	  (Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

      item->setToolTip(item->text());
      m_ui.shortcuts->setItem(i, 1, item);
    }

  m_ui.shortcuts->resizeColumnsToContents();
  m_ui.shortcuts->setSortingEnabled(true);
  m_ui.shortcuts->sortByColumn(0, Qt::AscendingOrder);
  slotMainWindowShortcutChanged();
  QApplication::restoreOverrideCursor();
}

void biblioteq_otheroptions::prepareSpecialColors(const bool reset)
{
  QSettings settings;

  m_ui.special_value_colors->setRowCount
    (static_cast<int> (Limits::SpecialValueColorsRows));
  m_ui.special_value_colors->setSortingEnabled(false);

  for(int i = 0; i < m_ui.special_value_colors->rowCount(); i++)
    {
      QColor color;
      QString text("");
      QString title("");

      for(int j = 0; j < m_ui.special_value_colors->columnCount(); j++)
	{
	  auto item = new QTableWidgetItem
	    (qUtf8Printable(settings.
			    value(QString("special_value_colors_%1_%2").
				  arg(i).arg(j)).toString().trimmed()));

	  if(reset)
	    item->setText("");

	  if(j == static_cast<int> (SpecialColorsColumns::CellText))
	    text = item->text();
	  else if(j == static_cast<int> (SpecialColorsColumns::Color))
	    {
	      color = QColor(item->text().remove('&'));

	      if(color.isValid())
		item->setData(Qt::DecorationRole, color);
	      else
		item->setData(Qt::DecorationRole, QBrush());
	    }
	  else if(j == static_cast<int> (SpecialColorsColumns::ColumnTitle))
	    title = item->text();
	  else if(j == static_cast<int> (SpecialColorsColumns::Reset))
	    item->setText(tr("Reset Row"));

	  item->setFlags
	    (Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
	  m_ui.special_value_colors->setItem(i, j, item);
	}

      if(color.isValid() && text.isEmpty() == false && title.isEmpty() == false)
	m_specialValueColors[qMakePair(text, title)] = color;
    }

  m_ui.special_value_colors->setSortingEnabled(true);
  m_ui.special_value_colors->sortByColumn(0, Qt::AscendingOrder);
  reset ?
    (void) 0 :
    m_ui.special_value_colors_check_box->setChecked
    (settings.value("otheroptions/enable_special_values_colors").toBool());
}

void biblioteq_otheroptions::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(auto widget, findChildren<QWidget *> ())
    {
      auto f(font);

      f.setBold(widget->font().bold());
      widget->setFont(f);
      widget->update();
    }

  m_ui.date_format->resizeRowsToContents();
  update();
}

void biblioteq_otheroptions::showNormal(void)
{
  prepareSettings();
#ifndef Q_OS_ANDROID
  restoreGeometry(QSettings().value("otheroptions_geometry").toByteArray());
#endif
  QMainWindow::showNormal();
}

void biblioteq_otheroptions::slotClose(void)
{
#ifdef Q_OS_ANDROID
  hide();
#else
  close();
#endif
}

void biblioteq_otheroptions::slotMainWindowShortcutChanged(void)
{
  if(!m_ui.shortcuts->item(0, 0))
    return;

  QMap<QString, int> map;
  auto const static color(m_ui.shortcuts->item(0, 0)->background());

  for(int i = 0; i < m_ui.shortcuts->rowCount(); i++)
    {
      auto item = m_ui.shortcuts->item(i, 1);

      if(!item)
	continue;

      if(map.contains(item->text()))
	{
	  item->setBackground(QColor(255, 114, 118));
	  m_ui.shortcuts->item
	    (map.value(item->text()), 1)->setBackground(item->background());
	}
      else
	item->setBackground(color);

      map[item->text()] = i;
    }
}

void biblioteq_otheroptions::slotPreviewCanvasBackgroundColor
(const QColor &color)
{
  biblioteq_misc_functions::assignImage
    (m_ui.main_window_canvas_background_color, color);
  m_ui.main_window_canvas_background_color->setText(color.name());
}

void biblioteq_otheroptions::slotReset(void)
{
  for(int i = 0; i < m_ui.availability_color->rowCount(); i++)
    {
      auto widget = m_ui.availability_color->cellWidget
	(i, static_cast<int> (ItemsColumns::AVAILABILITY_COLOR));

      if(widget)
	{
	  auto pushButton = widget->findChild<QPushButton *> ();

	  if(pushButton)
	    {
	      biblioteq_misc_functions::assignImage
		(pushButton, QColor(Qt::white));
	      pushButton->setText(QColor(Qt::white).name());
	    }
	}
    }

  for(int i = 0; i < m_ui.date_format->rowCount(); i++)
    {
      auto item = m_ui.date_format->item
	(i, static_cast<int> (ItemsColumns::DATE_FORMAT));

      if(item)
	item->setText(biblioteq::s_databaseDateFormat);
    }

  for(int i = 0; i < m_ui.members_visible_columns->count(); i++)
    {
      auto item = m_ui.members_visible_columns->item(i);

      if(item)
	item->setCheckState(Qt::Checked);
    }

  {
    QColor const color(255, 248, 220);

    biblioteq_misc_functions::assignImage
      (m_ui.item_mandatory_field_color, color);
    biblioteq_misc_functions::assignImage
      (m_ui.members_mandatory_field_color, color);
    m_ui.item_mandatory_field_color->setText(color.name());
    m_ui.members_mandatory_field_color->setText(color.name());
  }

  {
    QColor const color(162, 205, 90);

    biblioteq_misc_functions::assignImage(m_ui.item_query_result_color, color);
    m_ui.item_query_result_color->setText(color.name());
  }

  {
    QColor const color(Qt::white);

    biblioteq_misc_functions::assignImage
      (m_ui.main_window_canvas_background_color, color);
    m_ui.main_window_canvas_background_color->setText(color.name());
  }

  {
    auto const list
      (m_ui.shortcuts->findItems(tr("Custom Query Favorite"),
				 Qt::MatchExactly));

    if(list.value(0))
      {
	auto item = m_ui.shortcuts->item(list.value(0)->row(), 1);

	if(item)
	  {
	    item->setText("");
	    item->setToolTip(item->text());
	  }
      }
  }

  prepareSpecialColors(true);
  m_ui.availability_color->clearSelection();
  m_ui.availability_color->resizeColumnToContents
    (static_cast<int> (ItemsColumns::ITEM_TYPE));
  m_ui.availability_color->resizeRowsToContents();
  m_ui.availability_color->scrollToTop();
  m_ui.availability_color->sortByColumn(0, Qt::AscendingOrder);
  m_ui.availability_colors->setChecked(false);
  m_ui.book_read_status->setChecked(false);
  m_ui.books_accession_number->setCurrentIndex
    (m_ui.books_accession_number->findText(tr("Numeric")));
  m_ui.books_accession_number->setCurrentIndex
    (m_ui.books_accession_number->currentIndex() < 0 ?
     0 : m_ui.books_accession_number->currentIndex());
  m_ui.date_format->clearSelection();
  m_ui.date_format->resizeColumnToContents
    (static_cast<int> (ItemsColumns::ITEM_TYPE));
  m_ui.date_format->resizeRowsToContents();
  m_ui.date_format->scrollToTop();
  m_ui.date_format->sortByColumn(0, Qt::AscendingOrder);
  m_ui.display_icon_set->setCurrentIndex(0);
  m_ui.generated_letter->clear();
  m_ui.icons_view_column_count->setValue(5);
  m_ui.isbn10_display_format->setCurrentIndex(0);
  m_ui.isbn13_display_format->setCurrentIndex(0);
  m_ui.only_utf8_printable_text->setChecked(false);
  m_ui.scripts->clear();
  m_ui.show_maintable_images->setChecked(true);
  m_ui.show_maintable_progress_dialogs->setChecked(true);
  m_ui.show_maintable_tooltips->setChecked(false);
  m_ui.shortcuts->clearSelection();
  m_ui.shortcuts->resizeColumnsToContents();
  m_ui.shortcuts->scrollToTop();
  m_ui.shortcuts->sortByColumn(0, Qt::AscendingOrder);
  m_ui.special_value_colors->clearSelection();
  m_ui.special_value_colors->scrollToTop();
  m_ui.special_value_colors->sortByColumn(0, Qt::AscendingOrder);
  m_ui.special_value_colors_check_box->setChecked(false);
  m_ui.sqlite_database_encoding->setCurrentIndex(0);
  m_ui.sqlite_reminders->clear();
  m_ui.sqlite_runtime_loadable_extensions->clear();
  m_ui.style_override->clear();
  slotResetCustomQueryColors();
}

void biblioteq_otheroptions::slotResetCustomQueryColors(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_ui.custom_query->clearSelection();
  m_ui.custom_query->setSortingEnabled(false);

  for(int i = 0; i < m_ui.custom_query->rowCount(); i++)
    {
      auto item = m_ui.custom_query->item(i, 1);

      if(item)
	{
	  item->setData(Qt::DecorationRole, QColor(Qt::black));
	  item->setText(QColor(Qt::black).name());
	}
    }

  m_ui.custom_query->resizeColumnsToContents();
  m_ui.custom_query->scrollToTop();
  m_ui.custom_query->setSortingEnabled(true);
  m_ui.custom_query->sortByColumn(0, Qt::AscendingOrder);
  QApplication::restoreOverrideCursor();
}

void biblioteq_otheroptions::slotSave(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  biblioteq_misc_functions::saveSqliteReturnReminders
    (m_qmain->getDB(), m_ui.sqlite_reminders->toPlainText());

  QSettings settings;
  QString string("");

  settings.setValue
    ("otheroptions/QT_STYLE_OVERRIDE", m_ui.style_override->text().trimmed());
  settings.setValue
    ("otheroptions/availability_colors", m_ui.availability_colors->isChecked());

  for(int i = 0; i < m_ui.custom_query->rowCount(); i++)
    {
      auto item1 = m_ui.custom_query->item(i, 0);
      auto item2 = m_ui.custom_query->item(i, 1);

      if(item1 && item2)
	{
	  string.append(item1->text());
	  string.append("=");
	  string.append(item2->text().remove('&'));
	  string.append(",");
	}
    }

  if(string.isEmpty())
    settings.setValue("otheroptions/custom_query_colors", "");
  else
    settings.setValue
      ("otheroptions/custom_query_colors", string.mid(0, string.length() - 1));

  settings.setValue
    ("otheroptions/enable_special_values_colors",
     m_ui.special_value_colors_check_box->isChecked());
  string.clear();

  for(int i = 0; i < m_ui.members_visible_columns->count(); i++)
    {
      auto item = m_ui.members_visible_columns->item(i);

      if(item)
	{
	  string.append(item->text());
	  string.append("=");
	  string.append(item->checkState() == Qt::Checked ? "true" : "false");
	  string.append(",");
	}
    }

  if(string.isEmpty())
    settings.setValue("otheroptions/members_visible_columns", "");
  else
    settings.setValue
      ("otheroptions/members_visible_columns",
       string.mid(0, string.length() - 1));

  for(int i = 0; i < m_ui.availability_color->rowCount(); i++)
    {
      auto widget = m_ui.availability_color->cellWidget
	(i, static_cast<int> (ItemsColumns::AVAILABILITY_COLOR));

      if(widget)
	{
	  auto pushButton = widget->findChild<QPushButton *> ();

	  if(pushButton)
	    settings.setValue
	      (pushButton->property("key").toString(),
	       pushButton->text().remove('&').trimmed());
	}
    }

  for(int i = 0; i < m_ui.date_format->rowCount(); i++)
    {
      auto item = m_ui.date_format->item
	(i, static_cast<int> (ItemsColumns::DATE_FORMAT));

      if(item)
	{
	  auto str(item->text().trimmed());

	  if(str.isEmpty())
	    {
	      item->setText(biblioteq::s_databaseDateFormat);
	      str = item->text();
	    }

	  settings.setValue(item->data(Qt::UserRole).toString(), str);
	}
    }

  for(int i = 0; i < m_ui.shortcuts->rowCount(); i++)
    if(m_ui.shortcuts->item(i, 0) &&
       m_ui.shortcuts->item(i, 0)->text() == tr("Custom Query Favorite") &&
       m_ui.shortcuts->item(i, 1))
      {
	m_ui.shortcuts->item(i, 1)->setToolTip
	  (m_ui.shortcuts->item(i, 1)->text().trimmed());
	settings.setValue
	  ("custom_query_favorite_shortcut",
	   m_ui.shortcuts->item(i, 1)->text().trimmed());
	break;
      }

  settings.setValue
    ("mainwindow_canvas_background_color",
     m_ui.main_window_canvas_background_color->text().remove('&'));
  settings.setValue
    ("otheroptions/book_read_status", m_ui.book_read_status->isChecked());
  settings.setValue
    ("otheroptions/books_accession_number_index",
     m_ui.books_accession_number->currentIndex());
  settings.setValue
    ("otheroptions/display_icon_set_index",
     m_ui.display_icon_set->currentIndex());
  settings.setValue
    ("otheroptions/generated_letter",
     m_ui.generated_letter->toPlainText().toUtf8().toBase64());
  settings.setValue
    ("otheroptions/icons_view_column_count",
     m_ui.icons_view_column_count->value());
  settings.setValue
    ("otheroptions/isbn10_display_format_index",
     m_ui.isbn10_display_format->currentIndex());
  settings.setValue
    ("otheroptions/isbn13_display_format_index",
     m_ui.isbn13_display_format->currentIndex());
  settings.setValue
    ("otheroptions/item_mandatory_field_color",
     m_ui.item_mandatory_field_color->text().remove('&'));
  settings.setValue
    ("otheroptions/item_query_result_color",
     m_ui.item_query_result_color->text().remove('&'));
  settings.setValue
    ("otheroptions/members_mandatory_field_color",
     m_ui.members_mandatory_field_color->text().remove('&'));
  settings.setValue
    ("otheroptions/only_utf8_printable_text",
     m_ui.only_utf8_printable_text->isChecked());
  settings.setValue
    ("otheroptions/scripts", m_ui.scripts->toPlainText().trimmed());
  settings.setValue
    ("otheroptions/sqlite_database_encoding_index",
     m_ui.sqlite_database_encoding->currentIndex());
  settings.setValue
    ("otheroptions/sqlite_runtime_loadable_extensions",
     m_ui.sqlite_runtime_loadable_extensions->toPlainText().trimmed());
  settings.setValue
    ("show_maintable_images", m_ui.show_maintable_images->isChecked());
  settings.setValue
    ("show_maintable_progress_dialogs",
     m_ui.show_maintable_progress_dialogs->isChecked());
  settings.setValue
    ("show_maintable_tooltips", m_ui.show_maintable_tooltips->isChecked());

  if(settings.status() == QSettings::NoError)
    m_ui.save->animate(2500);
  else
    m_ui.save->animateNegatively(2500);

  m_specialValueColors.clear();

  for(int i = 0; i < m_ui.special_value_colors->rowCount(); i++)
    {
      QColor color;
      QString text("");
      QString title("");

      for(int j = 0; j < m_ui.special_value_colors->columnCount(); j++)
	if(j != static_cast<int> (SpecialColorsColumns::Reset))
	  {
	    auto item = m_ui.special_value_colors->item(i, j);

	    if(item)
	      {
		item->setText(item->text().trimmed());

		if(j == static_cast<int> (SpecialColorsColumns::CellText))
		  text = item->text();
		else if(j == static_cast<int> (SpecialColorsColumns::Color))
		  color = QColor(item->text().remove('&'));
		else if(j == static_cast<int> (SpecialColorsColumns::
					       ColumnTitle))
		  title = item->text();

		settings.setValue
		  (QString("special_value_colors_%1_%2").arg(i).arg(j),
		   qUtf8Printable(item->text()));
	      }
	  }

      if(color.isValid() && text.isEmpty() == false && title.isEmpty() == false)
	m_specialValueColors[qMakePair(text, title)] = color;
    }

  m_isbn10Format = m_ui.isbn10_display_format->currentText();
  m_isbn13Format = m_ui.isbn13_display_format->currentText();
  emit mainWindowCanvasBackgroundColorChanged
    (QColor(m_ui.main_window_canvas_background_color->text().remove('&')));
  emit saved();
  prepareIcons();
  QApplication::restoreOverrideCursor();
}

void biblioteq_otheroptions::slotSelectAvailabilityColor(void)
{
  auto pushButton = qobject_cast<QPushButton *> (sender());

  if(!pushButton)
    return;

  QColorDialog dialog(this);

  dialog.setCurrentColor(QColor(pushButton->text().remove('&')));

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      biblioteq_misc_functions::assignImage
	(pushButton, dialog.selectedColor());
      pushButton->setText(dialog.selectedColor().name());
    }
  else
    QApplication::processEvents();
}

void biblioteq_otheroptions::slotSelectColor(void)
{
  auto pushButton = qobject_cast<QPushButton *> (sender());

  if(!pushButton)
    return;

  QColor const color(pushButton->text().remove('&').trimmed());
  QColorDialog dialog(this);

  dialog.setCurrentColor(color);

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      biblioteq_misc_functions::assignImage
	(pushButton, dialog.selectedColor());
      pushButton->setText(dialog.selectedColor().name());
    }
  else
    QApplication::processEvents();
}

void biblioteq_otheroptions::slotSelectMainwindowCanvasBackgroundColor(void)
{
  QColor const color
    (m_ui.main_window_canvas_background_color->text().remove('&'));
  QColorDialog dialog(this);

  dialog.setCurrentColor(color);
  connect(&dialog,
	  SIGNAL(currentColorChanged(const QColor &)),
	  this,
	  SIGNAL(mainWindowCanvasBackgroundColorPreview(const QColor &)));
  connect(&dialog,
	  SIGNAL(currentColorChanged(const QColor &)),
	  this,
	  SLOT(slotPreviewCanvasBackgroundColor(const QColor &)));

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      biblioteq_misc_functions::assignImage
	(m_ui.main_window_canvas_background_color, dialog.selectedColor());
      m_ui.main_window_canvas_background_color->setText
	(dialog.selectedColor().name());
    }
  else
    {
      QApplication::processEvents();
      biblioteq_misc_functions::assignImage
	(m_ui.main_window_canvas_background_color, color);
      emit mainWindowCanvasBackgroundColorChanged(QColor());
      m_ui.main_window_canvas_background_color->setText(color.name());
    }
}

void biblioteq_otheroptions::slotShowCustomQuery(void)
{
  m_ui.tab->setCurrentIndex(0);
  showNormal();
}
