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
  grey_literature.setupUi(this);
#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif
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
	  grey_literature.retranslateUi(this);
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
    (grey_literature.author->viewport(), QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (grey_literature.code_a, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (grey_literature.code_b, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (grey_literature.id, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (grey_literature.title, QColor(255, 248, 220));
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
  QString fieldname = "";
  QString str = "";
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
}

void biblioteq_grey_literature::slotSelectImage(void)
{
}

void biblioteq_grey_literature::updateWindow(const int state)
{
  QString str = "";

  if(state == biblioteq::EDITABLE)
    {
      str = QString(tr("BiblioteQ: Modify Grey Literature Entry (")) +
	grey_literature.id->text() + tr(")");
      m_engWindowTitle = "Modify";
    }
  else
    {
      str = QString(tr("BiblioteQ: View Grey Literature Details (")) +
	grey_literature.id->text() + tr(")");
      m_engWindowTitle = "View";
    }

  setWindowTitle(str);
}
