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
  disconnect(ma.cancelButton, SIGNAL(clicked(void)), (qtbook_magazine *) this,
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
  qmain->removeJournal(this);
}

/*
** -- closeEvent() --
*/

void qtbook_journal::closeEvent(QCloseEvent *e)
{
  (void) e;
  qmain->removeJournal(this);
}
