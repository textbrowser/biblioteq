#include "biblioteq.h"
#include "biblioteq_batch_activities.h"

biblioteq_batch_activities::biblioteq_batch_activities(biblioteq *parent):
  QMainWindow(parent)
{
  m_qmain = parent;
  m_ui.setupUi(this);
  connect(m_qmain,
	  SIGNAL(fontChanged(const QFont &)),
	  this,
	  SLOT(slotSetGlobalFonts(const QFont &)));
  connect(m_ui.borrow_add_row,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAddBorrowingRow(void)));
  connect(m_ui.borrow_remove_row,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotDeleteBorrowingRow(void)));
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
}

void biblioteq_batch_activities::changeEvent(QEvent *event)
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
	{
	  break;
	}
      }

  QMainWindow::changeEvent(event);
}

void biblioteq_batch_activities::show(QMainWindow *parent)
{
  static auto resized = false;

  if(!resized && parent)
    resize(qRound(0.50 * parent->size().width()),
	   qRound(0.80 * parent->size().height()));

  resized = true;
  biblioteq_misc_functions::center(this, parent);
  showNormal();
  activateWindow();
  raise();
}

void biblioteq_batch_activities::slotAddBorrowingRow(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  QApplication::restoreOverrideCursor();
}

void biblioteq_batch_activities::slotClose(void)
{
#ifdef Q_OS_ANDROID
  hide();
#else
  close();
#endif
}

void biblioteq_batch_activities::slotDeleteBorrowingRow(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  QApplication::restoreOverrideCursor();
}

void biblioteq_batch_activities::slotReset(void)
{
}

void biblioteq_batch_activities::slotSetGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(auto widget, findChildren<QWidget *> ())
    {
      widget->setFont(font);
      widget->update();
    }

  update();
}
