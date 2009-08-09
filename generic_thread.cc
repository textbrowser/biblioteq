#include "generic_thread.h"

extern qtbook *qmain;

/*
** -- generic_thread() --
*/

generic_thread::generic_thread(void)
{
  type = -1;
  eType = "";
  errorStr = "";
  list.clear();
  outputListBool.clear();
  setTerminationEnabled(true);
}

/*
** -- ~generic_thread() --
*/

generic_thread::~generic_thread()
{
  list.clear();
  LOCResults.clear();
  outputListBool.clear();
}

/*
** -- run() --
*/

void generic_thread::run(void)
{
  /*
  ** Some of this could have been implemented with Qt signals.
  */

  switch(type)
    {
    case READ_GLOBAL_CONFIG_FILE:
      {
	QFile qf;
	myqstring str = "";
	QTextStream qts;

	qf.setFileName(filename);

	if(!qf.open(QIODevice::ReadOnly))
	  {
	    errorStr = tr("Unable to read ");
	    errorStr.append(filename);
	    errorStr.append(tr(". This file is required by BiblioteQ."));
	    return;
	  }

	qts.setDevice(&qf);

	while(!qts.atEnd())
	  {
	    str = str.prepConfigString(qts.readLine().trimmed(), true);

	    // Ignore comments.

	    if(!str.startsWith("#"))
	      list.append(str);
	  }

	qf.close();
	break;
      }
    case Z3950_QUERY:
      {
	size_t i = 0;
	const char *rec = 0;
	ZOOM_resultset zoomResultSet = 0;
	ZOOM_connection zoomConnection = ZOOM_connection_new
	  (static_cast<const char *>
	   ((qmain->getLOCHash().value("Address") + ":" +
	     qmain->getLOCHash().value("Port") + "/" +
	     qmain->getLOCHash().value("Database")).toStdString().data()),
	   0);

	ZOOM_connection_option_set(zoomConnection,
				   "preferredRecordSyntax", "USMARC");
	ZOOM_connection_option_set
	  (zoomConnection,
	   "user", qmain->getLOCHash().value("user").toStdString().data());
	ZOOM_connection_option_set
	  (zoomConnection,
	   "password",
	   qmain->getLOCHash().value("password").toStdString().data());
	zoomResultSet = ZOOM_connection_search_pqf
	  (zoomConnection,
	   static_cast<const char *> (LOCSearchStr.toStdString().data()));

	while((rec = ZOOM_record_get(ZOOM_resultset_record(zoomResultSet, i),
				     "render", 0)) != 0)
	  {
	    i += 1;
	    LOCResults.append(rec);
	  }

	ZOOM_resultset_destroy(zoomResultSet);
	ZOOM_connection_destroy(zoomConnection);

	if(LOCResults.isEmpty())
	  {
	    eType = tr("Query Error");
	    errorStr = tr("Query Error");
	  }

	/*
	  try
	  {
	  ZOOM::connection conn
	  (qmain->getLOCHash().value("Address").toStdString(),
	  qmain->getLOCHash().value("Port").toInt());
	  conn.option
	  ("databaseName",
	  qmain->getLOCHash().value("Database").toStdString());
	  conn.option("preferredRecordSyntax", "USMARC");
	  ZOOM::resultSet rs
	  (conn, ZOOM::prefixQuery(LOCSearchStr.toStdString()));

	  for(i = 0; i < rs.size(); i++)
	  {
	  ZOOM::record rec(rs, i);
	  LOCResults.append(rec.render().data());
	  }
	  }
	  catch(ZOOM::systemException &e)
	  {
	  eType = tr("System Error");
	  errorStr = QString::number(e.errcode()) +
	  QString(" (%1)").arg(e.errmsg().data());
	  }
	  catch(ZOOM::bib1Exception &e)
	  {
	  eType = tr("BIB-1 Error");
	  errorStr = QString::number(e.errcode()) +
	  QString(" (%1): %2").arg(e.errmsg().data()).arg
	  (e.addinfo().data());
	  }
	  catch(ZOOM::queryException &e)
	  {
	  eType = tr("Query Error");
	  errorStr = QString::number(e.errcode()) +
	  QString(" (%1): %2").arg(e.errmsg().data()).arg
	  (e.addinfo().data());
	  }
	  catch(ZOOM::exception &e)
	  {
	  eType = tr("General Error");
	  errorStr = QString::number(e.errcode()) +
	  QString(" (%1)").arg(e.errmsg().data());
	  }
	*/

	break;
      }
    default:
      break;
    }
}

/*
** -- getList() --
*/

QStringList generic_thread::getList(void)
{
  return list;
}

/*
** -- getErrorStr() --
*/

QString generic_thread::getErrorStr(void)
{
  return errorStr;
}

/*
** -- setType() --
*/

void generic_thread::setType(const int type_arg)
{
  type = type_arg;
}

/*
** -- setOutput() --
*/

void generic_thread::setOutputList(const QList<bool> &list_arg)
{
  int i = 0;

  for(i = 0; i < list_arg.size(); i++)
    outputListBool.append(list_arg.at(i));
}

/*
** -- setFilename() --
*/

void generic_thread::setFilename(const QString &filename_arg)
{
  filename = filename_arg;
}

/*
** -- setLOCSearchString() --
*/

void generic_thread::setLOCSearchString(const QString &LOCSearchStr_arg)
{
  LOCSearchStr = LOCSearchStr_arg;
}

/*
** -- getLOCResults() --
*/

QStringList generic_thread::getLOCResults(void)
{
  return LOCResults;
}

/*
** -- getEType() --
*/

QString generic_thread::getEType(void)
{
  return eType;
}

/*
** -- msleep() --
*/

void generic_thread::msleep(const int msecs)
{
  QThread::msleep(msecs);
}
