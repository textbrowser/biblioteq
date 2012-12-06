/*
** -- Qt Includes --
*/

#include <QtDebug>

/*
** -- Local Includes --
*/

#include "main_table.h"

/*
** -- main_table() --
*/

main_table::main_table(QWidget *parent):QTableWidget(parent)
{
  setDragEnabled(false);
  setAcceptDrops(false);
  horizontalHeader()->setMovable(true);
  horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  horizontalHeader()->setSortIndicatorShown(true);
  horizontalHeader()->setStretchLastSection(true);
  verticalHeader()->setResizeMode(QHeaderView::Fixed);
}

/*
** -- setColumns() --
*/

void main_table::setColumns(const QString &username,
			    const QString &type, const QString &roles)
{
  int i = 0;
  QStringList list;

  m_columnHeaderIndexes.clear();

  if(type == "All" ||
     type == "All Available" ||
     type == "All Overdue" ||
     type == "All Requested" ||
     type == "All Reserved")
    {
      if(type == "All Overdue" || type == "All Reserved")
	{
	  if(!roles.isEmpty())
	    {
	      list.append(tr("Borrower"));
	      list.append(tr("Member ID"));
	      m_columnHeaderIndexes["Borrower"] = i; i++;
	      m_columnHeaderIndexes["Member ID"] = i; i++;
	    }

	  list.append(tr("Barcode"));
	  list.append(tr("Reservation Date"));
	  list.append(tr("Due Date"));
	  m_columnHeaderIndexes["Barcode"] = i; i++;
	  m_columnHeaderIndexes["Reservation Date"] = i; i++;
	  m_columnHeaderIndexes["Due Date"] = i; i++;
	}
      else if(type == "All Requested")
	{
	  if(!roles.isEmpty())
	    {
	      list.append(tr("Borrower"));
	      list.append(tr("Member ID"));
	      m_columnHeaderIndexes["Borrower"] = i; i++;
	      m_columnHeaderIndexes["Member ID"] = i; i++;
	    }

	  list.append(tr("Request Date"));
	  m_columnHeaderIndexes["Request Date"] = i; i++;
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
      m_columnHeaderIndexes["Title"] = i; i++;
      m_columnHeaderIndexes["ID Number"] = i; i++;
      m_columnHeaderIndexes["Publisher"] = i; i++;
      m_columnHeaderIndexes["Publication Date"] = i; i++;
      m_columnHeaderIndexes["Categories"] = i; i++;
      m_columnHeaderIndexes["Language"] = i; i++;
      m_columnHeaderIndexes["Price"] = i; i++;
      m_columnHeaderIndexes["Monetary Units"] = i; i++;
      m_columnHeaderIndexes["Quantity"] = i; i++;
      m_columnHeaderIndexes["Location"] = i; i++;

      if(type != "All Requested")
	{
	  list.append(tr("Availability"));
	  list.append(tr("Total Reserved"));
	  m_columnHeaderIndexes["Availability"] = i; i++;
	  m_columnHeaderIndexes["Total Reserved"] = i; i++;
	}

      list.append(tr("Type"));
      list.append("MYOID");
      m_columnHeaderIndexes["Type"] = i; i++;
      m_columnHeaderIndexes["MYOID"] = i; i++;

      if(type == "All Requested")
	{
	  list.append("REQUESTOID");
	  m_columnHeaderIndexes["REQUESTOID"] = i; i++;
	}
    }
  else if(type == "Books")
    {
      list.append(tr("Title"));
      list.append(tr("Author(s)"));
      list.append(tr("Publisher"));
      list.append(tr("Publication Date"));
      list.append(tr("Place of Publication"));
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
      list.append(tr("Total Reserved"));
      list.append(tr("Type"));
      list.append("MYOID");
      m_columnHeaderIndexes["Title"] = i; i++;
      m_columnHeaderIndexes["Author(s)"] = i; i++;
      m_columnHeaderIndexes["Publisher"] = i; i++;
      m_columnHeaderIndexes["Publication Date"] = i; i++;
      m_columnHeaderIndexes["Place of Publication"] = i; i++;
      m_columnHeaderIndexes["Edition"] = i; i++;
      m_columnHeaderIndexes["Categories"] = i; i++;
      m_columnHeaderIndexes["Language"] = i; i++;
      m_columnHeaderIndexes["ISBN-10"] = i; i++;
      m_columnHeaderIndexes["Price"] = i; i++;
      m_columnHeaderIndexes["Monetary Units"] = i; i++;
      m_columnHeaderIndexes["Quantity"] = i; i++;
      m_columnHeaderIndexes["Book Binding Type"] = i; i++;
      m_columnHeaderIndexes["Location"] = i; i++;
      m_columnHeaderIndexes["ISBN-13"] = i; i++;
      m_columnHeaderIndexes["LC Control Number"] = i; i++;
      m_columnHeaderIndexes["Call Number"] = i; i++;
      m_columnHeaderIndexes["Dewey Class Number"] = i; i++;
      m_columnHeaderIndexes["Availability"] = i; i++;
      m_columnHeaderIndexes["Total Reserved"] = i; i++;
      m_columnHeaderIndexes["Type"] = i; i++;
      m_columnHeaderIndexes["MYOID"] = i; i++;
    }
  else if(type == "DVDs")
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
      list.append(tr("Total Reserved"));
      list.append(tr("Type"));
      list.append("MYOID");
      m_columnHeaderIndexes["Title"] = i; i++;
      m_columnHeaderIndexes["Format"] = i; i++;
      m_columnHeaderIndexes["Studio"] = i; i++;
      m_columnHeaderIndexes["Release Date"] = i; i++;
      m_columnHeaderIndexes["Number of Discs"] = i; i++;
      m_columnHeaderIndexes["Runtime"] = i; i++;
      m_columnHeaderIndexes["Categories"] = i; i++;
      m_columnHeaderIndexes["Language"] = i; i++;
      m_columnHeaderIndexes["UPC"] = i; i++;
      m_columnHeaderIndexes["Price"] = i; i++;
      m_columnHeaderIndexes["Monetary Units"] = i; i++;
      m_columnHeaderIndexes["Quantity"] = i; i++;
      m_columnHeaderIndexes["Location"] = i; i++;
      m_columnHeaderIndexes["Rating"] = i; i++;
      m_columnHeaderIndexes["Region"] = i; i++;
      m_columnHeaderIndexes["Aspect Ratio"] = i; i++;
      m_columnHeaderIndexes["Availability"] = i; i++;
      m_columnHeaderIndexes["Total Reserved"] = i; i++;
      m_columnHeaderIndexes["Type"] = i; i++;
      m_columnHeaderIndexes["MYOID"] = i; i++;
    }
  else if(type == "Journals" || type == "Magazines")
    {
      list.append(tr("Title"));
      list.append(tr("Publisher"));
      list.append(tr("Publication Date"));
      list.append(tr("Place of Publication"));
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
      list.append(tr("Total Reserved"));
      list.append(tr("Type"));
      list.append("MYOID");
      m_columnHeaderIndexes["Title"] = i; i++;
      m_columnHeaderIndexes["Publisher"] = i; i++;
      m_columnHeaderIndexes["Publication Date"] = i; i++;
      m_columnHeaderIndexes["Place of Publication"] = i; i++;
      m_columnHeaderIndexes["Volume"] = i; i++;
      m_columnHeaderIndexes["Issue"] = i; i++;
      m_columnHeaderIndexes["Categories"] = i; i++;
      m_columnHeaderIndexes["Language"] = i; i++;
      m_columnHeaderIndexes["ISSN"] = i; i++;
      m_columnHeaderIndexes["Price"] = i; i++;
      m_columnHeaderIndexes["Monetary Units"] = i; i++;
      m_columnHeaderIndexes["Quantity"] = i; i++;
      m_columnHeaderIndexes["Location"] = i; i++;
      m_columnHeaderIndexes["LC Control Number"] = i; i++;
      m_columnHeaderIndexes["Call Number"] = i; i++;
      m_columnHeaderIndexes["Dewey Number"] = i; i++;
      m_columnHeaderIndexes["Availability"] = i; i++;
      m_columnHeaderIndexes["Total Reserved"] = i; i++;
      m_columnHeaderIndexes["Type"] = i; i++;
      m_columnHeaderIndexes["MYOID"] = i; i++;
    }
  else if(type == "Music CDs")
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
      list.append(tr("Total Reserved"));
      list.append(tr("Type"));
      list.append("MYOID");
      m_columnHeaderIndexes["Title"] = i; i++;
      m_columnHeaderIndexes["Artist"] = i; i++;
      m_columnHeaderIndexes["Format"] = i; i++;
      m_columnHeaderIndexes["Recording Label"] = i; i++;
      m_columnHeaderIndexes["Release Date"] = i; i++;
      m_columnHeaderIndexes["Number of Discs"] = i; i++;
      m_columnHeaderIndexes["Runtime"] = i; i++;
      m_columnHeaderIndexes["Categories"] = i; i++;
      m_columnHeaderIndexes["Language"] = i; i++;
      m_columnHeaderIndexes["Catalog Number"] = i; i++;
      m_columnHeaderIndexes["Price"] = i; i++;
      m_columnHeaderIndexes["Monetary Units"] = i; i++;
      m_columnHeaderIndexes["Quantity"] = i; i++;
      m_columnHeaderIndexes["Location"] = i; i++;
      m_columnHeaderIndexes["Audio"] = i; i++;
      m_columnHeaderIndexes["Recording Type"] = i; i++;
      m_columnHeaderIndexes["Availability"] = i; i++;
      m_columnHeaderIndexes["Total Reserved"] = i; i++;
      m_columnHeaderIndexes["Type"] = i; i++;
      m_columnHeaderIndexes["MYOID"] = i; i++;
    }
  else if(type == "Photograph Collections")
    {
      list.append(tr("Title"));
      list.append(tr("ID"));
      list.append(tr("About"));
      list.append(tr("Type"));
      list.append("MYOID");
      m_columnHeaderIndexes["Title"] = i; i++;
      m_columnHeaderIndexes["ID"] = i; i++;
      m_columnHeaderIndexes["About"] = i; i++;
      m_columnHeaderIndexes["Type"] = i; i++;
      m_columnHeaderIndexes["MYOID"] = i; i++;
    }
  else if(type == "Video Games")
    {
      list.append(tr("Title"));
      list.append(tr("Game Rating"));
      list.append(tr("Platform"));
      list.append(tr("Mode"));
      list.append(tr("Publisher"));
      list.append(tr("Release Date"));
      list.append(tr("Place of Publication"));
      list.append(tr("Genres"));
      list.append(tr("Language"));
      list.append(tr("UPC"));
      list.append(tr("Price"));
      list.append(tr("Monetary Units"));
      list.append(tr("Quantity"));
      list.append(tr("Location"));
      list.append(tr("Availability"));
      list.append(tr("Total Reserved"));
      list.append(tr("Type"));
      list.append("MYOID");
      m_columnHeaderIndexes["Title"] = i; i++;
      m_columnHeaderIndexes["Game Rating"] = i; i++;
      m_columnHeaderIndexes["Platform"] = i; i++;
      m_columnHeaderIndexes["Mode"] = i; i++;
      m_columnHeaderIndexes["Publisher"] = i; i++;
      m_columnHeaderIndexes["Release Date"] = i; i++;
      m_columnHeaderIndexes["Place of Publication"] = i; i++;
      m_columnHeaderIndexes["Genres"] = i; i++;
      m_columnHeaderIndexes["Language"] = i; i++;
      m_columnHeaderIndexes["UPC"] = i; i++;
      m_columnHeaderIndexes["Price"] = i; i++;
      m_columnHeaderIndexes["Monetary Units"] = i; i++;
      m_columnHeaderIndexes["Quantity"] = i; i++;
      m_columnHeaderIndexes["Location"] = i; i++;
      m_columnHeaderIndexes["Availability"] = i; i++;
      m_columnHeaderIndexes["Total Reserved"] = i; i++;
      m_columnHeaderIndexes["Type"] = i; i++;
      m_columnHeaderIndexes["MYOID"] = i; i++;
    }

  setColumnCount(list.size());
  setHorizontalHeaderLabels(list);

  if(type != "All" &&
     type != "All Available" &&
     type != "All Overdue" &&
     type != "All Requested" &&
     type != "All Reserved")
    {
      /*
      ** Hide the Type and MYOID columns.
      */

      setColumnHidden(list.size() - 1, true);
      setColumnHidden(list.size() - 2, true);
    }
  else
    {
      /*
      ** Hide the MYOID and REQUESTOID columns.
      */

      setColumnHidden(list.size() - 1, true);

      if(type == "All Requested")
	setColumnHidden(list.size() - 2, true);
    }

  list.clear();

  QString l_type(type);
  QString indexstr("");

  indexstr.append(username);
  indexstr.append(l_type.replace(" ", "_"));
  indexstr.append("_header_state");

  for(int i = 0; i < hiddenColumns[indexstr].size(); i++)
    setColumnHidden(hiddenColumns[indexstr][i], true);
}

/*
** -- resetTable() --
*/

void main_table::resetTable(const QString &username, const QString &type,
			    const QString &roles)
{
  clear();
  setRowCount(0);
  setColumnCount(0);
  scrollToTop();
  horizontalScrollBar()->setValue(0);

  if(!type.isEmpty())
    setColumns(username, type, roles);

  horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  horizontalHeader()->setSortIndicatorShown(true);
  resizeColumnsToContents();
  horizontalHeader()->setStretchLastSection(true);
  clearSelection();
  setCurrentItem(0);
}

/*
** -- recordColumnHidden() --
*/

void main_table::recordColumnHidden(const QString &username,
				    const QString &type, const int index,
				    const bool hidden)
{
  QString l_type(type);
  QString indexstr("");

  indexstr.append(username);
  indexstr.append(l_type.replace(" ", "_"));
  indexstr.append("_header_state");

  if(hidden)
    {
      if(!hiddenColumns[indexstr].contains(index))
	hiddenColumns[indexstr].append(index);
    }
  else
    (int) hiddenColumns[indexstr].removeAll(index);
}

/*
** -- friendlyStates() --
*/

QHash<QString, QString> main_table::friendlyStates(void) const
{
  QHash<QString, QString> states;

  for(int i = 0; i < hiddenColumns.keys().size(); i++)
    {
      QString state("");

      for(int j = 0; j < hiddenColumns[hiddenColumns.keys().at(i)].size();
	  j++)
	state += QString::number
	  (hiddenColumns[hiddenColumns.keys().at(i)].at(j)).append(",");

      if(state.endsWith(","))
	state = state.mid(0, state.length() - 1);

      states[hiddenColumns.keys().at(i)] = state;
    }

  return states;
}

/*
** -- parseStates() --
*/

void main_table::parseStates(const QHash<QString, QString> &states)
{
  for(int i = 0; i < states.keys().size(); i++)
    {
      QList<int> intList;
      QStringList strList
	(states[states.keys().at(i)].split(",",
					   QString::SkipEmptyParts));

      for(int j = 0; j < strList.size(); j++)
	if(strList.at(j).toInt() >= 0)
	  intList.append(strList.at(j).toInt());

      hiddenColumns[states.keys().at(i)] = intList;
    }
}

/*
** -- columnNumber() --
*/

int main_table::columnNumber(const QString &name)
{
  return m_columnHeaderIndexes.value(name);
}
