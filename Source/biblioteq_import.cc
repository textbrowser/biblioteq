#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>

#include "biblioteq.h"
#include "biblioteq_import.h"
#include "biblioteq_misc_functions.h"
#include "ui_biblioteq_generalmessagediag.h"

biblioteq_import::biblioteq_import(biblioteq *parent):QMainWindow(parent)
{
  m_qmain = parent;
  m_ui.setupUi(this);

  if(m_qmain)
    connect(m_qmain,
	    SIGNAL(fontChanged(const QFont &)),
	    this,
	    SLOT(setGlobalFonts(const QFont &)));

  connect(m_ui.add_book_row,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAddBookRow(void)));
  connect(m_ui.books_templates,
	  SIGNAL(currentIndexChanged(int)),
	  this,
	  SLOT(slotBooksTemplates(int)));
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
  connect(m_ui.delete_book_row,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotDeleteBookRow(void)));
  connect(m_ui.reset,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(m_ui.save,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotImport(void)));
  connect(m_ui.select_csv_file,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectCSVFile(void)));
}

void biblioteq_import::changeEvent(QEvent *event)
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

void biblioteq_import::closeEvent(QCloseEvent *event)
{
  QMainWindow::closeEvent(event);
}

void biblioteq_import::importBooks(QProgressDialog *progress,
				   QStringList &errors,
				   qint64 *imported,
				   qint64 *notImported)
{
  if(!progress)
    return;

  QFile file(m_ui.csv_file->text());

  if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return;

  QMapIterator<int, QPair<QString, QString> > it(m_booksMappings);
  QString f("");
  QString q("");
  QString queryString("");

  while(it.hasNext())
    {
      it.next();

      if(!it.value().first.contains("<ignored>"))
	{
	  f.append(it.value().first);
	  q.append("?");

	  if(it.hasNext())
	    {
	      f.append(",");
	      q.append(",");
	    }
	}
    }

  queryString.append("INSERT INTO book(");
  queryString.append("description,");

  if(m_qmain->getDB().driverName() != "QPSQL")
    queryString.append("myoid,");

  queryString.append(f);
  queryString.append(") VALUES (");
  queryString.append("?,");

  if(m_qmain->getDB().driverName() != "QPSQL")
    queryString.append("?,");

  queryString.append(q);
  queryString.append(")");

  if(m_qmain->getDB().driverName() == "QPSQL")
    queryString.append(" RETURNING myoid");

  if(imported)
    *imported = 0;

  if(notImported)
    *notImported = 0;

  auto list(m_ui.books_ignored_rows->text().trimmed().
	    split(' ',
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
		  Qt::SkipEmptyParts
#else
		  QString::SkipEmptyParts
#endif
		  ));
  qint64 ct = 0;

  while(ct++, !file.atEnd())
    {
      QString data(file.readLine().trimmed());

      if(list.contains(QString::number(ct)))
	continue;

      if(!data.isEmpty())
	{
	  /*
	  ** Separate by the delimiter.
	  */

	  auto list
	    (data.split
	     (QRegularExpression(QString("%1(?=([^\"]*\"[^\"]*\")*[^\"]*$)").
				 arg(m_ui.delimiter->text()))));

	  if(!list.isEmpty())
	    {
	      QSqlQuery query(m_qmain->getDB());
	      QString id("");
	      int quantity = 1;
	      qint64 oid = 0;

	      query.prepare(queryString);
	      query.addBindValue("N/A"); // Description.

	      if(m_qmain->getDB().driverName() != "QPSQL")
		{
		  QString errorstr("");

		  oid = biblioteq_misc_functions::getSqliteUniqueId
		    (m_qmain->getDB(), errorstr);

		  if(errorstr.isEmpty())
		    query.addBindValue(oid);
		}

	      for(int i = 1; i <= list.size(); i++)
		{
		  if(!m_booksMappings.contains(i))
		    continue;
		  else if(m_booksMappings.value(i).first.contains("<ignored>"))
		    continue;

		  auto str(QString(list.at(i - 1)).remove('"').trimmed());

		  if(m_booksMappings.value(i).first == "id")
		    id = str;
		  else if(m_booksMappings.value(i).first == "quantity")
		    quantity = str.toInt();

		  if(str.isEmpty())
		    /*
		    ** If the value in the CSV file is empty,
		    ** refer to a substitution.
		    */

		    str = m_booksMappings.value(i).second;

		  if(str.isEmpty())
		    query.addBindValue(QVariant(QVariant::String));
		  else
		    query.addBindValue(str);
		}

	      if(query.exec())
		{
		  if(m_qmain->getDB().driverName() == "QPSQL")
		    {
		      query.next();
		      oid = query.value(0).toLongLong();
		    }

		  QString errorstr("");

		  if(id.isEmpty())
		    biblioteq_misc_functions::createInitialCopies
		      (QString::number(oid),
		       quantity,
		       m_qmain->getDB(),
		       "Book",
		       errorstr);
		  else
		    biblioteq_misc_functions::createInitialCopies
		      (id, quantity, m_qmain->getDB(), "Book", errorstr);

		  if(!errorstr.isEmpty())
		    // Do not increase notImported.

		    errors << tr("biblioteq_misc_functions::"
				 "createInitialCopies() "
				 "error (%1) on row %2.").
		      arg(errorstr).arg(ct);

		  if(imported)
		    *imported += 1;
		}
	      else
		{
		  errors << tr("Database error (%1) on row %2.").
		    arg(query.lastError().text()).arg(ct);

		  if(notImported)
		    *notImported += 1;
		}
	    }
	  else if(notImported)
	    {
	      errors << tr("Empty row %1.").arg(ct);
	      *notImported += 1;
	    }
	}
    }

  file.close();
}

void biblioteq_import::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(auto widget, findChildren<QWidget *> ())
    {
      widget->setFont(font);
      widget->update();
    }

  m_ui.books->resizeRowsToContents();
  update();
}

void biblioteq_import::show(QMainWindow *parent)
{
  static auto resized = false;

  if(parent && !resized)
    resize(qRound(0.50 * parent->size().width()),
	   qRound(0.80 * parent->size().height()));

  resized = true;
  biblioteq_misc_functions::center(this, parent);
  showNormal();
  activateWindow();
  raise();
}

void biblioteq_import::slotAddBookRow(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_ui.books->setRowCount(m_ui.books->rowCount() + 1);

  auto item = new QTableWidgetItem();

  item->setText(QString::number(m_ui.books->rowCount()));
  m_ui.books->setItem
    (m_ui.books->rowCount() - 1, BooksColumns::CSV_COLUMN_NUMBER, item);

  auto widget = new QWidget();
  auto comboBox = new QComboBox();

  comboBox->addItems(QStringList()
		     << "<ignored>"
		     << "accession_number"
		     << "author"
		     << "binding_type"
		     << "callnumber"
		     << "category"
		     << "condition"
		     << "description"
		     << "deweynumber"
		     << "edition"
		     << "id"
		     << "isbn13"
		     << "keyword"
		     << "language"
		     << "lccontrolnumber"
		     << "location"
		     << "marc_tags"
		     << "monetary_units"
		     << "originality"
		     << "pdate"
		     << "place"
		     << "price"
		     << "publisher"
		     << "quantity"
		     << "title");
  comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

  auto layout = new QHBoxLayout(widget);
  auto spacer = new QSpacerItem
    (40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);

  layout->addWidget(comboBox);
  layout->addSpacerItem(spacer);
  layout->setContentsMargins(0, 0, 0, 0);
  m_ui.books->setCellWidget
    (m_ui.books->rowCount() - 1,
     BooksColumns::BIBLIOTEQ_BOOKS_TABLE_FIELD_NAME,
     widget);
  item = new QTableWidgetItem();
  item->setText("N/A");
  m_ui.books->setItem
    (m_ui.books->rowCount() - 1, BooksColumns::SUBSTITUTE_VALUE, item);
  m_ui.books->resizeRowsToContents();
  QApplication::restoreOverrideCursor();
}

void biblioteq_import::slotBooksTemplates(int index)
{
  switch(index)
    {
    case 0:
      {
	break;
      }
    case 1:
      {
	if(m_ui.books->rowCount() > 0)
	  {
	    if(QMessageBox::question(this,
				     tr("BiblioteQ: Question"),
				     tr("Populate the Books table with "
					"Template 1 values?"),
				     QMessageBox::Yes | QMessageBox::No,
				     QMessageBox::No) == QMessageBox::No)
	      {
		QApplication::processEvents();
		break;
	      }
	  }

	m_ui.books->setRowCount(0);
	m_ui.books_ignored_rows->setText("1");

	QStringList list;

	list << "title"
	     << "author"
	     << "publisher"
	     << "pdate"
	     << "place"
	     << "edition"
	     << "category"
	     << "language"
	     << "id"
	     << "price"
	     << "monetary_units"
	     << "quantity"
	     << "binding_type"
	     << "location"
	     << "isbn13"
	     << "lccontrolnumber"
	     << "callnumber"
	     << "deweynumber"
	     << "<ignored>" // Availability
	     << "<ignored>" // Total Reserved
	     << "originality"
	     << "condition"
	     << "accession_number";

	for(int i = 0; i < list.size(); i++)
	  {
	    slotAddBookRow();

	    auto widget = m_ui.books->cellWidget
	      (i, BooksColumns::BIBLIOTEQ_BOOKS_TABLE_FIELD_NAME);

	    if(widget)
	      {
		auto comboBox = widget->findChild<QComboBox *> ();

		if(comboBox)
		  {
		    comboBox->setCurrentIndex(comboBox->findText(list.at(i)));

		    if(comboBox->currentIndex() < 0)
		      comboBox->setCurrentIndex(0);
		  }
	      }
	  }

	break;
      }
    default:
      {
	break;
      }
    }

  m_ui.books_templates->setCurrentIndex(0);
}

void biblioteq_import::slotClose(void)
{
  close();
}

void biblioteq_import::slotDeleteBookRow(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  auto rows(biblioteq_misc_functions::selectedRows(m_ui.books));

  for(int i = rows.size() - 1; i >= 0; i--)
    m_ui.books->removeRow(rows.at(i));

  QApplication::restoreOverrideCursor();
}

void biblioteq_import::slotImport(void)
{
  /*
  ** Test if the specified file is readable.
  */

  QFileInfo fileInfo(m_ui.csv_file->text());

  if(!fileInfo.isReadable())
    {
      if(fileInfo.absolutePath().isEmpty())
	QMessageBox::critical
	  (this,
	   tr("BiblioteQ: Error"),
	   tr("The specified file is not readable."));
      else
	QMessageBox::critical
	  (this,
	   tr("BiblioteQ: Error"),
	   tr("The file %1 is not readable.").arg(fileInfo.absolutePath()));

      return;
    }

  /*
  ** Test the various mappings.
  */

  QMap<int, QPair<QString, QString> > map;

  for(int i = 0; i < m_ui.books->rowCount(); i++)
    {
      auto item1 = m_ui.books->item(i, BooksColumns::CSV_COLUMN_NUMBER);
      auto item2 = m_ui.books->item(i, BooksColumns::SUBSTITUTE_VALUE);
      auto widget = m_ui.books->cellWidget
	(i, BooksColumns::BIBLIOTEQ_BOOKS_TABLE_FIELD_NAME);

      if(!item1 || !item2 || !widget)
	continue;

      auto comboBox = widget->findChild<QComboBox *> ();

      if(!comboBox)
	continue;

      if(map.contains(item1->text().toInt()))
	{
	  m_ui.books->selectRow(i);
	  QMessageBox::critical
	    (this,
	     tr("BiblioteQ: Error"),
	     tr("Duplicate mapping discovered in the Books table. Please "
		"review row %1.").arg(item1->row()));
	  return;
	}

      map[item1->text().toInt()] =
	QPair<QString, QString> (comboBox->currentText(), item2->text());
    }

  if(map.isEmpty())
    {
      QMessageBox::critical
	(this,
	 tr("BiblioteQ: Error"),
	 tr("Please define column mappings."));
      return;
    }

  QScopedPointer<QProgressDialog> progress(new QProgressDialog(this));

  progress->setLabelText(tr("Importing the CSV file..."));
  progress->setMaximum(0);
  progress->setMinimum(0);
  progress->setModal(true);
  progress->setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress->show();
  progress->repaint();
  QApplication::processEvents();
  m_booksMappings = map;

  QStringList errors;
  qint64 imported = 0;
  qint64 notImported = 0;

  importBooks(progress.data(), errors, &imported, &notImported);
  progress->close();
  QApplication::processEvents();

  if(!errors.isEmpty())
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      QDialog dialog(this);
      QString errorstr("");
      Ui_generalmessagediag ui;

      errors.prepend(tr("Imported: %1. Not imported: %2.\n").
		     arg(imported).
		     arg(notImported));

      for(int i = 0; i < errors.size(); i++)
	{
	  errorstr.append(errors.at(i));
	  errorstr.append('\n');
	}

      ui.setupUi(&dialog);
      ui.text->setPlainText(errorstr.trimmed());
      connect(ui.cancelButton,
	      SIGNAL(clicked(void)),
	      &dialog,
	      SLOT(close(void)));
      dialog.setWindowTitle(tr("BiblioteQ: Import Results"));
      QApplication::restoreOverrideCursor();
      dialog.exec();
    }
  else
    QMessageBox::information
      (this,
       tr("BiblioteQ: Information"),
       tr("Imported: %1. Not imported: %2.").arg(imported).arg(notImported));

  QApplication::processEvents();
}

void biblioteq_import::slotReset(void)
{
  if(m_ui.books->rowCount() > 0 ||
     !m_ui.csv_file->text().isEmpty() ||
     m_ui.delimiter->text() != ",")
    if(QMessageBox::question(this,
			     tr("BiblioteQ: Question"),
			     tr("Are you sure that you wish to reset?"),
			     QMessageBox::Yes | QMessageBox::No,
			     QMessageBox::No) == QMessageBox::No)
      {
	QApplication::processEvents();
	return;
      }

  m_booksMappings.clear();
  m_ui.books->setRowCount(0);
  m_ui.books_ignored_rows->clear();
  m_ui.csv_file->clear();
  m_ui.delimiter->setText(",");
}

void biblioteq_import::slotSelectCSVFile(void)
{
  QFileDialog dialog(this);

  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter("CSV (*.csv)");
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowTitle(tr("BiblioteQ: Select CSV Import File"));
  dialog.exec();
  QApplication::processEvents();

  if(dialog.result() == QDialog::Accepted)
    m_ui.csv_file->setText(dialog.selectedFiles().value(0));
}
