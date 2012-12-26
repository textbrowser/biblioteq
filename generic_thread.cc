/*
** -- Qt Includes --
*/

#include <QMutex>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QNetworkAccessManager>

/*
** -- Local Includes --
*/

#include "qtbook.h"
#include "generic_thread.h"

extern qtbook *qmain;

/*
** -- generic_thread() --
*/

generic_thread::generic_thread(QObject *parent):QThread(parent)
{
  m_type = -1;
  m_eType = "";
  m_errorStr = "";
  m_sruName = "";
  m_z3950Name = "";
  setTerminationEnabled(true);
}

/*
** -- ~generic_thread() --
*/

generic_thread::~generic_thread()
{
  m_list.clear();
  m_z3950Results.clear();
  m_outputListBool.clear();
}

/*
** -- run() --
*/

void generic_thread::run(void)
{
  /*
  ** Some of this could have been implemented with Qt signals.
  */

  switch(m_type)
    {
    case READ_GLOBAL_CONFIG_FILE:
      {
	QFile qf;
	myqstring str = "";
	QTextStream qts;

	qf.setFileName(m_filename);

	if(!qf.open(QIODevice::ReadOnly))
	  {
	    m_errorStr = tr("Unable to read ");
	    m_errorStr.append(m_filename);
	    m_errorStr.append(tr(". This file is required by BiblioteQ."));
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
    case SRU_QUERY:
      {
	QMutex mutex;

	mutex.lock();
	m_sruCondition.wait(&mutex, 30000);
	mutex.unlock();
	break;
      }
    case Z3950_QUERY:
      {
	size_t i = 0;
	const char *rec = 0;
	ZOOM_options options = ZOOM_options_create();
	ZOOM_resultset zoomResultSet = 0;
	ZOOM_connection zoomConnection = 0;
	QHash<QString, QString> proxy(qmain->getZ3950Maps().
				      value(m_z3950Name));

	ZOOM_options_set
	  (options,
	   "databaseName",
	   qmain->getZ3950Maps()[m_z3950Name].value("Database").
	   toAscii().constData());
	ZOOM_options_set(options, "preferredRecordSyntax", "MARC21");

	if(!proxy.value("proxy_host").isEmpty() &&
	   !proxy.value("proxy_port").isEmpty())
	  {
	    QString value(QString("%1:%2").
			  arg(proxy["proxy_host"]).
			  arg(proxy["proxy_port"]));

	    ZOOM_options_set(options, "proxy", value.toAscii().constData());
	  }

	if(!qmain->getZ3950Maps()[m_z3950Name].value("Userid").isEmpty())
	  ZOOM_options_set
	    (options,
	     "user",
	     qmain->getZ3950Maps()[m_z3950Name].value("Userid").toAscii().
	     constData());

	if(!qmain->getZ3950Maps()[m_z3950Name].value("Password").isEmpty())
	  ZOOM_options_set
	    (options,
	     "password",
	     qmain->getZ3950Maps()[m_z3950Name].value("Password").toAscii().
	     constData());

	zoomConnection = ZOOM_connection_create(options);
	ZOOM_connection_connect
	  (zoomConnection, (qmain->getZ3950Maps()[m_z3950Name].
			    value("Address") +
			    ":" +
			    qmain->getZ3950Maps()[m_z3950Name].value("Port")).
	   toAscii().constData(), 0);
 	zoomResultSet = ZOOM_connection_search_pqf
	  (zoomConnection,
	   m_z3950SearchStr.toAscii().constData());

	QString format = qmain->getZ3950Maps()[m_z3950Name].value("Format").
	  trimmed().toLower();

	if(format.isEmpty())
	  format = "render";
	else
	  format.prepend("render; charset=");

	while((rec = ZOOM_record_get(ZOOM_resultset_record(zoomResultSet, i),
				     format.toAscii().constData(), 0)) != 0)
	  {
	    i += 1;
	    m_z3950Results.append(QString::fromUtf8(rec));
	  }

	ZOOM_resultset_destroy(zoomResultSet);

	const char *errmsg;
	const char *addinfo;

	if(ZOOM_connection_error(zoomConnection, &errmsg, &addinfo) != 0)
	  {
	    m_eType = errmsg;
	    m_errorStr = addinfo;
	  }
	else if(m_z3950Results.isEmpty())
	  {
	    m_eType = tr("Z39.50 Empty Results Set");
	    m_errorStr = tr("Z39.50 Empty Results Set");
	  }

	ZOOM_options_destroy(options);
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
  return m_list;
}

/*
** -- getErrorStr() --
*/

QString generic_thread::getErrorStr(void) const
{
  return m_errorStr;
}

/*
** -- setType() --
*/

void generic_thread::setType(const int type)
{
  m_type = type;
}

/*
** -- setOutput() --
*/

void generic_thread::setOutputList(const QList<bool> &list)
{
  int i = 0;

  for(i = 0; i < list.size(); i++)
    m_outputListBool.append(list.at(i));
}

/*
** -- setFilename() --
*/

void generic_thread::setFilename(const QString &filename)
{
  m_filename = filename;
}

/*
** -- setZ3950SearchString() --
*/

void generic_thread::setZ3950SearchString(const QString &z3950SearchStr)
{
  m_z3950SearchStr = z3950SearchStr;
}

/*
** -- getZ3950Results() --
*/

QStringList generic_thread::getZ3950Results(void) const
{
  return m_z3950Results;
}

/*
** -- getEType() --
*/

QString generic_thread::getEType(void) const
{
  return m_eType;
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
  m_z3950Name = name;
}

/*
** -- setSRUName() --
*/

void generic_thread::setSRUName(const QString &name)
{
  m_sruName = name;
}

/*
** -- getSRUResults() --
*/

QByteArray generic_thread::getSRUResults(void) const
{
  return m_sruResults;
}

/*
** -- setSRUSearchString() --
*/

void generic_thread::setSRUSearchString(const QString &sruSearchStr)
{
  m_sruSearchStr = sruSearchStr;
}

/*
** -- start() --
*/

void generic_thread::start(void)
{
  if(m_type == SRU_QUERY)
    {
      QNetworkAccessManager *manager = findChild<QNetworkAccessManager *> ();

      if(manager)
	return;

      if(manager->findChild<QNetworkReply *> ())
	return;

      if((manager = new(std::nothrow) QNetworkAccessManager(this)) == 0)
	return;

      QUrl url(QUrl::fromUserInput(m_sruSearchStr));
      QString type("");
      QNetworkProxy proxy;
      QHash<QString, QString> hash(qmain->getSRUMaps()[m_sruName]);

      if(hash.contains("proxy_type"))
	type = hash["proxy_type"].toLower().trimmed();
      else if(hash.contains("proxy_type"))
	type = hash["proxy_type"].toLower().trimmed();

      if(type == "none")
	proxy.setType(QNetworkProxy::NoProxy);
      else
	{
	  if(type == "http" || type == "socks5" || type == "system")
	    {
	      /*
	      ** This is required to resolve an odd error.
	      */

	      QNetworkReply *reply = manager->get
		(QNetworkRequest(QUrl::fromUserInput("http://0.0.0.0")));

	      if(reply)
		reply->deleteLater();
	    }

	  if(type == "http" || type == "socks5")
	    {
	      if(type == "http")
		proxy.setType(QNetworkProxy::HttpProxy);
	      else
		proxy.setType(QNetworkProxy::Socks5Proxy);

	      quint16 port = 0;
	      QString host("");
	      QString user("");
	      QString password("");

	      host = hash["proxy_host"];
	      port = hash["proxy_port"].toUShort();
	      user = hash["proxy_username"];
	      password = hash["proxy_password"];
	      proxy.setHostName(host);
	      proxy.setPort(port);

	      if(!user.isEmpty())
		proxy.setUser(user);

	      if(!password.isEmpty())
		proxy.setPassword(password);

	      manager->setProxy(proxy);
	    }
	  else if(type == "system")
	    {
	      QNetworkProxyQuery query(url);
	      QList<QNetworkProxy> list
		(QNetworkProxyFactory::systemProxyForQuery(query));

	      if(!list.isEmpty())
		proxy = list.at(0);

	      manager->setProxy(proxy);
	    }
	}

      QNetworkReply *reply = manager->get(QNetworkRequest(url));

      if(!reply)
	{
	  manager->deleteLater();
	  return;
	}
      else
	{
	  connect(reply, SIGNAL(readyRead(void)),
		  this, SLOT(slotReadyRead(void)));
	  connect(reply, SIGNAL(finished(void)),
		  this, SLOT(slotDownloadFinished(void)));
	}
    }

  QThread::start();
}

/*
** -- slotReadyRead() --
*/

void generic_thread::slotReadyRead(void)
{
  QNetworkReply *reply = qobject_cast<QNetworkReply *> (sender());

  if(reply)
    m_sruResults.append(reply->readAll());
}

/*
** -- slotDownloadFinished() --
*/

void generic_thread::slotDownloadFinished(void)
{
  m_sruCondition.wakeAll();
}
