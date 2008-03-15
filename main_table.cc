/*
** -- Local Includes --
*/

#include "main_table.h"

/*
** -- main_table() --
*/

main_table::main_table(QFrame *frame)
{
  (void) frame;
  setDragEnabled(false);
  setAcceptDrops(false);
  horizontalHeader()->setMovable(true);
  horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  horizontalHeader()->setSortIndicatorShown(true);
  verticalHeader()->setResizeMode(QHeaderView::Fixed);
}

/*
** -- setColumns() --
*/

void main_table::setColumns(const QString &type)
{
  int i = 0;
  QStringList list;

  if(type == "All" || type == "Overdue" || type == "Reserved")
    {
      if(type == "Overdue" || type == "Reserved")
	{
	  list.append("Borrower");
	  list.append("Member ID");
	  list.append("Barcode");
	  list.append("Reservation Date");
	  list.append("Due Date");
	}

      list.append("Title");
      list.append("ID Number");
      list.append("Publisher");
      list.append("Publication Date");
      list.append("Category");
      list.append("Language");
      list.append("Price");
      list.append("Monetary Units");
      list.append("Quantity");
      list.append("Location");
      list.append("Availability");
      list.append("Type");
      list.append("OID");
    }
  else if(type == "Books")
    {
      list.append("Title");
      list.append("Author(s)");
      list.append("Publisher");
      list.append("Publication Date");
      list.append("Edition");
      list.append("Category");
      list.append("Language");
      list.append("ISBN-10");
      list.append("Price");
      list.append("Monetary Units");
      list.append("Quantity");
      list.append("Book Binding Type");
      list.append("Location");
      list.append("ISBN-13");
      list.append("LC Control Number");
      list.append("Call Number");
      list.append("Dewey Class Number");
      list.append("Availability");
      list.append("Type");
      list.append("OID");
    }
  else if(type == "Video Games")
    {
      list.append("Title");
      list.append("Game Rating");
      list.append("Platform");
      list.append("Mode");
      list.append("Publisher");
      list.append("Release Date");
      list.append("Genre");
      list.append("Language");
      list.append("UPC");
      list.append("Price");
      list.append("Monetary Units");
      list.append("Quantity");
      list.append("Location");
      list.append("Availability");
      list.append("Type");
      list.append("OID");
    }
  else if(type == "Music CDs")
    {
      list.append("Title");
      list.append("Artist");
      list.append("Format");
      list.append("Recording Label");
      list.append("Release Date");
      list.append("Number of Discs");
      list.append("Runtime");
      list.append("Category");
      list.append("Language");
      list.append("Catalog Number");
      list.append("Price");
      list.append("Monetary Units");
      list.append("Quantity");
      list.append("Location");
      list.append("Audio");
      list.append("Recording Type");
      list.append("Availability");
      list.append("Type");
      list.append("OID");
    }
  else if(type == "DVDs")
    {
      list.append("Title");
      list.append("Format");
      list.append("Studio");
      list.append("Release Date");
      list.append("Number of Discs");
      list.append("Runtime");
      list.append("Category");
      list.append("Language");
      list.append("UPC");
      list.append("Price");
      list.append("Monetary Units");
      list.append("Quantity");
      list.append("Location");
      list.append("Rating");
      list.append("Region");
      list.append("Aspect Ratio");
      list.append("Availability");
      list.append("Type");
      list.append("OID");
    }
  else if(type == "Journals" || type == "Magazines")
    {
      list.append("Title");
      list.append("Publisher");
      list.append("Publication Date");
      list.append("Volume");
      list.append("Issue");
      list.append("Category");
      list.append("Language");
      list.append("ISSN");
      list.append("Price");
      list.append("Monetary Units");
      list.append("Quantity");
      list.append("Location");
      list.append("LC Control Number");
      list.append("Call Number");
      list.append("Dewey Number");
      list.append("Availability");
      list.append("Type");
      list.append("OID");
    }

  setColumnCount(list.size());
  setHorizontalHeaderLabels(list);

  if(type != "All" && type != "Overdue" && type != "Reserved")
    {
      setColumnHidden(list.size() - 1, true);
      setColumnHidden(list.size() - 2, true);
    }
  else
    setColumnHidden(list.size() - 1, true);

  for(i = 0; i < hiddenColumns[type].size(); i++)
    setColumnHidden(hiddenColumns[type][i], true);

  list.clear();
}

/*
** -- resetTable() --
*/

void main_table::resetTable(const QString &type)
{
  clear();
  setRowCount(0);
  setColumnCount(0);
  scrollToTop();
  horizontalScrollBar()->setValue(0);

  if(!type.isEmpty())
    setColumns(type);

  horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  horizontalHeader()->setSortIndicatorShown(true);
  resizeColumnsToContents();
}

/*
** -- recordColumnHidden() --
*/

void main_table::recordColumnHidden(const QString &type, const int index,
				    const bool hidden)
{
  if(hidden)
    {
      if(!hiddenColumns[type].contains(index))
	hiddenColumns[type].append(index);
    }
  else
    (int) hiddenColumns[type].removeAll(index);
}

/*
** -- clearHiddenColumnsRecord() --
*/

void main_table::clearHiddenColumnsRecord(void)
{
  hiddenColumns.clear();
}
