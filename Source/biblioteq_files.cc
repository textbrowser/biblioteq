#include <QMessageBox>
#include <QProgressDialog>
#include <QSettings>
#include <QtMath>

#include "biblioteq.h"
#include "biblioteq_files.h"
#include "biblioteq_filesize_table_item.h"

biblioteq_files::biblioteq_files(biblioteq *biblioteq):QMainWindow(biblioteq)
{
  m_biblioteq = biblioteq;
  m_ui.setupUi(this);
  m_ui.files_table->setColumnHidden
    (m_ui.files_table->columnCount() - 1, true); // MYOID

  QSettings settings;

  m_ui.pages->setValue(settings.value("filesPerPage", 500).toInt());
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
  connect(m_ui.export_files,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotExport(void)));
  connect(m_ui.pages,
	  SIGNAL(valueChanged(int)),
	  this,
	  SLOT(slotPagesChanged(int)));
  connect(m_ui.refresh,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRefresh(void)));
}

biblioteq_files::~biblioteq_files()
{
}

void biblioteq_files::changeEvent(QEvent *event)
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

void biblioteq_files::reset(void)
{
  disconnect(m_ui.page,
	     SIGNAL(currentIndexChanged(int)),
	     this,
	     SLOT(slotRefresh(void)));
  m_ui.files_table->setRowCount(0);
  m_ui.page->clear();
  m_ui.page->addItem("1");
}

void biblioteq_files::slotClose(void)
{
  close();
}

void biblioteq_files::slotExport(void)
{
  QModelIndexList list(m_ui.files_table->selectionModel()->selectedRows());

  if(biblioteq::MAXIMUM_DEVICES_CONFIRMATION <= list.size())
    {
      if(QMessageBox::question(this,
			       tr("BiblioteQ: Question"),
			       tr("Are you sure that you wish to export %1 "
				  "files?").arg(list.size()),
			       QMessageBox::Yes | QMessageBox::No,
			       QMessageBox::No) == QMessageBox::No)
	{
	  QApplication::processEvents();
	  return;
	}

      QApplication::processEvents();
    }
  else if(list.isEmpty())
    return;
}

void biblioteq_files::slotPagesChanged(int value)
{
  QSettings settings;

  settings.setValue("filesPerPage", value);
}

void biblioteq_files::slotRefresh(void)
{
  if(!m_biblioteq->getDB().isOpen())
    return;

  QProgressDialog progress(this);

  progress.setLabelText(tr("Populating..."));
  progress.setMaximum(0);
  progress.setMinimum(0);
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.show();
  progress.repaint();
  QApplication::processEvents();
  disconnect(m_ui.page,
	     SIGNAL(currentIndexChanged(int)),
	     this,
	     SLOT(slotRefresh(void)));
  m_ui.files_table->setRowCount(0);

  QSqlQuery query(m_biblioteq->getDB());
  int page = m_ui.page->currentIndex();

  m_ui.page->clear();
  query.setForwardOnly(true);
  query.prepare
    ("SELECT (SELECT COUNT(*) FROM book_files) "
     "+ "
     "(SELECT COUNT(*) FROM grey_literature_files) "
     "+ "
     "(SELECT COUNT(*) FROM journal_files) "
     "+ "
     "(SELECT COUNT(*) FROM magazine_files) AS totalCount");

  if(query.exec() && query.next())
    {
      int pages = qCeil
	(query.value(0).toDouble() / static_cast<double> (m_ui.pages->value()));

      for(int i = 0; i < pages; i++)
	m_ui.page->addItem(QString::number(i + 1));

      m_ui.files_table->setRowCount(query.value(0).toInt());
    }

  if(m_ui.page->count() == 0)
    m_ui.page->addItem("1");

  page = qBound(0, page, m_ui.page->count() - 1);
  m_ui.page->setCurrentIndex(page);
  query.prepare
    (QString("SELECT file_name, description, file_digest, LENGTH(file), "
	     "'book', myoid "
	     "FROM book_files "
	     "UNION "
	     "SELECT file_name, description, file_digest, LENGTH(file), "
	     "'grey literature', myoid "
	     "FROM grey_literature_files "
	     "UNION "
	     "SELECT file_name, description, file_digest, LENGTH(file), "
	     "'journal', myoid "
	     "FROM journal_files "
	     "UNION "
	     "SELECT file_name, description, file_digest, LENGTH(file), "
	     "'magazine', myoid "
	     "FROM magazine_files "
	     "LIMIT %1 OFFSET %2").
     arg(m_ui.pages->value()).
     arg(m_ui.page->currentIndex() * m_ui.pages->value()));

  if(query.exec())
    {
      QLocale locale;
      int row = 0;
      int totalRows = 0;

      m_ui.files_table->setSortingEnabled(false);

      while(query.next() && totalRows < m_ui.files_table->rowCount())
	{
	  totalRows += 1;

	  for(int i = 0; i < m_ui.files_table->columnCount(); i++)
	    {
	      QTableWidgetItem *item = nullptr;

	      if(i == 3) // File Size
		item = new biblioteq_filesize_table_item
		  (locale.toString(query.value(i).toLongLong()));
	      else
		item = new QTableWidgetItem(query.value(i).toString());

	      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	      m_ui.files_table->setItem(row, i, item);
	    }

	  progress.repaint();
	  row += 1;
	  QApplication::processEvents();
	}

      m_ui.files_table->setRowCount(totalRows);
      m_ui.files_table->setSortingEnabled(true);
      m_ui.files_table->sortByColumn(0, Qt::AscendingOrder);
    }

  connect(m_ui.page,
	  SIGNAL(currentIndexChanged(int)),
	  this,
	  SLOT(slotRefresh(void)));
}
