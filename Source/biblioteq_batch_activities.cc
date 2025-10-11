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

#include <QComboBox>
#include <QCompleter>
#include <QFileDialog>
#include <QProgressDialog>
#include <QSettings>
#include <QSqlField>
#include <QSqlRecord>
#include <QTextStream>
#include <QtMath>

#include "biblioteq.h"
#include "biblioteq_batch_activities.h"

QColor biblioteq_batch_activities::s_notSoOkColor =
  QColor(255, 114, 118); // Red light.
QColor biblioteq_batch_activities::s_okColor =
  QColor(144, 238, 144); // Green light.

biblioteq_batch_activities_item_delegate::
biblioteq_batch_activities_item_delegate(QObject *parent):
  QStyledItemDelegate(parent)
{
}

QWidget *biblioteq_batch_activities_item_delegate::createEditor
(QWidget *parent,
 const QStyleOptionViewItem &option,
 const QModelIndex &index) const
{
  switch(index.column())
    {
    case static_cast<int> (biblioteq_batch_activities::
			   DreamyTableColumns::
			   NEW_RETURN_DATE):
      {
	auto editor = new QDateEdit(parent);

	editor->setCalendarPopup(true);
	editor->setDate
	  (QDate::fromString(index.data().toString().trimmed(),
			     biblioteq::s_databaseDateFormat));
	return editor;
      }
    default:
      {
	break;
      }
    }

  return QStyledItemDelegate::createEditor(parent, option, index);
}


biblioteq_batch_activities::biblioteq_batch_activities(biblioteq *parent):
  QMainWindow(parent)
{
  m_qmain = parent;
  m_ui.setupUi(this);
#ifndef BIBLIOTEQ_AUDIO_SUPPORTED
  m_ui.audio->setChecked(false);
  m_ui.audio->setEnabled(false);
  m_ui.audio->setToolTip
    (tr("BiblioteQ was not created with multimedia libraries."));
#else
  m_ui.audio->setChecked
    (QSettings().value("otheroptions/batch_activities_audio", false).toBool());
#endif

  QAction *action1 = nullptr;
  QAction *action2 = nullptr;

  m_ui.discover_list->addAction
    (action1 = new QAction(tr("List Discovered Items"), this));
  action1->setProperty("in-or-not-in", " IN ");
  m_ui.discover_list->addAction
    (action2 = new QAction(tr("List Other Items"), this));
  action2->setProperty("in-or-not-in", " NOT IN ");
  connect(action1,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotListDiscoveredItems(void)));
  connect(action2,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotListDiscoveredItems(void)));
  connect(m_qmain,
	  SIGNAL(fontChanged(const QFont &)),
	  this,
	  SLOT(slotSetGlobalFonts(const QFont &)));
  connect(m_ui.audio,
	  SIGNAL(toggled(bool)),
	  this,
	  SLOT(slotAudioEnabled(void)));
  connect(m_ui.borrow_add_row,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAddBorrowingRow(void)));
  connect(m_ui.borrow_delete_row,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotDeleteBorrowingRow(void)));
  connect(m_ui.borrow_list,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotListMembersReservedItems(void)));
  connect(m_ui.borrow_member_id,
	  SIGNAL(editingFinished(void)),
	  this,
	  SLOT(slotDiscoverMemberName(void)));
  connect(m_ui.borrow_member_id,
	  SIGNAL(textEdited(const QString &)),
	  m_ui.borrow_member_name,
	  SLOT(clear(void)));
  connect(m_ui.borrow_member_id,
	  SIGNAL(textEdited(const QString &)),
	  this,
	  SLOT(slotMemberIdEdited(const QString &)));
  connect(m_ui.borrow_scan,
	  SIGNAL(returnPressed(void)),
	  this,
	  SLOT(slotScannedBorrowing(void)));
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
  connect(m_ui.discover_list,
	  SIGNAL(clicked(void)),
	  m_ui.discover_list,
	  SLOT(showMenu(void)));
  connect(m_ui.discover_scan,
	  SIGNAL(returnPressed(void)),
	  this,
	  SLOT(slotScannedDiscover(void)));
  connect(m_ui.dreamy_member_id,
	  SIGNAL(editingFinished(void)),
	  this,
	  SLOT(slotDiscoverMemberName(void)));
  connect(m_ui.dreamy_member_id,
	  SIGNAL(returnPressed(void)),
	  this,
	  SLOT(slotDiscoverDreamy(void)));
  connect(m_ui.dreamy_member_id,
	  SIGNAL(textEdited(const QString &)),
	  m_ui.dreamy_member_name,
	  SLOT(clear(void)));
  connect(m_ui.dreamy_member_id,
	  SIGNAL(textEdited(const QString &)),
	  this,
	  SLOT(slotMemberIdEdited(const QString &)));
  connect(m_ui.export_missing,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotExportMissing(void)));
  connect(m_ui.go,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotGo(void)));
  connect(m_ui.reset,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(m_ui.return_scan,
	  SIGNAL(returnPressed(void)),
	  this,
	  SLOT(slotScannedReturn(void)));
  connect(m_ui.tab,
	  SIGNAL(currentChanged(int)),
	  this,
	  SLOT(slotPageIndexChanged(int)));
  m_ui.dreamy_table->horizontalHeader()->setSortIndicator
    (static_cast<int> (DreamyTableColumns::NEW_RETURN_DATE),
     Qt::AscendingOrder);
  m_ui.dreamy_table->setColumnHidden
    (m_ui.dreamy_table->columnCount() - 1, true);
  m_ui.dreamy_table->setItemDelegateForColumn
    (static_cast<int> (DreamyTableColumns::NEW_RETURN_DATE),
     new biblioteq_batch_activities_item_delegate(this));
  m_ui.tab->setCurrentIndex
    (qBound(0,
	    QSettings().value("otheroptions/batch_activities_page_index").
	    toInt(),
	    m_ui.tab->count() - 1));
  prepareIcons();
  slotPageIndexChanged(m_ui.tab->currentIndex());
}

biblioteq_batch_activities::~biblioteq_batch_activities()
{
#ifndef Q_OS_ANDROID
  isVisible() ?
    QSettings().setValue("batch_activities_geometry", saveGeometry()) :
    (void) 0;
#endif
}

void biblioteq_batch_activities::borrow(void)
{
  auto const memberId(m_ui.borrow_member_id->text().trimmed());

  if(memberId.isEmpty())
    {
      m_ui.borrow_member_id->setFocus();
      m_ui.borrow_member_id->setPlaceholderText
	(tr("Please provide the patron's identifier."));
      return;
    }

  QProgressDialog progress(this);

  progress.setLabelText(tr("Borrowing item(s)..."));
  progress.setMaximum(m_ui.borrow_table->rowCount());
  progress.setMinimum(0);
  progress.setMinimumWidth
    (qCeil(biblioteq::PROGRESS_DIALOG_WIDTH_MULTIPLIER *
	   progress.sizeHint().width()));
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.show();
  progress.repaint();

  QString error("");
  auto const expired = biblioteq_misc_functions::hasMemberExpired
    (m_qmain->getDB(), memberId, error);
  auto const maximumReserved = biblioteq_misc_functions::maximumReserved
    (m_qmain->getDB(), memberId, "book"); // Only books offer maximums.

  for(int i = 0; i < m_ui.borrow_table->rowCount(); i++)
    {
      if(progress.wasCanceled())
	break;

      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      progress.repaint();
      QApplication::processEvents();

      QComboBox *category = nullptr;
      auto widget = m_ui.borrow_table->cellWidget
	(i, static_cast<int> (BorrowTableColumns::CATEGORY_COLUMN));

      if(widget)
	category = widget->findChild<QComboBox *> ();

      auto const copyIdentifier = m_ui.borrow_table->item
	(i, static_cast<int> (BorrowTableColumns::COPY_IDENTIFIER_COLUMN));
      auto const identifier = m_ui.borrow_table->item
	(i, static_cast<int> (BorrowTableColumns::IDENTIFIER_COLUMN));
      auto const results = m_ui.borrow_table->item
	(i, static_cast<int> (BorrowTableColumns::RESULTS_COLUMN));

      if(!category || !copyIdentifier || !identifier || !results)
	{
	  if(results)
	    {
	      results->setBackground(s_notSoOkColor);
	      results->setText(tr("Critical error! Missing table item(s)."));
	      results->setToolTip(results->text());
	    }

	  continue;
	}

      results->setBackground(copyIdentifier->background());
      results->setText("");
      results->setToolTip("");

      if(expired)
	{
	  results->setBackground(s_notSoOkColor);

	  if(error.isEmpty())
	    results->setText(tr("Membership has expired."));
	  else
	    results->setText(tr("Cannot locate member (%1).").arg(error));

	  results->setToolTip(results->text());
	  continue;
	}

      if(copyIdentifier->text().trimmed().isEmpty())
	{
	  results->setBackground(s_notSoOkColor);
	  results->setText(tr("Empty copy identifier."));
	  results->setToolTip(results->text());
	  continue;
	}
      else if(identifier->text().trimmed().isEmpty())
	{
	  results->setBackground(s_notSoOkColor);
	  results->setText(tr("Empty identifier."));
	  results->setToolTip(results->text());
	  continue;
	}

      QString error("");
      QString type("");

      if(category && category->currentText() == tr("Book"))
	type = "Book";
      else if(category && category->currentText() == tr("CD"))
	type = "CD";
      else if(category && category->currentText() == tr("DVD"))
	type = "DVD";
      else if(category && category->currentText() == tr("Grey Literature"))
	type = "Grey Literature";
      else if(category && category->currentText() == tr("Journal"))
	type = "Journal";
      else if(category && category->currentText() == tr("Magazine"))
	type = "Magazine";
      else if(category && category->currentText() == tr("Video Game"))
	type = "Video Game";

      auto const available = biblioteq_misc_functions::isItemAvailable
	(error,
	 m_qmain->getDB(),
	 identifier->text(),
	 copyIdentifier->text(),
	 type);

      if(!available)
	{
	  results->setBackground(s_notSoOkColor);

	  if(error.isEmpty())
	    results->setText(tr("Item is not available for reservation."));
	  else
	    results->setText
	      (tr("Item is not available (%1) for reservation.").arg(error));

	  results->setToolTip(results->text());
	  continue;
	}

      if(maximumReserved > 0)
	{
	  auto const totalReserved = biblioteq_misc_functions::
	    getItemsReservedCounts(m_qmain->getDB(), memberId, error).
	    value("numbooks");

	  if(maximumReserved <= totalReserved)
	    {
	      results->setBackground(s_notSoOkColor);
	      results->setText
		(tr("Maximum (%1) number of reserved (%2) items exceeded.").
		 arg(maximumReserved).arg(totalReserved));
	      results->setToolTip(results->text());
	      continue;
	    }
	}

      /*
      ** Reserve the item.
      */

      QSqlQuery query(m_qmain->getDB());
      QString errorstr("");
      auto dueDate(QDate::currentDate());
      auto const itemOid = biblioteq_misc_functions::getOID
	(identifier->text(), type, m_qmain->getDB(), errorstr);
      auto const copyNumber = biblioteq_misc_functions::getCopyNumber
	(m_qmain->getDB(), copyIdentifier->text(), itemOid, type, errorstr);

      if(copyNumber == -1)
	{
	  results->setBackground(s_notSoOkColor);
	  results->setText(tr("Error retrieving copy number."));
	  results->setToolTip(results->text());
	  continue;
	}

      dueDate = dueDate.addDays
	(biblioteq_misc_functions::
	 getMinimumDays(m_qmain->getDB(), type, errorstr));
      query.prepare("INSERT INTO item_borrower "
		    "(copy_number, "
		    "copyid, "
		    "duedate, "
		    "item_oid, "
		    "memberid, "
		    "reserved_by, "
		    "reserved_date, "
		    "type) "
		    "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
      query.addBindValue(copyNumber);
      query.addBindValue(copyIdentifier->text());
      query.addBindValue(dueDate.toString(biblioteq::s_databaseDateFormat));
      query.addBindValue(itemOid);
      query.addBindValue(memberId);
      query.addBindValue(m_qmain->getAdminID());
      query.addBindValue
	(QDate::currentDate().toString(biblioteq::s_databaseDateFormat));
      query.addBindValue(type);

      if(query.exec())
	{
	  copyIdentifier->setData(Qt::BackgroundRole, QVariant());
	  results->setBackground(s_okColor);
	  results->setText(tr("Reserved!"));
	  results->setToolTip("");

	  /*
	  ** Delete the request.
	  */

	  QSqlQuery query(m_qmain->getDB());

	  query.prepare
	    ("DELETE FROM item_request WHERE "
	     "item_oid = ? AND memberid = ? and type = ?");
	  query.addBindValue(itemOid);
	  query.addBindValue(memberId);
	  query.addBindValue(type);
	  query.exec();
	}
      else
	{
	  results->setBackground(s_notSoOkColor);
	  results->setText
	    (tr("Reservation problem (%1).").arg(query.lastError().text()));
	  results->setToolTip(results->text());
	  continue;
	}

      auto const dnt = biblioteq_misc_functions::dnt
	(m_qmain->getDB(), memberId, errorstr);

      if(!dnt)
	{
	  query.prepare("INSERT INTO member_history "
			"(memberid, "
			"item_oid, "
			"copyid, "
			"reserved_date, "
			"duedate, "
			"returned_date, "
			"reserved_by, "
			"type) "
			"VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
	  query.addBindValue(memberId);
	  query.addBindValue(itemOid);
	  query.addBindValue(copyIdentifier->text());
	  query.addBindValue
	    (QDate::currentDate().toString(biblioteq::s_databaseDateFormat));
	  query.addBindValue
	    (dueDate.toString(biblioteq::s_databaseDateFormat));
	  query.addBindValue(QString("N/A"));
	  query.addBindValue(m_qmain->getAdminID());
	  query.addBindValue(type);

	  if(!query.exec())
	    m_qmain->addError(tr("Database Error"),
			      tr("Unable to create a member_history entry."),
			      query.lastError().text(),
			      __FILE__,
			      __LINE__);
	}
    }

  progress.close();
  QApplication::processEvents();
}

void biblioteq_batch_activities::changeEvent(QEvent *event)
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
	{
	  break;
	}
      }

  QMainWindow::changeEvent(event);
}

void biblioteq_batch_activities::closeEvent(QCloseEvent *event)
{
#ifndef Q_OS_ANDROID
  isVisible() ?
    QSettings().setValue("batch_activities_geometry", saveGeometry()) :
    (void) 0;
#endif
  QMainWindow::closeEvent(event);
}

void biblioteq_batch_activities::discover(void)
{
}

void biblioteq_batch_activities::play(const QString &file)
{
  if(!m_ui.audio->isChecked())
    return;

#ifdef BIBLIOTEQ_AUDIO_SUPPORTED
  auto player = findChild<QMediaPlayer *> ();

  if(player)
    player->deleteLater();

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
  m_audioOutput.reset(new QAudioOutput());
  m_audioOutput->setVolume(1.0);
  player = new QMediaPlayer(this);
  player->setAudioOutput(m_audioOutput.data());
  player->setSource(QUrl::fromUserInput(file));
  connect(player,
	  SIGNAL(errorOccurred(QMediaPlayer::Error, const QString &)),
	  this,
	  SLOT(slotMediaError(QMediaPlayer::Error, const QString &)));
#else
  player = new QMediaPlayer(this, QMediaPlayer::LowLatency);
  connect(player,
	  SIGNAL(error(QMediaPlayer::Error)),
	  this,
	  SLOT(slotMediaError(QMediaPlayer::Error)));
  player->setMedia(QUrl::fromUserInput(file));
  player->setVolume(100);
#endif
  connect(player,
	  SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
	  this,
	  SLOT(slotMediaStatusChanged(QMediaPlayer::MediaStatus)));
  player->play();
#else
  Q_UNUSED(file);
#endif
}

void biblioteq_batch_activities::prepareIcons(void)
{
  QSettings setting;
  auto const index = setting.value
    ("otheroptions/display_icon_set_index", 0).toInt();

  if(index == 1)
    {
      // System.

      m_ui.borrow_add_row->setIcon
	(QIcon::fromTheme("list-add", QIcon(":/16x16/add.png")));
      m_ui.borrow_delete_row->setIcon
	(QIcon::fromTheme("list-remove", QIcon(":/16x16/eraser.png")));
      m_ui.borrow_list->setIcon(QIcon(":/32x32/stamp.png"));
      m_ui.close->setIcon
	(QIcon::fromTheme("window-close", QIcon(":/32x32/cancel.png")));
      m_ui.export_missing->setIcon
	(QIcon::fromTheme("document-save-as", QIcon(":/32x32/save.png")));
      m_ui.go->setIcon(QIcon::fromTheme("dialog-ok", QIcon(":/32x32/ok.png")));
      m_ui.reset->setIcon
	(QIcon::fromTheme("view-refresh", QIcon(":/32x32/reload.png")));
    }
  else
    {
      // Faenza.

      m_ui.borrow_add_row->setIcon(QIcon(":/16x16/add.png"));
      m_ui.borrow_delete_row->setIcon(QIcon(":/16x16/eraser.png"));
      m_ui.borrow_list->setIcon(QIcon(":/32x32/stamp.png"));
      m_ui.close->setIcon(QIcon(":/32x32/cancel.png"));
      m_ui.export_missing->setIcon(QIcon(":/32x32/save.png"));
      m_ui.go->setIcon(QIcon(":/32x32/ok.png"));
      m_ui.reset->setIcon(QIcon(":/32x32/reload.png"));
    }
}

void biblioteq_batch_activities::reset(void)
{
  m_memberIdCompleter ? m_memberIdCompleter->deleteLater() : (void) 0;
  m_memberIdModel ? m_memberIdModel->deleteLater() : (void) 0;
  m_ui.borrow_member_id->setCompleter(nullptr);
  m_ui.dreamy_member_id->setCompleter(nullptr);
  slotReset();
}

void biblioteq_batch_activities::returnItems(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  QSet<QString> values;
  QString bookFrontCover("'' AS front_cover ");
  QString cdFrontCover("'' AS front_cover ");
  QString dvdFrontCover("'' AS front_cover ");
  QString greyLiteratureFrontCover("'' AS front_cover ");
  QString in("(");
  QString journalFrontCover("'' AS front_cover ");
  QString magazineFrontCover("'' AS front_cover ");
  QString searchstr("");
  QString videoGameFrontCover("'' AS front_cover ");

  if(m_qmain->showMainTableImages())
    {
      bookFrontCover = "book.front_cover ";
      cdFrontCover = "cd.front_cover ";
      dvdFrontCover = "dvd.front_cover ";
      greyLiteratureFrontCover = "grey_literature.front_cover ";
      journalFrontCover = "journal.front_cover ";
      magazineFrontCover = "magazine.front_cover ";
      videoGameFrontCover = "videogame.front_cover ";
    }

  for(int i = 0; i < m_ui.return_table->rowCount(); i++)
    {
      auto item = m_ui.return_table->item
	(i, m_ui.return_table->columnCount() - 1);

      if(!item)
	continue;

      if(!values.contains(item->text()))
	{
	  in.append("?");
	  in.append(", ");
	  values << item->text();
	}
    }

  if(values.isEmpty())
    {
      QApplication::restoreOverrideCursor();
      (void) m_qmain->populateTable
	(biblioteq::POPULATE_ALL, "All Reserved", "");
      return;
    }

  auto query = new QSqlQuery(m_qmain->getDB());

  in = in.mid(0, in.length() - 2); // Remove the last two characters.
  in.append(")");
  searchstr.append("SELECT DISTINCT "
		   "member.last_name || ', ' || "
		   "member.first_name AS name, "
		   "member.memberid, "
		   "member.telephone_num, "
		   "item_borrower.copyid, "
		   "item_borrower.reserved_date, "
		   "item_borrower.duedate, "
		   "book.title, "
		   "book.id, "
		   "book.callnumber, "
		   "book.publisher, "
		   "book.pdate, "
		   "book.category, "
		   "book.language, "
		   "book.price, "
		   "book.monetary_units, "
		   "book.quantity, "
		   "book.location, "
		   "book.quantity - "
		   "COUNT(item_borrower.item_oid) "
		   "AS availability, "
		   "COUNT(item_borrower.item_oid) AS "
		   "total_reserved, "
		   "book.accession_number, "
		   "book.type, "
		   "book.myoid, " +
		   bookFrontCover +
		   "FROM "
		   "member, "
		   "book LEFT JOIN item_borrower ON "
		   "book.myoid = item_borrower.item_oid "
		   "AND item_borrower.type = 'Book' "
		   "WHERE "
		   "book.myoid IN ");
  searchstr.append(in);
  searchstr.append(" AND ");
  searchstr.append("item_borrower.memberid = "
		   "member.memberid ");
  searchstr.append("GROUP BY "
		   "name, "
		   "member.memberid, "
		   "member.telephone_num, "
		   "item_borrower.copyid, "
		   "item_borrower.reserved_date, "
		   "item_borrower.duedate, "
		   "book.title, "
		   "book.id, "
		   "book.callnumber, "
		   "book.publisher, "
		   "book.pdate, "
		   "book.category, "
		   "book.language, "
		   "book.price, "
		   "book.monetary_units, "
		   "book.quantity, "
		   "book.location, "
		   "book.accession_number, "
		   "book.type, "
		   "book.myoid, "
		   "book.front_cover ");
  searchstr.append("UNION ALL ");
  searchstr.append("SELECT DISTINCT "
		   "member.last_name || ', ' || "
		   "member.first_name AS name, "
		   "member.memberid, "
		   "member.telephone_num, "
		   "item_borrower.copyid, "
		   "item_borrower.reserved_date, "
		   "item_borrower.duedate, "
		   "cd.title, "
		   "cd.id, "
		   "'' AS callnumber, "
		   "cd.recording_label, "
		   "cd.rdate, "
		   "cd.category, "
		   "cd.language, "
		   "cd.price, "
		   "cd.monetary_units, "
		   "cd.quantity, "
		   "cd.location, "
		   "cd.quantity - "
		   "COUNT(item_borrower.item_oid) "
		   "AS availability, "
		   "COUNT(item_borrower.item_oid) AS "
		   "total_reserved, "
		   "cd.accession_number, "
		   "cd.type, "
		   "cd.myoid, " +
		   cdFrontCover +
		   "FROM "
		   "member, "
		   "cd LEFT JOIN item_borrower ON "
		   "cd.myoid = item_borrower.item_oid "
		   "AND item_borrower.type = 'CD' "
		   "WHERE "
		   "cd.myoid IN ");
  searchstr.append(in);
  searchstr.append(" AND ");
  searchstr.append("item_borrower.memberid = "
		   "member.memberid ");
  searchstr.append("GROUP BY "
		   "name, "
		   "member.memberid, "
		   "member.telephone_num, "
		   "item_borrower.copyid, "
		   "item_borrower.reserved_date, "
		   "item_borrower.duedate, "
		   "cd.title, "
		   "cd.id, "
		   "callnumber, "
		   "cd.recording_label, "
		   "cd.rdate, "
		   "cd.category, "
		   "cd.language, "
		   "cd.price, "
		   "cd.monetary_units, "
		   "cd.quantity, "
		   "cd.location, "
		   "cd.accession_number, "
		   "cd.type, "
		   "cd.myoid, "
		   "cd.front_cover ");
  searchstr.append("UNION ALL ");
  searchstr.append("SELECT DISTINCT "
		   "member.last_name || ', ' || "
		   "member.first_name AS name, "
		   "member.memberid, "
		   "member.telephone_num, "
		   "item_borrower.copyid, "
		   "item_borrower.reserved_date, "
		   "item_borrower.duedate, "
		   "dvd.title, "
		   "dvd.id, "
		   "'' AS callnumber, "
		   "dvd.studio, "
		   "dvd.rdate, "
		   "dvd.category, "
		   "dvd.language, "
		   "dvd.price, "
		   "dvd.monetary_units, "
		   "dvd.quantity, "
		   "dvd.location, "
		   "dvd.quantity - "
		   "COUNT(item_borrower.item_oid) "
		   "AS availability, "
		   "COUNT(item_borrower.item_oid) AS "
		   "total_reserved, "
		   "dvd.accession_number, "
		   "dvd.type, "
		   "dvd.myoid, " +
		   dvdFrontCover +
		   "FROM "
		   "member, "
		   "dvd LEFT JOIN item_borrower ON "
		   "dvd.myoid = item_borrower.item_oid "
		   "AND item_borrower.type = 'DVD' "
		   "WHERE "
		   "dvd.myoid IN ");
  searchstr.append(in);
  searchstr.append(" AND ");
  searchstr.append("item_borrower.memberid = "
		   "member.memberid ");
  searchstr.append("GROUP BY "
		   "name, "
		   "member.memberid, "
		   "member.telephone_num, "
		   "item_borrower.copyid, "
		   "item_borrower.reserved_date, "
		   "item_borrower.duedate, "
		   "dvd.title, "
		   "dvd.id, "
		   "callnumber, "
		   "dvd.studio, "
		   "dvd.rdate, "
		   "dvd.category, "
		   "dvd.language, "
		   "dvd.price, "
		   "dvd.monetary_units, "
		   "dvd.quantity, "
		   "dvd.location, "
		   "dvd.accession_number, "
		   "dvd.type, "
		   "dvd.myoid, "
		   "dvd.front_cover ");
  searchstr.append("UNION ALL ");
  searchstr.append("SELECT DISTINCT "
		   "member.last_name || ', ' || "
		   "member.first_name AS name, "
		   "member.memberid, "
		   "member.telephone_num, "
		   "item_borrower.copyid, "
		   "item_borrower.reserved_date, "
		   "item_borrower.duedate, "
		   "grey_literature.document_title, "
		   "grey_literature.document_id, "
		   "'', "
		   "grey_literature.author, "
		   "grey_literature.document_date, "
		   "grey_literature.document_type, "
		   "'', "
		   "0.00, "
		   "'', "
		   "grey_literature.quantity, "
		   "grey_literature.location, "
		   "1 - COUNT(item_borrower.item_oid) "
		   "AS availability, "
		   "COUNT(item_borrower.item_oid) AS "
		   "total_reserved, "
		   "grey_literature.job_number, "
		   "grey_literature.type, "
		   "grey_literature.myoid, " +
		   greyLiteratureFrontCover +
		   "FROM "
		   "member, "
		   "grey_literature LEFT JOIN item_borrower ON "
		   "grey_literature.myoid = "
		   "item_borrower.item_oid "
		   "AND item_borrower.type = 'Grey Literature' "
		   "WHERE "
		   "grey_literature.myoid IN ");
  searchstr.append(in);
  searchstr.append(" AND ");
  searchstr.append("item_borrower.memberid = "
		   "member.memberid ");
  searchstr.append("GROUP BY "
		   "name, "
		   "member.memberid, "
		   "member.telephone_num, "
		   "item_borrower.copyid, "
		   "item_borrower.reserved_date, "
		   "item_borrower.duedate, "
		   "grey_literature.document_title, "
		   "grey_literature.document_id, "
		   "grey_literature.author, "
		   "grey_literature.document_date, "
		   "grey_literature.document_type, "
		   "grey_literature.quantity, "
		   "grey_literature.location, "
		   "grey_literature.job_number, "
		   "grey_literature.type, "
		   "grey_literature.myoid, "
		   "grey_literature.front_cover ");
  searchstr.append("UNION ALL ");
  searchstr.append("SELECT DISTINCT "
		   "member.last_name || ', ' || "
		   "member.first_name AS name, "
		   "member.memberid, "
		   "member.telephone_num, "
		   "item_borrower.copyid, "
		   "item_borrower.reserved_date, "
		   "item_borrower.duedate, "
		   "journal.title, "
		   "journal.id, "
		   "journal.callnumber, "
		   "journal.publisher, "
		   "journal.pdate, "
		   "journal.category, "
		   "journal.language, "
		   "journal.price, "
		   "journal.monetary_units, "
		   "journal.quantity, "
		   "journal.location, "
		   "journal.quantity - "
		   "COUNT(item_borrower.item_oid) "
		   "AS availability, "
		   "COUNT(item_borrower.item_oid) AS "
		   "total_reserved, "
		   "journal.accession_number, "
		   "journal.type, "
		   "journal.myoid, " +
		   journalFrontCover +
		   "FROM "
		   "member, "
		   "journal LEFT JOIN item_borrower ON "
		   "journal.myoid = "
		   "item_borrower.item_oid "
		   "AND item_borrower.type = journal.type "
		   "WHERE "
		   "journal.myoid IN ");
  searchstr.append(in);
  searchstr.append(" AND ");
  searchstr.append("item_borrower.memberid = "
		   "member.memberid ");
  searchstr.append("GROUP BY "
		   "name, "
		   "member.memberid, "
		   "member.telephone_num, "
		   "item_borrower.copyid, "
		   "item_borrower.reserved_date, "
		   "item_borrower.duedate, "
		   "journal.title, "
		   "journal.id, "
		   "journal.callnumber, "
		   "journal.publisher, "
		   "journal.pdate, "
		   "journal.category, "
		   "journal.language, "
		   "journal.price, "
		   "journal.monetary_units, "
		   "journal.quantity, "
		   "journal.location, "
		   "journal.accession_number, "
		   "journal.type, "
		   "journal.myoid, "
		   "journal.front_cover ");
  searchstr.append("UNION ALL ");
  searchstr.append("SELECT DISTINCT "
		   "member.last_name || ', ' || "
		   "member.first_name AS name, "
		   "member.memberid, "
		   "member.telephone_num, "
		   "item_borrower.copyid, "
		   "item_borrower.reserved_date, "
		   "item_borrower.duedate, "
		   "magazine.title, "
		   "magazine.id, "
		   "magazine.callnumber, "
		   "magazine.publisher, "
		   "magazine.pdate, "
		   "magazine.category, "
		   "magazine.language, "
		   "magazine.price, "
		   "magazine.monetary_units, "
		   "magazine.quantity, "
		   "magazine.location, "
		   "magazine.quantity - "
		   "COUNT(item_borrower.item_oid) "
		   "AS availability, "
		   "COUNT(item_borrower.item_oid) AS "
		   "total_reserved, "
		   "magazine.accession_number, "
		   "magazine.type, "
		   "magazine.myoid, " +
		   magazineFrontCover +
		   "FROM "
		   "member, "
		   "magazine LEFT JOIN item_borrower ON "
		   "magazine.myoid = "
		   "item_borrower.item_oid "
		   "AND item_borrower.type = magazine.type "
		   "WHERE "
		   "magazine.myoid IN ");
  searchstr.append(in);
  searchstr.append(" AND ");
  searchstr.append("item_borrower.memberid = "
		   "member.memberid ");
  searchstr.append("GROUP BY "
		   "name, "
		   "member.memberid, "
		   "member.telephone_num, "
		   "item_borrower.copyid, "
		   "item_borrower.reserved_date, "
		   "item_borrower.duedate, "
		   "magazine.title, "
		   "magazine.id, "
		   "magazine.callnumber, "
		   "magazine.publisher, "
		   "magazine.pdate, "
		   "magazine.category, "
		   "magazine.language, "
		   "magazine.price, "
		   "magazine.monetary_units, "
		   "magazine.quantity, "
		   "magazine.location, "
		   "magazine.accession_number, "
		   "magazine.type, "
		   "magazine.myoid, "
		   "magazine.front_cover ");
  searchstr.append("UNION ALL ");
  searchstr.append("SELECT DISTINCT "
		   "member.last_name || ', ' || "
		   "member.first_name AS name, "
		   "member.memberid, "
		   "member.telephone_num, "
		   "item_borrower.copyid, "
		   "item_borrower.reserved_date, "
		   "item_borrower.duedate, "
		   "videogame.title, "
		   "videogame.id, "
		   "'' AS callnumber, "
		   "videogame.publisher, "
		   "videogame.rdate, "
		   "videogame.genre, "
		   "videogame.language, "
		   "videogame.price, "
		   "videogame.monetary_units, "
		   "videogame.quantity, "
		   "videogame.location, "
		   "videogame.quantity - "
		   "COUNT(item_borrower.item_oid) "
		   "AS availability, "
		   "COUNT(item_borrower.item_oid) AS "
		   "total_reserved, "
		   "videogame.accession_number, "
		   "videogame.type, "
		   "videogame.myoid, " +
		   videoGameFrontCover +
		   "FROM "
		   "member, "
		   "videogame LEFT JOIN item_borrower ON "
		   "videogame.myoid = "
		   "item_borrower.item_oid "
		   "AND item_borrower.type = 'Video Game' "
		   "WHERE "
		   "videogame.myoid IN ");
  searchstr.append(in);
  searchstr.append(" AND ");
  searchstr.append("item_borrower.memberid = "
		   "member.memberid ");
  searchstr.append("GROUP BY "
		   "name, "
		   "member.memberid, "
		   "member.telephone_num, "
		   "item_borrower.copyid, "
		   "item_borrower.reserved_date, "
		   "item_borrower.duedate, "
		   "videogame.title, "
		   "videogame.id, "
		   "callnumber, "
		   "videogame.publisher, "
		   "videogame.rdate, "
		   "videogame.genre, "
		   "videogame.language, "
		   "videogame.price, "
		   "videogame.monetary_units, "
		   "videogame.quantity, "
		   "videogame.location, "
		   "videogame.accession_number, "
		   "videogame.type, "
		   "videogame.myoid, "
		   "videogame.front_cover ");
  searchstr.append("ORDER BY 1");
  query->prepare(searchstr);

  for(int i = 0; i < searchstr.count("WHERE"); i++) // Seven tables.
    {
      QSetIterator<QString> it(values);

      while(it.hasNext())
	query->addBindValue(it.next());
    }

  QApplication::restoreOverrideCursor();
  (void) m_qmain->populateTable
    (query, "All Reserved", biblioteq::NEW_PAGE, biblioteq::POPULATE_SEARCH);
}

void biblioteq_batch_activities::show(QMainWindow *parent, const bool center)
{
  Q_UNUSED(center);
#ifdef Q_OS_ANDROID
  Q_UNUSED(parent);
  showMaximized();
#else
  restoreGeometry
    (QSettings().value("batch_activities_geometry").toByteArray());
  biblioteq_misc_functions::center(this, parent, false);
  showNormal();
#endif
  activateWindow();
  raise();
  m_ui.borrow_member_id->setFocus();

  if(!m_memberIdCompleter)
    m_memberIdCompleter = new QCompleter(this);

  if(!m_memberIdModel)
    m_memberIdModel = new QSqlQueryModel(this);

  if(m_memberIdCompleter && m_memberIdModel)
    {
      m_memberIdCompleter->setCaseSensitivity(Qt::CaseInsensitive);
      m_memberIdCompleter->setCompletionColumn(0);
      m_memberIdCompleter->setCompletionMode(QCompleter::InlineCompletion);
      m_memberIdCompleter->setFilterMode(Qt::MatchContains);
      m_memberIdCompleter->setModel(m_memberIdModel);

      if(!m_ui.borrow_member_id->completer())
	m_ui.borrow_member_id->setCompleter(m_memberIdCompleter);

      if(!m_ui.dreamy_member_id->completer())
	m_ui.dreamy_member_id->setCompleter(m_memberIdCompleter);
    }

  m_ui.dreamy_date->setDate(QDate::currentDate());
}

void biblioteq_batch_activities::slotAddBorrowingRow(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  disconnect(m_ui.borrow_table,
	     SIGNAL(itemChanged(QTableWidgetItem *)),
	     this,
	     SLOT(slotBorrowItemChanged(QTableWidgetItem *)));
  m_ui.borrow_table->setRowCount(m_ui.borrow_table->rowCount() + 1);

  if(m_ui.bottom_scroll_on_add->isChecked())
    m_ui.borrow_table->scrollToBottom();

  auto const row = m_ui.borrow_table->rowCount() - 1;

  for(int i = 0; i < m_ui.borrow_table->columnCount(); i++)
    if(i == static_cast<int> (BorrowTableColumns::CATEGORY_COLUMN))
      {
	QStringList list;
	auto comboBox = new QComboBox();
	auto widget = new QWidget();

	list << tr("Book")
	     << tr("CD")
	     << tr("DVD")
	     << tr("Grey Literature")
	     << tr("Journal")
	     << tr("Magazine")
	     << tr("Video Game");
	std::sort(list.begin(), list.end());
	comboBox->addItems(list);
	comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

	auto layout = new QHBoxLayout(widget);
	auto spacer = new QSpacerItem
	  (40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);

	layout->addWidget(comboBox);
	layout->addSpacerItem(spacer);
	layout->setContentsMargins(0, 0, 0, 0);
	m_ui.borrow_table->setCellWidget(row, i, widget);
      }
    else
      {
	auto item = new QTableWidgetItem();

	if(i == static_cast<int> (BorrowTableColumns::FIELD_COLUMN) ||
	   i == static_cast<int> (BorrowTableColumns::RESULTS_COLUMN))
	  item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	else
	  item->setFlags
	    (Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	m_ui.borrow_table->setItem(row, i, item);
      }

  for(int i = 0; i < m_ui.borrow_table->columnCount() - 1; i++)
    m_ui.borrow_table->resizeColumnToContents(i);

  m_ui.borrow_table->resizeRowsToContents();
  connect(m_ui.borrow_table,
	  SIGNAL(itemChanged(QTableWidgetItem *)),
	  this,
	  SLOT(slotBorrowItemChanged(QTableWidgetItem *)));
  QApplication::restoreOverrideCursor();
}

void biblioteq_batch_activities::slotAudioEnabled(void)
{
  QSettings().setValue
    ("otheroptions/batch_activities_audio", m_ui.audio->isChecked());
}

void biblioteq_batch_activities::slotBorrowItemChanged(QTableWidgetItem *item)
{
  if(!item)
    return;

  if(item->column() ==
     static_cast<int> (BorrowTableColumns::COPY_IDENTIFIER_COLUMN))
    {
      m_ui.borrow_table->blockSignals(true);
      item->setData(Qt::BackgroundRole, QVariant());
      m_ui.borrow_table->blockSignals(false);
    }
}

void biblioteq_batch_activities::slotClose(void)
{
#ifdef Q_OS_ANDROID
  hide();
#else
  close();
#endif
}

void biblioteq_batch_activities::slotDeleteBorrowingRow(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  auto const rows(biblioteq_misc_functions::selectedRows(m_ui.borrow_table));

  for(auto i = rows.size() - 1; i >= 0; i--)
    m_ui.borrow_table->removeRow(rows.at(i));

  QApplication::restoreOverrideCursor();
}

void biblioteq_batch_activities::slotDiscoverDreamy(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_ui.dreamy_table->setRowCount(0);

  QSqlQuery query(m_qmain->getDB());
  QString search("");
  auto const ids(QStringList()
		  << "id"
		  << "id"
		  << "id"
		  << "document_id"
		  << "id"
		  << "id"
		  << "id");
  auto const list(QStringList()
		  << "book"
		  << "cd"
		  << "dvd"
		  << "grey_literature"
		  << "journal"
		  << "magazine"
		  << "videogame");
  auto const text(m_ui.dreamy_member_id->text().trimmed());
  auto const titles(QStringList()
		  << "title"
		  << "title"
		  << "title"
		  << "document_title"
		  << "title"
		  << "title"
		  << "title");
  auto const types(QStringList()
		  << "Book"
		  << "CD"
		  << "DVD"
		  << "Grey Literature"
		  << "Journal"
		  << "Magazine"
		  << "Video Game");

  query.setForwardOnly(true);

  for(int i = 0; i < list.size(); i++)
    {
      search.append
	(QString("SELECT DISTINCT "
		 "item_borrower.duedate, "
		 "item_borrower.reserved_date, "
		 "%1.%2, "
		 "%1.%3, "
		 "member.memberid, "
		 "member.last_name || ', ' || member.first_name AS name, "
		 "item_borrower.reserved_by, "
		 "%1.type, "
		 "item_borrower.myoid "
		 "FROM "
		 "member, "
		 "%1 LEFT JOIN item_borrower ON "
		 "%1.myoid = item_borrower.item_oid AND "
		 "item_borrower.type = '%4' "
		 "WHERE "
		 "item_borrower.memberid LIKE ? AND "
		 "item_borrower.memberid = member.memberid "
		 "GROUP BY "
		 "item_borrower.duedate, "
		 "item_borrower.reserved_date, "
		 "%1.%2, "
		 "%1.%3, "
		 "member.memberid, "
		 "name, "
		 "item_borrower.reserved_by, "
		 "%1.type, "
		 "item_borrower.myoid ").
	 arg(list.at(i)).arg(ids.at(i)).arg(titles.at(i)).arg(types.at(i)));

      if(i != list.size() - 1)
	search.append("UNION ALL ");
    }

  query.prepare(search);

  for(int i = 0; i < list.size(); i++)
    if(text.isEmpty())
      query.addBindValue("%");
    else
      query.addBindValue(text);

  if(query.exec())
    {
      m_ui.dreamy_table->setSortingEnabled(false);

      while(query.next())
	{
	  m_ui.dreamy_table->setRowCount(m_ui.dreamy_table->rowCount() + 1);

	  auto const record(query.record());

	  for(int i = 0; i < record.count(); i++)
	    {
	      auto item = new QTableWidgetItem();
	      auto text(record.field(i).value().toString().trimmed());

	      if(i == static_cast<int> (DreamyTableColumns::NEW_RETURN_DATE) ||
		 i == static_cast<int> (DreamyTableColumns::RESERVED_DATE))
		{
		  item->setFlags(Qt::ItemIsEditable |
				 Qt::ItemIsEnabled |
				 Qt::ItemIsSelectable);
		  text = QLocale().toString
		    (QDate::fromString(text, biblioteq::s_databaseDateFormat),
		     QLocale::ShortFormat);
		}
	      else
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	      item->setText(text);
	      m_ui.dreamy_table->setItem
		(m_ui.dreamy_table->rowCount() - 1, i, item);
	    }
	}

      m_ui.dreamy_table->setSortingEnabled(true);
    }

  QApplication::restoreOverrideCursor();
}

void biblioteq_batch_activities::slotDiscoverMemberName(void)
{
  if(m_ui.borrow_member_id == sender() || m_ui.dreamy_member_id == sender())
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      QString errorstr("");
      auto const name
	(biblioteq_misc_functions::
	 getMemberName(m_qmain->getDB(),
		       qobject_cast<QLineEdit *> (sender())->text(),
		       errorstr));
      auto widget = m_ui.borrow_member_id == sender() ?
	m_ui.borrow_member_name : m_ui.dreamy_member_name;

      widget->setText(name);
      widget->setCursorPosition(0);
      QApplication::restoreOverrideCursor();
    }
}

void biblioteq_batch_activities::slotExportMissing(void)
{
  if(m_ui.discover_table->rowCount() == 0)
    return;

  QFileDialog dialog(this);

  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setDefaultSuffix("csv");
  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setNameFilter(tr("CSV (*.csv)"));
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowTitle(tr("BiblioteQ: Export As CSV"));
  dialog.exec();
  QApplication::processEvents();

  if(dialog.result() == QDialog::Accepted)
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      QFile file(dialog.selectedFiles().value(0));

      if(file.open(QIODevice::Text |
		   QIODevice::Truncate |
		   QIODevice::WriteOnly))
	{
	  QString str("");
	  QTextStream stream(&file);
	  auto const i = static_cast<int>
	    (DiscoverTableColumns::IDENTIFIER_COLUMN);

	  if(m_ui.discover_table->horizontalHeaderItem(i))
	    {
	      if(m_ui.discover_table->horizontalHeaderItem(i)->text().
		 contains(","))
		str += QString("\"%1\"").arg
		  (m_ui.discover_table->horizontalHeaderItem(i)->text());
	      else
		str += QString("%1").arg
		  (m_ui.discover_table->horizontalHeaderItem(i)->text());
	    }

	  if(!str.isEmpty())
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
	    stream << str << Qt::endl;
#else
	    stream << str << endl;
#endif

	  for(int i = 0; i < m_ui.discover_table->rowCount(); i++)
	    {
	      str = "";

	      if(m_ui.discover_table->
		 item(i,
		      static_cast<int> (DiscoverTableColumns::
					CATEGORY_COLUMN)) &&
		 m_ui.discover_table->
		 item(i,
		      static_cast<int> (DiscoverTableColumns::
					CATEGORY_COLUMN))->text().length() > 0)
		continue;

	      auto const c = static_cast<int>
		(DiscoverTableColumns::IDENTIFIER_COLUMN);

	      if(!m_ui.discover_table->item(i, c))
		continue;

	      auto cleaned(m_ui.discover_table->item(i, c)->text());

	      cleaned.replace("\n", " ");
	      cleaned.replace("\r\n", " ");
	      cleaned = cleaned.trimmed();

	      if(cleaned.contains(","))
		str += QString("\"%1\"").arg(cleaned);
	      else
		str += QString("%1").arg(cleaned);

	      if(!str.isEmpty())
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
		stream << str << Qt::endl;
#else
	        stream << str << endl;
#endif
	    }

	  file.close();
	}

      QApplication::restoreOverrideCursor();
    }
}

void biblioteq_batch_activities::slotGo(void)
{
  switch(static_cast<Pages> (m_ui.tab->currentIndex()))
    {
    case Pages::Borrow:
      {
	borrow();
	break;
      }
    case Pages::Discover:
      {
	discover();
	break;
      }
    case Pages::Return:
      {
	returnItems();
	break;
      }
    default:
      {
	break;
      }
    }
}

void biblioteq_batch_activities::slotListDiscoveredItems(void)
{
  if(m_ui.discover_table->rowCount() == 0)
    return;

  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  QSet<QString> contains;
  QString bookFrontCover("'' AS front_cover ");
  QString cdFrontCover("'' AS front_cover ");
  QString dvdFrontCover("'' AS front_cover ");
  QString greyLiteratureFrontCover("'' AS front_cover ");
  QString in(action->property("in-or-not-in").toString());
  QString journalFrontCover("'' AS front_cover ");
  QString magazineFrontCover("'' AS front_cover ");
  QString photographCollectionFrontCover("'' AS image_scaled ");
  QString searchstr("");
  QString videoGameFrontCover("'' AS front_cover ");
  QString where("(");
  QStringList ids;
  QStringList types;
  QStringList values;
  auto query = new QSqlQuery(m_qmain->getDB());

  if(m_qmain->showMainTableImages())
    {
      bookFrontCover = "book.front_cover ";
      cdFrontCover = "cd.front_cover ";
      dvdFrontCover = "dvd.front_cover ";
      greyLiteratureFrontCover = "grey_literature.front_cover ";
      journalFrontCover = "journal.front_cover ";
      magazineFrontCover = "magazine.front_cover ";
      photographCollectionFrontCover = "photograph_collection.image_scaled ";
      videoGameFrontCover = "videogame.front_cover ";
    }

  types.append("Book");
  types.append("CD");
  types.append("DVD");
  types.append("Grey Literature");
  types.append("Journal");
  types.append("Magazine");
  types.append("Photograph Collection");
  types.append("Video Game");

  for(int i = 0; i < m_ui.discover_table->rowCount(); i++)
    {
      auto item = m_ui.discover_table->item
	(i, static_cast<int> (DiscoverTableColumns::IDENTIFIER_COLUMN));

      if(!item || contains.contains(item->text()))
	continue;
      else
	contains << item->text();

      ids << item->text();
      where.append("?");
      where.append(", ");
    }

  values << ids; // book.accession_number

  for(int i = 0; i < ids.size(); i++)
    values << QString(ids.at(i)).remove('-'); // book.id

  for(int i = 0; i < ids.size(); i++)
    values << QString(ids.at(i)).remove('-'); // book.isbn13

  for(int i = 0; i < types.size() - 1; i++)
    values << ids; // type.id

  where = where.mid(0, where.length() - 2); // Remove the last two characters.
  where.append(")");

  for(int i = 0; i < types.size(); i++)
    {
      QString str("");
      auto const type(types.at(i));

      if(type == "Grey Literature")
	{
	  str = "SELECT DISTINCT grey_literature.document_title, "
	    "grey_literature.document_id, "
	    "'', "
	    "grey_literature.document_date, "
	    "'', "
	    "'', "
	    "0.00, "
	    "'', "
	    "grey_literature.quantity, "
	    "grey_literature.location, "
	    "1 - COUNT(item_borrower.item_oid) AS availability, "
	    "COUNT(item_borrower.item_oid) AS total_reserved, "
	    "grey_literature.job_number, "
	    "grey_literature.type, "
	    "grey_literature.myoid, " +
	    greyLiteratureFrontCover +
	    "FROM "
	    "grey_literature LEFT JOIN item_borrower ON "
	    "grey_literature.myoid = "
	    "item_borrower.item_oid "
	    "AND item_borrower.type = 'Grey Literature' "
	    "WHERE grey_literature.document_id";
	  str.append(in);
	  str.append(where);
	}
      else if(type == "Photograph Collection")
	{
	  str = "SELECT DISTINCT photograph_collection.title, "
	    "photograph_collection.id, "
	    "'', "
	    "'', "
	    "'', "
	    "'', "
	    "0.00, "
	    "'', "
	    "1 AS quantity, "
	    "photograph_collection.location, "
	    "0 AS availability, "
	    "0 AS total_reserved, "
	    "photograph_collection.accession_number, "
	    "photograph_collection.type, "
	    "photograph_collection.myoid, " +
	    photographCollectionFrontCover +
	    "FROM photograph_collection "
	    "WHERE photograph_collection.id";
	  str.append(in);
	  str.append(where);
	}
      else
	{
	  str = QString
	    ("SELECT DISTINCT %1.title, "
	     "%1.id, "
	     "%1.publisher, %1.pdate, "
	     "%1.category, "
	     "%1.language, "
	     "%1.price, %1.monetary_units, "
	     "%1.quantity, "
	     "%1.location, "
	     "%1.quantity - "
	     "COUNT(item_borrower.item_oid) AS availability, "
	     "COUNT(item_borrower.item_oid) AS total_reserved, "
	     "%1.accession_number, "
	     "%1.type, "
	     "%1.myoid, ").arg(type.toLower().remove(" "));

	  if(type == "Book")
	    str.append(bookFrontCover);
	  else if(type == "CD")
	    str.append(cdFrontCover);
	  else if(type == "DVD")
	    str.append(dvdFrontCover);
	  else if(type == "Journal")
	    str.append(journalFrontCover);
	  else if(type == "Magazine")
	    str.append(magazineFrontCover);
	  else
	    str.append(videoGameFrontCover);

	  str += QString("FROM "
			 "%1 LEFT JOIN item_borrower ON "
			 "%1.myoid = "
			 "item_borrower.item_oid "
			 "AND item_borrower.type = '%2' "
			 "WHERE ").arg(type.toLower().remove(" ")).arg(type);

	  if(type == "Book")
	    {
	      str.append("COALESCE(book.accession_number, '')");
	      str.append(in);
	      str.append(where);

	      if(in.contains("NOT"))
		str.append(" AND ");
	      else
		str.append(" OR ");

	      str.append("COALESCE(book.id, '')");
	      str.append(in);
	      str.append(where);

	      if(in.contains("NOT"))
		str.append(" AND ");
	      else
		str.append(" OR ");

	      str.append("COALESCE(book.isbn13, '')");
	      str.append(in);
	      str.append(where);
	    }
	  else
	    str.append(QString("%1.id %2 %3").
		       arg(type.toLower().remove(" ")).
		       arg(in).
		       arg(where));

	  str.append(" ");
	}

      if(type != "Grey Literature" && type != "Photograph Collection")
	{
	  str += QString("GROUP BY "
			 "%1.title, "
			 "%1.id, "
			 "%1.publisher, %1.pdate, "
			 "%1.category, "
			 "%1.language, "
			 "%1.price, "
			 "%1.monetary_units, "
			 "%1.quantity, "
			 "%1.location, "
			 "%1.keyword, "
			 "%1.accession_number, "
			 "%1.type, "
			 "%1.myoid, "
			 "%1.front_cover ").arg(type.toLower().remove(" "));
	}
      else if(type == "Grey Literature")
	{
	  str += "GROUP BY grey_literature.document_title, "
	    "grey_literature.document_id, "
	    "grey_literature.document_date, "
	    "grey_literature.location, "
	    "grey_literature.job_number, "
	    "grey_literature.type, "
	    "grey_literature.myoid, "
	    "grey_literature.front_cover ";
	}
      else if(type == "Photograph Collection")
	{
	  str += "GROUP BY "
	    "photograph_collection.title, "
	    "photograph_collection.id, "
	    "photograph_collection.location, "
	    "photograph_collection.accession_number, "
	    "photograph_collection.type, "
	    "photograph_collection.myoid, "
	    "photograph_collection.image_scaled ";
	}

      if(type == "CD")
	{
	  str = str.replace("pdate", "rdate");
	  str = str.replace("publisher", "recording_label");
	}
      else if(type == "DVD")
	{
	  str = str.replace("pdate", "rdate");
	  str = str.replace("publisher", "studio");
	}
      else if(type == "Video Game")
	{
	  str = str.replace("pdate", "rdate");
	  str = str.replace("category", "genre");
	}

      if(type != "Video Game")
	str += "UNION ALL ";
      else
	str += " ";

      searchstr += str;
    }

  query->prepare(searchstr);

  for(int i = 0; i < values.size(); i++)
    query->addBindValue(values.at(i));

  QApplication::restoreOverrideCursor();
  (void) m_qmain->populateTable
    (query, "All", biblioteq::NEW_PAGE, biblioteq::POPULATE_SEARCH);
  show(m_qmain, false);
}

void biblioteq_batch_activities::slotListMembersReservedItems(void)
{
  emit listMembersReservedItems(m_ui.borrow_member_id->text());
  show(m_qmain, false);
}

#ifdef BIBLIOTEQ_AUDIO_SUPPORTED
void biblioteq_batch_activities::slotMediaError(QMediaPlayer::Error error)
{
  auto player = qobject_cast<QMediaPlayer *> (sender());

  if(!player)
    return;

  if(error != QMediaPlayer::NoError)
    player->deleteLater();
}

void biblioteq_batch_activities::slotMediaError
(QMediaPlayer::Error error, const QString &errorString)
{
  Q_UNUSED(errorString);

  auto player = qobject_cast<QMediaPlayer *> (sender());

  if(!player)
    return;

  if(error != QMediaPlayer::NoError)
    player->deleteLater();
}

void biblioteq_batch_activities::slotMediaStatusChanged
(QMediaPlayer::MediaStatus status)
{
  auto player = qobject_cast<QMediaPlayer *> (sender());

  if(!player)
    return;

  if(status == QMediaPlayer::EndOfMedia)
    player->deleteLater();
}
#endif

void biblioteq_batch_activities::slotMemberIdEdited(const QString &text)
{
  if(m_memberIdModel == nullptr || text.trimmed().isEmpty())
    return;

  QSqlQuery query(m_qmain->getDB());
  QString E("");

  if(m_qmain->getDB().driverName() != "QSQLITE")
    E = "E";

  query.prepare
    ("SELECT memberid FROM member WHERE "
     "LOWER(memberid) LIKE " + E + "'%' || ? || '%'");
  query.addBindValue(text.toLower().trimmed());
  query.exec();
  m_memberIdModel->clear();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
  m_memberIdModel->setQuery(std::move(query));
#else
  m_memberIdModel->setQuery(query);
#endif
}

void biblioteq_batch_activities::slotPageIndexChanged(int index)
{
  QSettings().setValue("otheroptions/batch_activities_page_index", index);
  m_ui.go->setEnabled(index != static_cast<int> (Pages::Discover));
}

void biblioteq_batch_activities::slotReset(void)
{
  /*
  ** If sender() is null, reset all fields.
  */

  if(!sender() || m_ui.tab->currentIndex() == static_cast<int> (Pages::Borrow))
    {
      if(m_ui.borrow_table->rowCount() > 0 && sender())
	if(QMessageBox::question(this,
				 tr("BiblioteQ: Question"),
				 tr("Are you sure that you wish to reset?"),
				 QMessageBox::No | QMessageBox::Yes,
				 QMessageBox::No) == QMessageBox::No)
	  {
	    QApplication::processEvents();
	    return;
	  }

      m_ui.borrow_member_id->clear();
      m_ui.borrow_member_id->setFocus();
      m_ui.borrow_member_name->clear();
      m_ui.borrow_scan->clear();
      m_ui.borrow_scan_type->setCurrentIndex(0);
      m_ui.borrow_table->clearContents();
      m_ui.borrow_table->setRowCount(0);
    }

  if(!sender() ||
     m_ui.tab->currentIndex() == static_cast<int> (Pages::Discover))
    {
      m_ui.discover_scan->clear();
      m_ui.discover_scan->setFocus();
      m_ui.discover_table->clearContents();
      m_ui.discover_table->setRowCount(0);
    }

  if(!sender() ||
     m_ui.tab->currentIndex() == static_cast<int> (Pages::DreamyExtensions))
    {
      if(m_ui.dreamy_table->rowCount() > 0 && sender())
	if(QMessageBox::question(this,
				 tr("BiblioteQ: Question"),
				 tr("Are you sure that you wish to reset?"),
				 QMessageBox::No | QMessageBox::Yes,
				 QMessageBox::No) == QMessageBox::No)
	  {
	    QApplication::processEvents();
	    return;
	  }

      m_ui.dreamy_date->setDate(QDate::currentDate());
      m_ui.dreamy_member_id->clear();
      m_ui.dreamy_member_id->setFocus();
      m_ui.dreamy_member_name->clear();
      m_ui.dreamy_table->clearContents();
      m_ui.dreamy_table->setRowCount(0);
    }

  if(!sender() ||
     m_ui.tab->currentIndex() == static_cast<int> (Pages::Return))
    {
      m_ui.return_scan->clear();
      m_ui.return_scan->setFocus();
      m_ui.return_table->clearContents();
      m_ui.return_table->setRowCount(0);
    }
}

void biblioteq_batch_activities::slotScanBorrowingTimerTimeout(void)
{
  if(!m_ui.borrow_scan->text().trimmed().isEmpty())
    {
      slotAddBorrowingRow();

      QString field("");
      auto const copyIdentifier = m_ui.borrow_table->item
	(m_ui.borrow_table->rowCount() - 1,
	 static_cast<int> (BorrowTableColumns::COPY_IDENTIFIER_COLUMN));

      if(copyIdentifier)
	{
	  QString type("");

	  if(m_ui.borrow_scan_type->currentText() == tr("Book"))
	    type = "Book";
	  else if(m_ui.borrow_scan_type->currentText() == tr("CD"))
	    type = "CD";
	  else if(m_ui.borrow_scan_type->currentText() == tr("DVD"))
	    type = "DVD";
	  else if(m_ui.borrow_scan_type->currentText() == tr("Grey Literature"))
	    type = "Grey Literature";
	  else if(m_ui.borrow_scan_type->currentText() == tr("Journal"))
	    type = "Journal";
	  else if(m_ui.borrow_scan_type->currentText() == tr("Magazine"))
	    type = "Magazine";
	  else if(m_ui.borrow_scan_type->currentText() == tr("Video Game"))
	    type = "Video Game";

	  auto ok = true;
	  auto const str(biblioteq_misc_functions::
			 getNextCopy(field,
				     ok,
				     m_qmain->getDB(),
				     m_ui.borrow_scan->text(),
				     type).trimmed());

	  m_ui.borrow_table->blockSignals(true);

	  if(ok && str.length() > 0)
	    {
	      copyIdentifier->setText(str);
	      copyIdentifier->setToolTip(copyIdentifier->text());
	      play("qrc:/discovered.wav");
	    }
	  else
	    {
	      copyIdentifier->setBackground(s_notSoOkColor);

	      if(str.isEmpty())
		copyIdentifier->setText(tr("A copy is not available."));
	      else
		copyIdentifier->setText
		  (tr("A copy is not available (%1).").arg(str));

	      copyIdentifier->setToolTip(copyIdentifier->text());
	      play("qrc:/error.wav");
	    }

	  m_ui.borrow_table->blockSignals(false);
	}

      auto fieldItem = m_ui.borrow_table->item
	(m_ui.borrow_table->rowCount() - 1,
	 static_cast<int> (BorrowTableColumns::FIELD_COLUMN));

      if(fieldItem)
	{
	  fieldItem->setText(field);
	  fieldItem->setToolTip(fieldItem->text());
	}

      auto identifier = m_ui.borrow_table->item
	(m_ui.borrow_table->rowCount() - 1,
	 static_cast<int> (BorrowTableColumns::IDENTIFIER_COLUMN));

      if(identifier)
	{
	  identifier->setText(m_ui.borrow_scan->text().trimmed());
	  identifier->setToolTip(identifier->text());
	}

      /*
      ** Discover the desired category. Unpleasant, yet again!
      */

      auto widget = m_ui.borrow_table->cellWidget
	(m_ui.borrow_table->rowCount() - 1,
	 static_cast<int> (BorrowTableColumns::CATEGORY_COLUMN));

      if(widget)
	{
	  auto comboBox = widget->findChild<QComboBox *> ();

	  if(comboBox)
	    {
	      comboBox->setCurrentIndex
		(comboBox->findText(m_ui.borrow_scan_type->currentText()));

	      if(comboBox->currentIndex() < 0)
		comboBox->setCurrentIndex(0);
	    }
	}
    }

  m_ui.borrow_scan->clear();
}

void biblioteq_batch_activities::slotScanDiscoverTimerTimeout(void)
{
  if(m_ui.discover_scan->text().trimmed().isEmpty())
    return;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  auto const id(m_ui.discover_scan->text().trimmed());

  for(int i = 0; i < m_ui.discover_table->rowCount(); i++)
    {
      auto item = m_ui.discover_table->item
	(i, static_cast<int> (DiscoverTableColumns::IDENTIFIER_COLUMN));

      if((item ? item->text() : "") == id)
	{
	  QApplication::restoreOverrideCursor();
	  return;
	}
    }

  QHash<QString, QString> hash;
  auto item = new QTableWidgetItem(m_ui.discover_scan->text().trimmed());

  item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  item->setToolTip(item->text());
  m_ui.discover_table->setRowCount(m_ui.discover_table->rowCount() + 1);

  if(m_ui.bottom_scroll_on_add->isChecked())
    m_ui.discover_table->scrollToBottom();

  m_ui.discover_table->setItem
    (m_ui.discover_table->rowCount() - 1,
     static_cast<int> (DiscoverTableColumns::IDENTIFIER_COLUMN),
     item);
  item = new QTableWidgetItem();
  item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  biblioteq_misc_functions::categories
    (hash, m_qmain->getDB(), m_ui.discover_scan->text());
  item->setText(hash.value("categories").trimmed());
  item->setToolTip(item->text());

  if(!item->text().isEmpty())
    play("qrc:/discovered.wav");
  else
    play("qrc:/error.wav");

  m_ui.discover_table->setItem
    (m_ui.discover_table->rowCount() - 1,
     static_cast<int> (DiscoverTableColumns::CATEGORY_COLUMN),
     item);
  item = new QTableWidgetItem();
  item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  item->setText(hash.value("title").trimmed());
  item->setToolTip(item->text());
  m_ui.discover_table->setItem
    (m_ui.discover_table->rowCount() - 1,
     static_cast<int> (DiscoverTableColumns::TITLE_COLUMN),
     item);
  m_ui.discover_scan->clear();
  m_ui.discover_scan->setFocus();
  QApplication::restoreOverrideCursor();
}

void biblioteq_batch_activities::slotScanReturnTimerTimeout(void)
{
  if(m_ui.return_scan->text().trimmed().isEmpty())
    return;

  if(m_ui.return_table->columnCount() == 0)
    {
      m_ui.return_table->setColumnCount(6);
      m_ui.return_table->setHorizontalHeaderLabels
	(QStringList()
	 << tr("Copy ID")
	 << tr("Member ID")
	 << tr("Member Information")
	 << tr("Reservation Date")
	 << tr("Due Date")
	 << tr("Item OID"));
    }

  QSqlQuery query(m_qmain->getDB());
  auto const id(m_ui.return_scan->text().remove('-').trimmed());

  query.setForwardOnly(true);
  query.prepare("SELECT "
		"item_borrower.copyid, "
		"item_borrower.memberid, "
		"member.last_name || ', ' || member.first_name, "
		"item_borrower.reserved_date, "
		"item_borrower.duedate, "
		"item_borrower.item_oid "
		"FROM item_borrower, member "
		"WHERE "
		"item_borrower.item_oid IN "
		"(SELECT myoid FROM book WHERE "
		"REPLACE(accession_number, '-', '') = ? OR "
		"id = ? OR "
		"isbn13 = ? UNION ALL "
		"SELECT myoid FROM cd WHERE id = ? UNION ALL "
		"SELECT myoid FROM dvd WHERE id = ? UNION ALL "
		"SELECT myoid FROM grey_literature WHERE document_id = ? "
		"UNION ALL "
		"SELECT myoid FROM journal WHERE id = ? UNION ALL "
		"SELECT myoid FROM magazine WHERE id = ? UNION ALL "
		"SELECT myoid FROM videogame WHERE id = ?) AND "
		"item_borrower.memberid = member.memberid");
  query.addBindValue(id);
  query.addBindValue(id);
  query.addBindValue(id);
  query.addBindValue(id);
  query.addBindValue(id);
  query.addBindValue(id);
  query.addBindValue(id);
  query.addBindValue(id);
  query.addBindValue(id);
  m_ui.return_scan->clear();

  if(query.exec() && query.next())
    {
      m_ui.return_table->setRowCount(m_ui.return_table->rowCount() + 1);

      if(m_ui.bottom_scroll_on_add->isChecked())
	m_ui.return_table->scrollToBottom();

      play("qrc:/discovered.wav");

      for(int i = 0; i < m_ui.return_table->columnCount(); i++)
	{
	  auto item = new QTableWidgetItem
	    (query.value(i).toString().trimmed());

	  item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	  item->setToolTip(item->text());
	  m_ui.return_table->setItem
	    (m_ui.return_table->rowCount() - 1, i, item);
	}
    }
  else
    play("qrc:/error.wav");
}

void biblioteq_batch_activities::slotScannedBorrowing(void)
{
  QTimer::singleShot(100, this, SLOT(slotScanBorrowingTimerTimeout(void)));
}

void biblioteq_batch_activities::slotScannedDiscover(void)
{
  QTimer::singleShot(100, this, SLOT(slotScanDiscoverTimerTimeout(void)));
}

void biblioteq_batch_activities::slotScannedReturn(void)
{
  QTimer::singleShot(100, this, SLOT(slotScanReturnTimerTimeout(void)));
}

void biblioteq_batch_activities::slotSetGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(auto widget, findChildren<QWidget *> ())
    {
      widget->setFont(font);
      widget->update();
    }

  m_ui.borrow_table->resizeColumnsToContents();
  m_ui.discover_table->resizeColumnsToContents();
  m_ui.return_table->resizeColumnsToContents();
  update();
}
