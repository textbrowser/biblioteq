/*
** Includes journal-specific methods.
*/

/*
** Note: The ISSN is not a unique value.
*/

/*
** -- Local Includes --
*/

#include "qtbook_magazine.h"

extern qtbook *qmain;

/*
** -- qtbook_journal() --
*/

qtbook_journal::qtbook_journal(QMainWindow *parent,
			       const QStringList &languages,
			       const QStringList &monetary_units,
			       const QStringList &locations,
			       const QString &oidArg,
			       const int rowArg):
  qtbook_magazine(parent,
		  languages,
		  monetary_units,
		  locations,
		  oidArg,
		  rowArg)
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
  if(windowTitle().contains("Modify"))
    if(hasDataChanged(this))
      if(QMessageBox::question(this, "BiblioteQ: Question",
			       "You have unsaved data. Continue closing?",
			       QMessageBox::Yes | QMessageBox::No,
			       QMessageBox::No) == QMessageBox::No)
	{
	  e->ignore();
	  return;
	}

  qmain->removeJournal(this);
}
