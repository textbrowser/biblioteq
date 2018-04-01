/*
** -- Qt Includes --
*/

#include <QSqlField>
#include <QSqlRecord>

/*
** Includes grey-literature-specific methods.
*/

/*
** -- Local Includes --
*/

#include "biblioteq.h"
#include "biblioteq_grey_literature.h"

extern biblioteq *qmain;

biblioteq_grey_literature::biblioteq_grey_literature(QMainWindow *parentArg,
						     const QString &oidArg,
						     const int rowArg):
  QMainWindow(), biblioteq_item(rowArg)
{
  QGraphicsScene *scene1 = 0;
  QGraphicsScene *scene2 = 0;
  QMenu *menu = 0;
  QString errorstr("");

  if((menu = new(std::nothrow) QMenu(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene1 = new(std::nothrow) QGraphicsScene(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene2 = new(std::nothrow) QGraphicsScene(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  m_isQueryEnabled = false;
  m_oid = oidArg;
  m_parentWid = parentArg;
  m_row = rowArg;
  m_ui.setupUi(this);
  m_ui.resetButton->setMenu(menu);
  connect(m_ui.resetButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Title")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &ID")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Date")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Author(s)")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Client(s)")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Code-A")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Code-B")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Job Number")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Notes")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Location")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Status")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Type")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_ui.date->setDisplayFormat(qmain->publicationDateFormat("greyliterature"));
  m_ui.location->addItems
    (biblioteq_misc_functions::getLocations(qmain->getDB(),
					    "Grey Literature",
					    errorstr));
  qmain->addError
    (QString(tr("Database Error")),
     QString(tr("Unable to retrieve the grey literature locations.")),
     errorstr, __FILE__, __LINE__);
  m_ui.type->addItems
    (biblioteq_misc_functions::getGreyLiteratureTypes(qmain->getDB(),
						      errorstr));
  qmain->addError
    (QString(tr("Database Error")),
     QString(tr("Unable to retrieve the grey literature document types.")),
     errorstr, __FILE__, __LINE__);

  if(m_ui.location->findText(tr("UNKNOWN")) == -1)
    m_ui.location->addItem(tr("UNKNOWN"));

  if(m_ui.type->findText(tr("UNKNOWN")) == -1)
    m_ui.type->addItem(tr("UNKNOWN"));

  QApplication::restoreOverrideCursor();
  updateFont(QApplication::font(), qobject_cast<QWidget *> (this));
  biblioteq_misc_functions::center(this, m_parentWid);
  biblioteq_misc_functions::hideAdminFields(this, qmain->getRoles());
}

biblioteq_grey_literature::~biblioteq_grey_literature()
{
}

void biblioteq_grey_literature::changeEvent(QEvent *event)
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

void biblioteq_grey_literature::closeEvent(QCloseEvent *e)
{
  if(m_engWindowTitle.contains("Create") ||
     m_engWindowTitle.contains("Modify"))
    if(hasDataChanged(this))
      if(QMessageBox::
	 question(this, tr("BiblioteQ: Question"),
		  tr("Your changes have not been saved. Continue closing?"),
		  QMessageBox::Yes | QMessageBox::No,
		  QMessageBox::No) == QMessageBox::No)
	{
	  if(e)
	    e->ignore();

	  return;
	}

  qmain->removeGreyLiterature(this);
}

void biblioteq_grey_literature::duplicate(const QString &p_oid, const int state)
{
  modify(state); // Initial population.
  m_engWindowTitle = "Create";
  m_oid = p_oid;
  setWindowTitle(tr("BiblioteQ: Duplicate Grey Literature Entry"));
}

void biblioteq_grey_literature::highlightRequiredWidgets(void)
{
  biblioteq_misc_functions::highlightWidget
    (m_ui.author->viewport(), QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget(m_ui.code_a, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget(m_ui.code_b, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget(m_ui.id, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (m_ui.job_number, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget(m_ui.title, QColor(255, 248, 220));
}

void biblioteq_grey_literature::insert(void)
{
  slotReset();
  highlightRequiredWidgets();
  m_engWindowTitle = "Create";
  setWindowTitle(tr("BiblioteQ: Create Grey Literature Entry"));
  storeData(this);
  showNormal();
  activateWindow();
  raise();
}

void biblioteq_grey_literature::modify(const int state)
{
  QSqlQuery query(qmain->getDB());
  QString fieldname("");
  QString str("");
  QVariant var;

  if(state == biblioteq::EDITABLE)
    {
      setWindowTitle(tr("BiblioteQ: Modify Grey Literature Entry"));
      m_engWindowTitle = "Modify";
    }
  else
    {
      setWindowTitle(tr("BiblioteQ: View Grey Literature Details"));
      m_engWindowTitle = "View";
    }

  str = m_oid;
  raise();
}

void biblioteq_grey_literature::search(const QString &field,
				       const QString &value)
{
  Q_UNUSED(field);
  Q_UNUSED(value);
}

void biblioteq_grey_literature::slotCancel(void)
{
  close();
}

void biblioteq_grey_literature::slotGo(void)
{
}

void biblioteq_grey_literature::slotPrint(void)
{
  m_html = "<html>";
  m_html += "</html>";
  print(this);
}

void biblioteq_grey_literature::slotPublicationDateEnabled(bool state)
{
  Q_UNUSED(state);
}

void biblioteq_grey_literature::slotQuery(void)
{
}

void biblioteq_grey_literature::slotReset(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(action != 0)
    {
    }
  else
    {
      /*
      ** Reset all.
      */

      if(!m_engWindowTitle.contains("Search"))
	m_ui.author->setPlainText("N/A");
      else
	m_ui.author->clear();

      m_ui.client->clear();
      m_ui.code_a->clear();
      m_ui.code_b->clear();

      if(m_engWindowTitle.contains("Search"))
	m_ui.date->setDate(QDate::fromString("2001", "yyyy"));
      else
	m_ui.date->setDate(QDate::fromString("01/01/2000", "MM/dd/yyyy"));

      m_ui.id->clear();
      m_ui.job_number->clear();
      m_ui.location->setCurrentIndex(0);
      m_ui.notes->clear();
      m_ui.status->clear();
      m_ui.title->clear();
      m_ui.title->setFocus();
      m_ui.type->setCurrentIndex(0);
    }
}

void biblioteq_grey_literature::slotSelectImage(void)
{
}

void biblioteq_grey_literature::updateWindow(const int state)
{
  QString str("");

  if(state == biblioteq::EDITABLE)
    {
      str = QString(tr("BiblioteQ: Modify Grey Literature Entry (")) +
	m_ui.id->text() + tr(")");
      m_engWindowTitle = "Modify";
    }
  else
    {
      str = QString(tr("BiblioteQ: View Grey Literature Details (")) +
	m_ui.id->text() + tr(")");
      m_engWindowTitle = "View";
    }

  setWindowTitle(str);
}
