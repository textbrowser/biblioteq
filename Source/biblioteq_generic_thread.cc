#include "biblioteq.h"
#include "biblioteq_generic_thread.h"

#include <QFile>
#include <QTextStream>

extern "C"
{
#ifdef BIBLIOTEQ_LINKED_WITH_YAZ
#include <yaz/zoom.h>
#endif
}

biblioteq_generic_thread::biblioteq_generic_thread(QObject *parent,
						   biblioteq *biblioteq):
  QThread(parent)
{
  m_eType = "";
  m_errorStr = "";
  m_type = -1;
  m_z3950Name = "";
  qmain = biblioteq;
  setTerminationEnabled(true);
}

biblioteq_generic_thread::~biblioteq_generic_thread()
{
  m_list.clear();
  m_outputListBool.clear();
  m_z3950Results.clear();
  wait();
}

QString biblioteq_generic_thread::getEType(void) const
{
  return m_eType;
}

QString biblioteq_generic_thread::getErrorStr(void) const
{
  return m_errorStr;
}

QStringList biblioteq_generic_thread::getList(void) const
{
  return m_list;
}

QStringList biblioteq_generic_thread::getZ3950Results(void) const
{
  return m_z3950Results;
}

void biblioteq_generic_thread::msleep(const unsigned long msecs)
{
  QThread::msleep(msecs);
}

void biblioteq_generic_thread::run(void)
{
  /*
  ** Some of this could have been implemented with Qt signals.
  */

  switch(m_type)
    {
    case READ_GLOBAL_CONFIG_FILE:
      {
	QFile qf;
	QTextStream qts;
	biblioteq_myqstring str = "";

	if(!m_filename.trimmed().isEmpty())
	  qf.setFileName(m_filename);

	if(m_filename.trimmed().isEmpty() || !qf.open(QIODevice::ReadOnly))
	  {
	    if(m_filename.trimmed().isEmpty())
	      m_errorStr = tr("The configuration file "
			      "(typically biblioteq.conf) cannot be read.");
	    else
	      m_errorStr = tr
		("Unable to read %1. This file is required by BiblioteQ.").
		arg(m_filename);

	    return;
	  }

	qts.setDevice(&qf);

	while(!qts.atEnd())
	  {
	    str = str.prepConfigString(qts.readLine().trimmed(), true);
	    m_list.append(str);
	  }

	qf.close();
	break;
      }
    case Z3950_QUERY:
      {
#ifdef BIBLIOTEQ_LINKED_WITH_YAZ
	ZOOM_connection zoomConnection = nullptr;
	ZOOM_resultset zoomResultSet = nullptr;
	auto hash(qmain->getZ3950Hash(m_z3950Name));
	auto options = ZOOM_options_create();
	auto recordSyntax(hash.value("RecordSyntax").trimmed());
	const char *rec = nullptr;
	size_t i = 0;

	ZOOM_options_set
	  (options,
	   "databaseName",
	   hash.value("Database").toLatin1().constData());
	ZOOM_options_set
	  (options,
	   "timeout",
	   hash.value("timeout", "30").toLatin1().

	if(recordSyntax.isEmpty())
	  ZOOM_options_set(options, "preferredRecordSyntax", "MARC21");
	else
	  ZOOM_options_set(options, "preferredRecordSyntax", recordSyntax.
			   toLatin1().constData());

	if(!hash.value("proxy_host").isEmpty() &&
	   !hash.value("proxy_port").isEmpty())
	  {
	    auto value(QString("%1:%2").
		       arg(hash.value("proxy_host")).
		       arg(hash.value("proxy_port")));

	    ZOOM_options_set(options, "proxy", value.toLatin1().constData());
	  }

	if(!hash.value("Userid").isEmpty())
	  ZOOM_options_set
	    (options,
	     "user",
	     hash.value("Userid").toLatin1().constData());

	if(!hash.value("Password").isEmpty())
	  ZOOM_options_set
	    (options,
	     "password",
	     hash.value("Password").toLatin1().constData());

	zoomConnection = ZOOM_connection_create(options);
	ZOOM_connection_connect
	  (zoomConnection, (hash.value("Address") + ":" +
			    hash.value("Port")).toLatin1().constData(), 0);
 	zoomResultSet = ZOOM_connection_search_pqf
	  (zoomConnection,
	   m_z3950SearchStr.toLatin1().constData());

	auto format = hash.value("Format").trimmed().toLower();

	if(format.isEmpty())
	  format = "render";
	else
	  format.prepend("render; charset=");

	while((rec = ZOOM_record_get(ZOOM_resultset_record(zoomResultSet, i),
				     format.toLatin1().constData(), nullptr)) !=
	      nullptr)
	  {
	    i += 1;
	    m_z3950Results.append(QString::fromUtf8(rec));
	  }

	ZOOM_resultset_destroy(zoomResultSet);

	if(i == 0)
	  {
	    const char *addinfo;
	    const char *errmsg;

	    if(ZOOM_connection_error(zoomConnection, &errmsg, &addinfo) != 0)
	      {
		m_eType = errmsg;
		m_errorStr = addinfo;

		if(m_errorStr.isEmpty())
		  m_errorStr = m_eType;
	      }
	    else if(m_z3950Results.isEmpty())
	      {
		m_eType = tr("Z39.50 Empty Results Set");
		m_errorStr = tr("Z39.50 Empty Results Set");
	      }
	  }

	ZOOM_options_destroy(options);
	ZOOM_connection_destroy(zoomConnection);
#endif
	break;
      }
    default:
      break;
    }
}

void biblioteq_generic_thread::setFilename(const QString &filename)
{
  m_filename = filename;
}

void biblioteq_generic_thread::setOutputList(const QList<bool> &list)
{
  int i = 0;

  for(i = 0; i < list.size(); i++)
    m_outputListBool.append(list.at(i));
}

void biblioteq_generic_thread::setType(const int type)
{
  m_type = type;
}

void biblioteq_generic_thread::setZ3950Name(const QString &name)
{
  m_z3950Name = name.trimmed();
}

void biblioteq_generic_thread::setZ3950SearchString
(const QString &z3950SearchStr)
{
  m_z3950SearchStr = z3950SearchStr.trimmed();
}
