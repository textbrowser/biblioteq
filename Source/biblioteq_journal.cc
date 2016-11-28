/*
** Includes journal-specific methods.
*/

/*
** Note: The ISSN is not a unique value.
*/

/*
** -- Local Includes --
*/

#include "biblioteq.h"
#include "biblioteq_magazine.h"

extern biblioteq *qmain;

/*
** -- biblioteq_journal() --
*/

biblioteq_journal::biblioteq_journal(QMainWindow *parent,
				     const QString &oidArg,
				     const int rowArg):
  biblioteq_magazine(parent,
		     oidArg,
		     rowArg,
		     "journal")
{
  m_subType = "Journal";
  ma.publication_date->setDisplayFormat
    (qmain->publicationDateFormat("journals"));
  disconnect(ma.cancelButton, SIGNAL(clicked(void)),
	     static_cast<biblioteq_magazine *> (this),
	     SLOT(slotCancel(void)));
  connect(ma.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCancel(void)));
}

/*
** -- ~biblioteq_journal() --
*/

biblioteq_journal::~biblioteq_journal()
{
}

/*
** -- slotCancel() --
*/

void biblioteq_journal::slotCancel(void)
{
  close();
}

/*
** -- closeEvent() --
*/

void biblioteq_journal::closeEvent(QCloseEvent *e)
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

  qmain->removeJournal(this);
}

/*
** -- changeEvent() --
*/

void biblioteq_journal::changeEvent(QEvent *event)
{
  if(event)
    switch(event->type())
      {
      case QEvent::LanguageChange:
	{
	  ma.retranslateUi(this);
	  break;
	}
      default:
	break;
      }

  QMainWindow::changeEvent(event);
}
