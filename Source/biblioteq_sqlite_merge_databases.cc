#include <QFileDialog>
#include <QProgressBar>
#include <QPushButton>

#include "biblioteq.h"
#include "biblioteq_sqlite_merge_databases.h"

biblioteq_sqlite_merge_databases::biblioteq_sqlite_merge_databases
(biblioteq *parent):QMainWindow(parent)
{
  m_qmain = parent;
  m_ui.setupUi(this);
  m_ui.splitter->setStretchFactor(0, 1);
  m_ui.splitter->setStretchFactor(1, 0);
  connect(m_qmain,
	  SIGNAL(fontChanged(const QFont &)),
	  this,
	  SLOT(slotSetGlobalFonts(const QFont &)));
  connect(m_ui.add_row,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAddRow(void)));
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(close(void)));
  connect(m_ui.delete_row,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotDeleteRow(void)));
  connect(m_ui.reset,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotReset(void)));
}

void biblioteq_sqlite_merge_databases::changeEvent(QEvent *event)
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

void biblioteq_sqlite_merge_databases::slotAddRow(void)
{
  m_ui.databases->setRowCount(m_ui.databases->rowCount() + 1);

  auto row = m_ui.databases->rowCount() - 1;

  for(int i = 0; i < m_ui.databases->columnCount(); i++)
    switch(i)
      {
      case PROGRESS_COLUMN:
	{
	  auto progress = new QProgressBar();

	  m_ui.databases->setCellWidget(row, i, progress);
	  break;
	}
      case SELECT_COLUMN:
	{
	  auto pushButton = new QPushButton(tr("Select SQLite Database"));

	  connect(pushButton,
		  SIGNAL(clicked(void)),
		  this,
		  SLOT(slotSelect(void)));
	  m_ui.databases->setCellWidget(row, i, pushButton);
	  break;
	}
      case SQLITE_DATABASE_COLUMN:
	{
	  auto item = new QTableWidgetItem();

	  item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	  m_ui.databases->setItem(row, i, item);
	}
      default:
	{
	  break;
	}
      }
}

void biblioteq_sqlite_merge_databases::slotDeleteRow(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  auto rows(biblioteq_misc_functions::selectedRows(m_ui.databases));

  for(auto i = rows.size() - 1; i >= 0; i--)
    m_ui.databases->removeRow(rows.at(i));

  QApplication::restoreOverrideCursor();
}

void biblioteq_sqlite_merge_databases::slotReset(void)
{
  m_ui.databases->clearContents();
  m_ui.databases->setRowCount(0);
  m_ui.results->clear();
}

void biblioteq_sqlite_merge_databases::slotSelect(void)
{
  auto pushButton = qobject_cast<QPushButton *> (sender());

  if(!pushButton)
    return;

  QFileDialog dialog(this);

  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter("SQLite Database (*.sqlite)");
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowTitle(tr("BiblioteQ: SQLite Database Selection"));
  dialog.exec();
  QApplication::processEvents();

  if(dialog.result() == QDialog::Accepted)
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      for(int i = 0; i < m_ui.databases->rowCount(); i++)
	if(m_ui.databases->cellWidget(i, SELECT_COLUMN) == pushButton)
	  if(m_ui.databases->item(i, SQLITE_DATABASE_COLUMN))
	    {
	      m_ui.databases->item(i, SQLITE_DATABASE_COLUMN)->
		setText(dialog.selectedFiles().value(0));
	      m_ui.databases->item(i, SQLITE_DATABASE_COLUMN)->setToolTip
		(dialog.selectedFiles().value(0));
	      break;
	    }

      QApplication::restoreOverrideCursor();
    }
}

void biblioteq_sqlite_merge_databases::slotSetGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(auto widget, findChildren<QWidget *> ())
    {
      widget->setFont(font);
      widget->update();
    }

  update();
}
