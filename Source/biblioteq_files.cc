#include <QProgressDialog>

#include "biblioteq.h"
#include "biblioteq_files.h"
#include "biblioteq_filesize_table_item.h"

biblioteq_files::biblioteq_files(biblioteq *biblioteq):QMainWindow(biblioteq)
{
  m_biblioteq = biblioteq;
  m_ui.setupUi(this);
  m_ui.files_table->setColumnHidden
    (m_ui.files_table->columnCount() - 1, true); // MYOID
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
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

void biblioteq_files::slotClose(void)
{
  close();
}

void biblioteq_files::slotRefresh(void)
{
  QProgressDialog progress(this);

  progress.setLabelText(tr("Populating..."));
  progress.setMaximum(0);
  progress.setMinimum(0);
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.show();
  progress.repaint();
  QApplication::processEvents();

  QSqlQuery query(m_biblioteq->getDB());

  query.setForwardOnly(true);
  query.prepare
    ("SELECT file_name, description, file_digest, LENGTH(file), 'book', myoid "
     "FROM book_files "
     "UNION "
     "SELECT file_name, description, file_digest, LENGTH(file), 'book', myoid "
     "FROM grey_literature_files "
     "UNION "
     "SELECT file_name, description, file_digest, LENGTH(file), 'book', myoid "
     "FROM journal_files "
     "UNION "
     "SELECT file_name, description, file_digest, LENGTH(file), 'book', myoid "
     "FROM magazine_files ");

  if(query.exec())
    {
      QLocale locale;

      m_ui.files_table->setRowCount(0);
      m_ui.files_table->setSortingEnabled(false);

      while(query.next())
	{
	  m_ui.files_table->setRowCount(m_ui.files_table->rowCount() + 1);

	  for(int i = 0; i < m_ui.files_table->columnCount(); i++)
	    {
	      QTableWidgetItem *item = nullptr;

	      if(i == 3) // File Size
		item = new biblioteq_filesize_table_item
		  (locale.toString(query.value(i).toLongLong()));
	      else
		item = new QTableWidgetItem(query.value(i).toString());

	      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	      m_ui.files_table->setItem
		(m_ui.files_table->rowCount() - 1, i, item);
	    }

	  progress.repaint();
	  QApplication::processEvents();
	}

      m_ui.files_table->setSortingEnabled(true);
      m_ui.files_table->sortByColumn(0, Qt::AscendingOrder);
    }
}
