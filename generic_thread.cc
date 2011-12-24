#include "generic_thread.h"

extern qtbook *qmain;

/*
** -- generic_thread() --
*/

generic_thread::generic_thread(QObject *parent):QThread(parent)
{
  type = -1;
  eType = "";
  errorStr = "";
  list.clear();
  outputListBool.clear();
  setTerminationEnabled(true);
  z3950Name = "";
}

/*
** -- ~generic_thread() --
*/

generic_thread::~generic_thread()
{
  list.clear();
  z3950Results.clear();
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
	  ((qmain->getZ3950Maps()[z3950Name].value("Address") + ":" +
	    qmain->getZ3950Maps()[z3950Name].value("Port") + "/" +
	    qmain->getZ3950Maps()[z3950Name].value("Database")).
	   toUtf8().constData(), 0);

	ZOOM_connection_option_set(zoomConnection,
				   "preferredRecordSyntax", "MARC21");
	ZOOM_connection_option_set
	  (zoomConnection,
	   "user",
	   qmain->getZ3950Maps()[z3950Name].value("Userid").toUtf8().
	   constData());
	ZOOM_connection_option_set
	  (zoomConnection,
	   "password",
	   qmain->getZ3950Maps()[z3950Name].value("Password").
	   toUtf8().constData());
 	zoomResultSet = ZOOM_connection_search_pqf
	  (zoomConnection,
	   z3950SearchStr.toUtf8().constData());

	while((rec = ZOOM_record_get(ZOOM_resultset_record(zoomResultSet, i),
				     "render", 0)) != 0)
	  {
	    i += 1;
	    z3950Results.append(rec);
	  }

	ZOOM_resultset_destroy(zoomResultSet);

	const char *errmsg = 0;
	const char *addinfo = 0;

	if(ZOOM_connection_error(zoomConnection, &errmsg, &addinfo) != 0)
	  {
	    eType = errmsg;
	    errorStr = addinfo;
	  }
	else if(z3950Results.isEmpty())
	  {
	    eType = tr("Z39.50 Empty Results Set");
	    errorStr = tr("Z39.50 Empty Results Set");
	  }

	ZOOM_connection_destroy(zoomConnection);
	break;
      }
    default:
      break;
    }
}

/*
** -- getList() --
*/

QStringList generic_thread::getList(void) const
{
  return list;
}

/*
** -- getErrorStr() --
*/

QString generic_thread::getErrorStr(void) const
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
** -- setZ3950SearchString() --
*/

void generic_thread::setZ3950SearchString(const QString &z3950SearchStr_arg)
{
  z3950SearchStr = z3950SearchStr_arg;
}

/*
** -- getZ3950Results() --
*/

QStringList generic_thread::getZ3950Results(void) const
{
  return z3950Results;
}

/*
** -- getEType() --
*/

QString generic_thread::getEType(void) const
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

/*
** -- setZ3950Name() --
*/

void generic_thread::setZ3950Name(const QString &name)
{
  z3950Name = name;
}
