/*
** -- Local Includes --
*/

#include "main_table.h"

/*
** -- main_table() --
*/

main_table::main_table(QWidget *parent)
{
  (void) parent;
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

void main_table::setColumns(const QString &type, const QString &roles)
{
  int i = 0;
  QStringList list;

  if(type == tr("All") || type == tr("All Overdue") ||
     type == tr("All Requested") ||
     type == tr("All Reserved"))
    {
      if(type == tr("All Overdue") || type == tr("All Reserved"))
	{
	  if(!roles.isEmpty())
	    {
	      list.append(tr("Borrower"));
	      list.append(tr("Member ID"));
	    }

	  list.append(tr("Barcode"));
	  list.append(tr("Reservation Date"));
	  list.append(tr("Due Date"));
	}
      else if(type == tr("All Requested"))
	{
	  if(!roles.isEmpty())
	    {
	      list.append(tr("Borrower"));
	      list.append(tr("Member ID"));
	    }

	  list.append(tr("Request Date"));
	}

      list.append(tr("Title"));
      list.append(tr("ID Number"));
      list.append(tr("Publisher"));
      list.append(tr("Publication Date"));
      list.append(tr("Categories"));
      list.append(tr("Language"));
      list.append(tr("Price"));
      list.append(tr("Monetary Units"));
      list.append(tr("Quantity"));
      list.append(tr("Location"));

      if(type != tr("All Requested"))
	list.append(tr("Availability"));

      list.append(tr("Type"));
      list.append("MYOID");

      if(type == tr("All Requested"))
	list.append("REQUESTOID");
    }
  else if(type == tr("Books"))
    {
      list.append(tr("Title"));
      list.append(tr("Author(s)"));
      list.append(tr("Publisher"));
      list.append(tr("Publication Date"));
      list.append(tr("Edition"));
      list.append(tr("Categories"));
      list.append(tr("Language"));
      list.append(tr("ISBN-10"));
      list.append(tr("Price"));
      list.append(tr("Monetary Units"));
      list.append(tr("Quantity"));
      list.append(tr("Book Binding Type"));
      list.append(tr("Location"));
      list.append(tr("ISBN-13"));
      list.append(tr("LC Control Number"));
      list.append(tr("Call Number"));
      list.append(tr("Dewey Class Number"));
      list.append(tr("Availability"));
      list.append(tr("Type"));
      list.append("MYOID");
    }
  else if(type == tr("Video Games"))
    {
      list.append(tr("Title"));
      list.append(tr("Game Rating"));
      list.append(tr("Platform"));
      list.append(tr("Mode"));
      list.append(tr("Publisher"));
      list.append(tr("Release Date"));
      list.append(tr("Genres"));
      list.append(tr("Language"));
      list.append(tr("UPC"));
      list.append(tr("Price"));
      list.append(tr("Monetary Units"));
      list.append(tr("Quantity"));
      list.append(tr("Location"));
      list.append(tr("Availability"));
      list.append(tr("Type"));
      list.append("MYOID");
    }
  else if(type == tr("Music CDs"))
    {
      list.append(tr("Title"));
      list.append(tr("Artist"));
      list.append(tr("Format"));
      list.append(tr("Recording Label"));
      list.append(tr("Release Date"));
      list.append(tr("Number of Discs"));
      list.append(tr("Runtime"));
      list.append(tr("Categories"));
      list.append(tr("Language"));
      list.append(tr("Catalog Number"));
      list.append(tr("Price"));
      list.append(tr("Monetary Units"));
      list.append(tr("Quantity"));
      list.append(tr("Location"));
      list.append(tr("Audio"));
      list.append(tr("Recording Type"));
      list.append(tr("Availability"));
      list.append(tr("Type"));
      list.append("MYOID");
    }
  else if(type == tr("DVDs"))
    {
      list.append(tr("Title"));
      list.append(tr("Format"));
      list.append(tr("Studio"));
      list.append(tr("Release Date"));
      list.append(tr("Number of Discs"));
      list.append(tr("Runtime"));
      list.append(tr("Categories"));
      list.append(tr("Language"));
      list.append(tr("UPC"));
      list.append(tr("Price"));
      list.append(tr("Monetary Units"));
      list.append(tr("Quantity"));
      list.append(tr("Location"));
      list.append(tr("Rating"));
      list.append(tr("Region"));
      list.append(tr("Aspect Ratio"));
      list.append(tr("Availability"));
      list.append(tr("Type"));
      list.append("MYOID");
    }
  else if(type == tr("Journals") || type == tr("Magazines"))
    {
      list.append(tr("Title"));
      list.append(tr("Publisher"));
      list.append(tr("Publication Date"));
      list.append(tr("Volume"));
      list.append(tr("Issue"));
      list.append(tr("Categories"));
      list.append(tr("Language"));
      list.append(tr("ISSN"));
      list.append(tr("Price"));
      list.append(tr("Monetary Units"));
      list.append(tr("Quantity"));
      list.append(tr("Location"));
      list.append(tr("LC Control Number"));
      list.append(tr("Call Number"));
      list.append(tr("Dewey Number"));
      list.append(tr("Availability"));
      list.append(tr("Type"));
      list.append("MYOID");
    }

  setColumnCount(list.size());
  setHorizontalHeaderLabels(list);

  if(type != tr("All") && type != tr("All Overdue") &&
     type != tr("All Requested") &&
     type != tr("All Reserved"))
    {
      setColumnHidden(list.size() - 1, true);
      setColumnHidden(list.size() - 2, true);
    }
  else
    {
      setColumnHidden(list.size() - 1, true);

      if(type == tr("All Requested"))
	setColumnHidden(list.size() - 2, true);
    }

  for(i = 0; i < hiddenColumns[type].size(); i++)
    setColumnHidden(hiddenColumns[type][i], true);

  list.clear();
}

/*
** -- resetTable() --
*/

void main_table::resetTable(const QString &type, const QString &roles)
{
  clear();
  setRowCount(0);
  setColumnCount(0);
  scrollToTop();
  horizontalScrollBar()->setValue(0);

  if(!type.isEmpty())
    setColumns(type, roles);

  horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  horizontalHeader()->setSortIndicatorShown(true);
  resizeColumnsToContents();
  setCurrentItem(0);
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
