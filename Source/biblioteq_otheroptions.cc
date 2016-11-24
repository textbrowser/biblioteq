/*
** -- Qt Includes --
*/

#include <QSettings>

/*
** -- Local Includes --
*/

#include "biblioteq.h"
#include "biblioteq_otheroptions.h"

extern biblioteq *qmain;

/*
** -- biblioteq_otheroptions() --
*/

biblioteq_otheroptions::biblioteq_otheroptions(void):QMainWindow()
{
  m_ui.setupUi(this);
#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
  prepareSettings();
}

/*
** -- ~biblioteq_otheroptions() --
*/

biblioteq_otheroptions::~biblioteq_otheroptions()
{
}

/*
** -- dateFormat() --
*/

QString biblioteq_otheroptions::dateFormat(const QString &itemType) const
{
  for(int i = 0; i < m_ui.publication_date->rowCount(); i++)
    if(m_ui.publication_date->item(i, 0) &&
       itemType.toLower().trimmed() ==
       m_ui.publication_date->item(i, 0)->text().toLower())
      {
	if(m_ui.publication_date->item(i, 1))
	  return m_ui.publication_date->item(i, 1)->text();

	break;
      }

  return "MM/dd/yyyy";
}

/*
** -- changeEvent() --
*/

void biblioteq_otheroptions::changeEvent(QEvent *event)
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

/*
** -- closeEvent() --
*/

void biblioteq_otheroptions::closeEvent(QCloseEvent *event)
{
  QMainWindow::closeEvent(event);
}

/*
** -- keyPressEvent() --
*/

void biblioteq_otheroptions::keyPressEvent(QKeyEvent *event)
{
  if(event && event->key() == Qt::Key_Escape)
    close();

  QMainWindow::keyPressEvent(event);
}

/*
** -- prepareSettings() --
*/

void biblioteq_otheroptions::prepareSettings(void)
{
  m_ui.publication_date->clearContents();

  QSettings settings;
  QStringList list1;
  QStringList list2;

  list1 << tr("Books")
	<< tr("DVDs")
	<< tr("Journals")
	<< tr("Magazines")
	<< tr("Music CDs")
	<< tr("Photographs")
	<< tr("Video Games");
  list2 << settings.value("otheroptions/book_publication_date_format").
           toString()
	<< settings.value("otheroptions/dvd_publication_date_format").toString()
	<< settings.value("otheroptions/journal_publication_date_format").
           toString()
	<< settings.value("otheroptions/magazine_publication_date_format").
           toString()
    	<< settings.value("otheroptions/cd_publication_date_format").toString()
	<< settings.value("otheroptions/photograph_publication_date_format").
           toString()
	<< settings.value("otheroptions/videogame_publication_date_format").
           toString();
  m_ui.publication_date->setSortingEnabled(false);
  m_ui.publication_date->setRowCount(list1.size());

  for(int i = 0; i < list1.size(); i++)
    {
      QString str(list2.at(i).trimmed());

      if(!(str == "MM/dd/yyyy" ||
	   str == "MM/dd" ||
	   str == "MM"))
	str = "MM/dd/yyyy";

      QComboBox *comboBox = new(std::nothrow) QComboBox();
      QTableWidgetItem *item = new(std::nothrow) QTableWidgetItem
	(list1.at(i));

      if(!comboBox || !item)
	{
	  qmain->addError(QString(tr("Memory Error")),
			  QString(tr("Unable to allocate memory for "
				     "\"comboBox\" or \"item\". "
				     "This is a serious problem!")),
			  QString(""), __FILE__, __LINE__);
	  continue;
	}

      comboBox->addItems(QStringList() << "MM/dd/yyyy"
			               << "MM/dd"
			               << "MM");

      if(comboBox->findText(str) >= 0)
	comboBox->setCurrentIndex(comboBox->findText(str));
      else
	comboBox->setCurrentIndex(0);

      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.publication_date->setItem(i, 0, item);
      m_ui.publication_date->setCellWidget(i, 1, comboBox);
    }

  m_ui.publication_date->resizeColumnToContents(0);
  m_ui.publication_date->setSortingEnabled(true);
  m_ui.publication_date->horizontalHeader()->
    setSortIndicator(0, Qt::AscendingOrder);
  m_ui.publication_date->horizontalHeader()->setSortIndicatorShown(true);
}

/*
** -- setGlobalFonts() --
*/

void biblioteq_otheroptions::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(QWidget *widget, findChildren<QWidget *> ())
    widget->setFont(font);
}

/*
** -- showNormal() --
*/

void biblioteq_otheroptions::showNormal(void)
{
  prepareSettings();
  QMainWindow::showNormal();
}

/*
** -- slotClose() --
*/

void biblioteq_otheroptions::slotClose(void)
{
  close();
}
