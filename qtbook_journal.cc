/*
** Includes journal-specific methods.
*/

/*
** Note: The ISSN is not a unique value.
*/

/*
** -- Local Includes --
*/

#include "qtbook.h"
#include "qtbook_magazine.h"

extern qtbook *qmain;

/*
** -- qtbook_journal() --
*/

qtbook_journal::qtbook_journal(QMainWindow *parent,
			       const QString &oidArg,
			       const int rowArg):
  qtbook_magazine(parent,
		  oidArg,
		  rowArg,
		  "journal")
{
  subType = "Journal";
  disconnect(ma.cancelButton, SIGNAL(clicked(void)),
	     static_cast<qtbook_magazine *> (this),
	     SLOT(slotCancel(void)));
  connect(ma.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCancel(void)));
}

/*
** -- ~qtbook_journal() --
*/

qtbook_journal::~qtbook_journal()
{
}

/*
** -- slotCancel() --
*/

void qtbook_journal::slotCancel(void)
{
  close();
}

/*
** -- closeEvent() --
*/

void qtbook_journal::closeEvent(QCloseEvent *e)
{
  if(engWindowTitle.contains("Create") ||
     engWindowTitle.contains("Modify"))
    if(hasDataChanged(this))
      if(QMessageBox::question(this, tr("BiblioteQ: Question"),
			       tr("You have unsaved data. Continue closing?"),
			       QMessageBox::Yes | QMessageBox::No,
			       QMessageBox::No) == QMessageBox::No)
	{
	  if(e)
	    e->ignore();

	  return;
	}

  qmain->removeJournal(this);
  QMainWindow::closeEvent(e);
}

/*
** -- changeEvent() --
*/

void qtbook_journal::changeEvent(QEvent *event)
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
