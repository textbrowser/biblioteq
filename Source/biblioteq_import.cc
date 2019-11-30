#include "biblioteq.h"
#include "biblioteq_import.h"
#include "biblioteq_misc_functions.h"

biblioteq_import::biblioteq_import(biblioteq *parent):QMainWindow(parent)
{
  m_ui.setupUi(this);
  connect(m_ui.add_book_row,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAddBookRow(void)));
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
  connect(m_ui.delete_book_row,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotDeleteBookRow(void)));
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

void biblioteq_import::show(QMainWindow *parent)
{
  static bool resized = false;

  if(parent && !resized)
    resize(qRound(0.85 * parent->size().width()),
	   parent->size().height());

  resized = true;
  biblioteq_misc_functions::center(this, parent);
  showNormal();
  activateWindow();
  raise();
}

void biblioteq_import::slotAddBookRow(void)
{
  m_ui.books->setRowCount(m_ui.books->rowCount() + 1);

  QTableWidgetItem *item = new QTableWidgetItem();

  item->setText(QString::number(m_ui.books->rowCount()));
  m_ui.books->setItem(m_ui.books->rowCount() - 1, 0, item);

  QWidget *widget = new QWidget();
  QComboBox *comboBox = new QComboBox();

  comboBox->addItems(QStringList()
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

  QHBoxLayout *layout = new QHBoxLayout(widget);
  QSpacerItem *spacer = new QSpacerItem
    (40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);

  layout->addWidget(comboBox);
  layout->addSpacerItem(spacer);
  layout->setContentsMargins(0, 0, 0, 0);
  m_ui.books->setCellWidget(m_ui.books->rowCount() - 1, 1, widget);
}

void biblioteq_import::slotClose(void)
{
  close();
}

void biblioteq_import::slotDeleteBookRow(void)
{
}
