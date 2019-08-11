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

biblioteq_journal::biblioteq_journal(biblioteq *parent,
				     const QString &oidArg,
				     const int rowArg):
  biblioteq_magazine(parent, oidArg, rowArg, "journal")
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

biblioteq_journal::~biblioteq_journal()
{
}

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

void biblioteq_journal::closeEvent(QCloseEvent *event)
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
	  if(event)
	    event->ignore();

	  return;
	}

  qmain->removeJournal(this);
}

void biblioteq_journal::slotCancel(void)
{
  close();
}
