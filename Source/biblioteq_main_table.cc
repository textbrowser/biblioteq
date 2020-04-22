#include "biblioteq.h"
#include "biblioteq_main_table.h"

#include <QScrollBar>

biblioteq_main_table::biblioteq_main_table(QWidget *parent):
  QTableWidget(parent)
{
  qmain = 0;
  setAcceptDrops(false);
  setDragEnabled(false);
#if QT_VERSION >= 0x050000
  horizontalHeader()->setSectionsMovable(true);
#else
  horizontalHeader()->setMovable(true);
#endif
  horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  horizontalHeader()->setSortIndicatorShown(true);
  horizontalHeader()->setStretchLastSection(true);
#if QT_VERSION >= 0x050000
  verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
  verticalHeader()->setResizeMode(QHeaderView::Fixed);
#endif
}

QHash<QString, QString> biblioteq_main_table::friendlyStates(void) const
{
  QHash<QString, QString> states;

  for(int i = 0; i < m_hiddenColumns.keys().size(); i++)
    {
      QString state("");

      for(int j = 0; j < m_hiddenColumns[m_hiddenColumns.keys().at(i)].size();
	  j++)
	state += QString::number
	  (m_hiddenColumns[m_hiddenColumns.keys().at(i)].at(j)).append(",");

      if(state.endsWith(","))
	state = state.mid(0, state.length() - 1);

      states[m_hiddenColumns.keys().at(i)] = state;
    }

  return states;
}

QStringList biblioteq_main_table::columnNames(void) const
{
  return m_columnHeaderIndexes.toList();
}

int biblioteq_main_table::columnNumber(const QString &name) const
{
  int index = m_columnHeaderIndexes.indexOf(name);

  if(index >= 0)
    return index;

  for(int i = 0; i < m_columnHeaderIndexes.size(); i++)
    if(m_columnHeaderIndexes.at(i).toLower() == name.toLower())
      {
	index = i;
	break;
      }

  return index;
}

void biblioteq_main_table::keyPressEvent(QKeyEvent *event)
{
  if(event)
    switch(event->key())
      {
      case Qt::Key_Enter:
      case Qt::Key_Return:
	{
	  emit enterKeyPressed();
	  break;
	}
      default:
	{
	  break;
	}
      }

  QTableWidget::keyPressEvent(event);
}

void biblioteq_main_table::parseStates(const QHash<QString, QString> &states)
{
  m_hiddenColumns.clear();

  for(int i = 0; i < states.keys().size(); i++)
    {
      QList<int> intList;
      QStringList strList
	(states[states.keys().at(i)].split(",",
					   QString::SkipEmptyParts));

      for(int j = 0; j < strList.size(); j++)
	if(strList.at(j).toInt() >= 0)
	  intList.append(strList.at(j).toInt());

      m_hiddenColumns[states.keys().at(i)] = intList;
    }
}

void biblioteq_main_table::recordColumnHidden(const QString &username,
					      const QString &type,
					      const int index,
					      const bool hidden)
{
  QString indexstr("");
  QString l_type(type);

  indexstr.append(username);
  indexstr.append(l_type.replace(" ", "_"));
  indexstr.append("_header_state");

  if(hidden)
    {
      if(!m_hiddenColumns[indexstr].contains(index))
	m_hiddenColumns[indexstr].append(index);
    }
  else if(m_hiddenColumns.contains(indexstr))
    m_hiddenColumns[indexstr].removeAll(index);
}

void biblioteq_main_table::resetTable(const QString &username,
				      const QString &type,
				      const QString &roles)
{
  setColumnCount(0);
  setRowCount(0);
  scrollToTop();
  horizontalScrollBar()->setValue(0);
  setColumns(username, type, roles);

  if(qmain && qmain->setting("automatically_resize_column_widths").toBool())
    {
      for(int i = 0; i < columnCount() - 1; i++)
	resizeColumnToContents(i);

      horizontalHeader()->setStretchLastSection(true);
    }

  clearSelection();
  horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  horizontalHeader()->setSortIndicatorShown(true);
  setCurrentItem(0);
  sortByColumn(0, Qt::AscendingOrder);
}

void biblioteq_main_table::setColumnNames(const QStringList &list)
{
  m_columnHeaderIndexes.clear();

  for(int i = 0; i < list.size(); i++)
    m_columnHeaderIndexes.append(list.at(i));
}

void biblioteq_main_table::setColumns(const QString &username,
				      const QString &t,
				      const QString &roles)
{
  QString type(t.trimmed());
  QStringList list;

  m_columnHeaderIndexes.clear();

  if(type.isEmpty())
    type = "All";

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
	      list.append(tr("Contact Information"));
	      m_columnHeaderIndexes.append("Borrower");
	      m_columnHeaderIndexes.append("Member ID");
	      m_columnHeaderIndexes.append("Contact Information");
	    }

	  list.append(tr("Barcode"));
	  list.append(tr("Reservation Date"));
	  list.append(tr("Due Date"));
	  m_columnHeaderIndexes.append("Barcode");
	  m_columnHeaderIndexes.append("Reservation Date");
	  m_columnHeaderIndexes.append("Due Date");
	}
      else if(type == "All Requested")
	{
	  if(!roles.isEmpty())
	    {
	      list.append(tr("Borrower"));
	      list.append(tr("Member ID"));
	      list.append(tr("Contact Information"));
	      m_columnHeaderIndexes.append("Borrower");
	      m_columnHeaderIndexes.append("Member ID");
	      m_columnHeaderIndexes.append("Contact Information");
	    }

	  list.append(tr("Request Date"));
	  m_columnHeaderIndexes.append("Request Date");
	}

      list.append(tr("Title"));
      list.append(tr("ID Number"));

      if(type == "All Overdue" ||
	 type == "All Requested" ||
	 type == "All Reserved")
	list.append(tr("Call Number"));

      list.append(tr("Publisher"));
      list.append(tr("Publication Date"));
      list.append(tr("Categories"));
      list.append(tr("Language"));
      list.append(tr("Price"));
      list.append(tr("Monetary Units"));
      list.append(tr("Quantity"));
      list.append(tr("Location"));
      m_columnHeaderIndexes.append("Title");
      m_columnHeaderIndexes.append("ID Number");

      if(type == "All Overdue" ||
	 type == "All Requested" ||
	 type == "All Reserved")
	m_columnHeaderIndexes.append("Call Number");

      m_columnHeaderIndexes.append("Publisher");
      m_columnHeaderIndexes.append("Publication Date");
      m_columnHeaderIndexes.append("Categories");
      m_columnHeaderIndexes.append("Language");
      m_columnHeaderIndexes.append("Price");
      m_columnHeaderIndexes.append("Monetary Units");
      m_columnHeaderIndexes.append("Quantity");
      m_columnHeaderIndexes.append("Location");

      if(type != "All Requested")
	{
	  list.append(tr("Availability"));
	  list.append(tr("Total Reserved"));
	  m_columnHeaderIndexes.append("Availability");
	  m_columnHeaderIndexes.append("Total Reserved");
	}

      list.append(tr("Accession Number"));
      list.append(tr("Type"));
      list.append("MYOID");
      m_columnHeaderIndexes.append("Accession Number");
      m_columnHeaderIndexes.append("Type");
      m_columnHeaderIndexes.append("MYOID");

      if(type == "All Requested")
	{
	  list.append("REQUESTOID");
	  m_columnHeaderIndexes.append("REQUESTOID");
	}
    }
  else if(type == "Books")
    {
      list.append(tr("Title"));
      list.append(tr("Authors"));
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
      list.append(tr("Originality"));
      list.append(tr("Condition"));
      list.append(tr("Accession Number"));
      list.append(tr("Type"));
      list.append("MYOID");
      m_columnHeaderIndexes.append("Title");
      m_columnHeaderIndexes.append("Authors");
      m_columnHeaderIndexes.append("Publisher");
      m_columnHeaderIndexes.append("Publication Date");
      m_columnHeaderIndexes.append("Place of Publication");
      m_columnHeaderIndexes.append("Edition");
      m_columnHeaderIndexes.append("Categories");
      m_columnHeaderIndexes.append("Language");
      m_columnHeaderIndexes.append("ISBN-10");
      m_columnHeaderIndexes.append("Price");
      m_columnHeaderIndexes.append("Monetary Units");
      m_columnHeaderIndexes.append("Quantity");
      m_columnHeaderIndexes.append("Book Binding Type");
      m_columnHeaderIndexes.append("Location");
      m_columnHeaderIndexes.append("ISBN-13");
      m_columnHeaderIndexes.append("LC Control Number");
      m_columnHeaderIndexes.append("Call Number");
      m_columnHeaderIndexes.append("Dewey Class Number");
      m_columnHeaderIndexes.append("Availability");
      m_columnHeaderIndexes.append("Total Reserved");
      m_columnHeaderIndexes.append("Originality");
      m_columnHeaderIndexes.append("Condition");
      m_columnHeaderIndexes.append("Accession Number");
      m_columnHeaderIndexes.append("Type");
      m_columnHeaderIndexes.append("MYOID");
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
      list.append(tr("Accession Number"));
      list.append(tr("Type"));
      list.append("MYOID");
      m_columnHeaderIndexes.append("Title");
      m_columnHeaderIndexes.append("Format");
      m_columnHeaderIndexes.append("Studio");
      m_columnHeaderIndexes.append("Release Date");
      m_columnHeaderIndexes.append("Number of Discs");
      m_columnHeaderIndexes.append("Runtime");
      m_columnHeaderIndexes.append("Categories");
      m_columnHeaderIndexes.append("Language");
      m_columnHeaderIndexes.append("UPC");
      m_columnHeaderIndexes.append("Price");
      m_columnHeaderIndexes.append("Monetary Units");
      m_columnHeaderIndexes.append("Quantity");
      m_columnHeaderIndexes.append("Location");
      m_columnHeaderIndexes.append("Rating");
      m_columnHeaderIndexes.append("Region");
      m_columnHeaderIndexes.append("Aspect Ratio");
      m_columnHeaderIndexes.append("Availability");
      m_columnHeaderIndexes.append("Total Reserved");
      m_columnHeaderIndexes.append("Accession Number");
      m_columnHeaderIndexes.append("Type");
      m_columnHeaderIndexes.append("MYOID");
    }
  else if(type == "Grey Literature")
    {
      list.append(tr("Authors"));
      list.append(tr("Clients"));
      list.append(tr("Document Code A"));
      list.append(tr("Document Code B"));
      list.append(tr("Document Date"));
      list.append(tr("Document ID"));
      list.append(tr("Document Status"));
      list.append(tr("Title"));
      list.append(tr("Document Type"));
      list.append(tr("Job Number"));
      list.append(tr("Location"));
      list.append(tr("File Count"));
      list.append(tr("Type"));
      list.append("MYOID");
      m_columnHeaderIndexes.append("Authors");
      m_columnHeaderIndexes.append("Clients");
      m_columnHeaderIndexes.append("Document Code A");
      m_columnHeaderIndexes.append("Document Code B");
      m_columnHeaderIndexes.append("Document Date");
      m_columnHeaderIndexes.append("Document ID");
      m_columnHeaderIndexes.append("Document Status");
      m_columnHeaderIndexes.append("Title");
      m_columnHeaderIndexes.append("Document Type");
      m_columnHeaderIndexes.append("Job Number");
      m_columnHeaderIndexes.append("Location");
      m_columnHeaderIndexes.append("File Count");
      m_columnHeaderIndexes.append("Type");
      m_columnHeaderIndexes.append("MYOID");
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
      list.append(tr("Accession Number"));
      list.append(tr("Type"));
      list.append("MYOID");
      m_columnHeaderIndexes.append("Title");
      m_columnHeaderIndexes.append("Publisher");
      m_columnHeaderIndexes.append("Publication Date");
      m_columnHeaderIndexes.append("Place of Publication");
      m_columnHeaderIndexes.append("Volume");
      m_columnHeaderIndexes.append("Issue");
      m_columnHeaderIndexes.append("Categories");
      m_columnHeaderIndexes.append("Language");
      m_columnHeaderIndexes.append("ISSN");
      m_columnHeaderIndexes.append("Price");
      m_columnHeaderIndexes.append("Monetary Units");
      m_columnHeaderIndexes.append("Quantity");
      m_columnHeaderIndexes.append("Location");
      m_columnHeaderIndexes.append("LC Control Number");
      m_columnHeaderIndexes.append("Call Number");
      m_columnHeaderIndexes.append("Dewey Number");
      m_columnHeaderIndexes.append("Availability");
      m_columnHeaderIndexes.append("Total Reserved");
      m_columnHeaderIndexes.append("Accession Number");
      m_columnHeaderIndexes.append("Type");
      m_columnHeaderIndexes.append("MYOID");
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
      list.append(tr("Accession Number"));
      list.append(tr("Type"));
      list.append("MYOID");
      m_columnHeaderIndexes.append("Title");
      m_columnHeaderIndexes.append("Artist");
      m_columnHeaderIndexes.append("Format");
      m_columnHeaderIndexes.append("Recording Label");
      m_columnHeaderIndexes.append("Release Date");
      m_columnHeaderIndexes.append("Number of Discs");
      m_columnHeaderIndexes.append("Runtime");
      m_columnHeaderIndexes.append("Categories");
      m_columnHeaderIndexes.append("Language");
      m_columnHeaderIndexes.append("Catalog Number");
      m_columnHeaderIndexes.append("Price");
      m_columnHeaderIndexes.append("Monetary Units");
      m_columnHeaderIndexes.append("Quantity");
      m_columnHeaderIndexes.append("Location");
      m_columnHeaderIndexes.append("Audio");
      m_columnHeaderIndexes.append("Recording Type");
      m_columnHeaderIndexes.append("Availability");
      m_columnHeaderIndexes.append("Total Reserved");
      m_columnHeaderIndexes.append("Accession Number");
      m_columnHeaderIndexes.append("Type");
      m_columnHeaderIndexes.append("MYOID");
    }
  else if(type == "Photograph Collections")
    {
      list.append(tr("Title"));
      list.append(tr("ID"));
      list.append(tr("Location"));
      list.append(tr("Photograph Count"));
      list.append(tr("About"));
      list.append(tr("Accession Number"));
      list.append(tr("Type"));
      list.append("MYOID");
      m_columnHeaderIndexes.append("Title");
      m_columnHeaderIndexes.append("ID");
      m_columnHeaderIndexes.append("Location");
      m_columnHeaderIndexes.append("Photograph Count");
      m_columnHeaderIndexes.append("About");
      m_columnHeaderIndexes.append("Accession Number");
      m_columnHeaderIndexes.append("Type");
      m_columnHeaderIndexes.append("MYOID");
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
      list.append(tr("Accession Number"));
      list.append(tr("Type"));
      list.append("MYOID");
      m_columnHeaderIndexes.append("Title");
      m_columnHeaderIndexes.append("Game Rating");
      m_columnHeaderIndexes.append("Platform");
      m_columnHeaderIndexes.append("Mode");
      m_columnHeaderIndexes.append("Publisher");
      m_columnHeaderIndexes.append("Release Date");
      m_columnHeaderIndexes.append("Place of Publication");
      m_columnHeaderIndexes.append("Genres");
      m_columnHeaderIndexes.append("Language");
      m_columnHeaderIndexes.append("UPC");
      m_columnHeaderIndexes.append("Price");
      m_columnHeaderIndexes.append("Monetary Units");
      m_columnHeaderIndexes.append("Quantity");
      m_columnHeaderIndexes.append("Location");
      m_columnHeaderIndexes.append("Availability");
      m_columnHeaderIndexes.append("Total Reserved");
      m_columnHeaderIndexes.append("Accession Number");
      m_columnHeaderIndexes.append("Type");
      m_columnHeaderIndexes.append("MYOID");
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

  QString indexstr("");
  QString l_type(type);

  indexstr.append(username);
  indexstr.append(l_type.replace(" ", "_"));
  indexstr.append("_header_state");

  for(int i = 0; i < m_hiddenColumns[indexstr].size(); i++)
    setColumnHidden(m_hiddenColumns[indexstr][i], true);
}

void biblioteq_main_table::setQMain(biblioteq *biblioteq)
{
  qmain = biblioteq;
}
