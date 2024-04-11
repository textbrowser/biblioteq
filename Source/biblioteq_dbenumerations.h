/*
** Copyright (c) 2006 - present, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from BiblioteQ without specific prior written permission.
**
** BIBLIOTEQ IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** BIBLIOTEQ, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _BIBLIOTEQ_DBENUMERATIONS_H_
#define _BIBLIOTEQ_DBENUMERATIONS_H_

#include <QPointer>
#include <QStyledItemDelegate>

#include "biblioteq_misc_functions.h"
#include "ui_biblioteq_dbenumerations.h"

class biblioteq;

class biblioteq_dbenumerations: public QMainWindow
{
  Q_OBJECT

 public:
  enum class LocationsTable
    {
      Location = 1,
      Type = 0
    };

  enum class MinimumDaysTable
    {
      Days = 1,
      Type = 0
    };

  enum class Page
    {
      ReservationMinimumDays = 12
    };

  biblioteq_dbenumerations(biblioteq *parent);
  void clear(void);
  void closeEvent(QCloseEvent *event);
  void prepareIcons(void);
  void setPage(const Page page);
  void show(QMainWindow *parent, const bool populate);

 private:
  QHash<QWidget *, QMap<QString, QString> > m_tableData;
  QHash<QWidget *, QStringList> m_listData;
  QPointer<biblioteq> qmain;
  Ui_dbEnumerations m_ui;
  void changeEvent(QEvent *event);
  void populateWidgets(void);
  void saveData(QHash<QWidget *, QStringList> &listData,
		QHash<QWidget *, QMap<QString, QString> > &tableData);

 private slots:
  void setGlobalFonts(const QFont &font);
  void slotAdd(void);
  void slotClose(void);
  void slotReload(void);
  void slotRemove(void);
  void slotSave(void);
  void slotScrollToSection(int index);
};

class biblioteq_dbenumerations_item_delegate: public QStyledItemDelegate
{
  Q_OBJECT

 public:
  biblioteq_dbenumerations_item_delegate(QObject *parent):
    QStyledItemDelegate(parent)
  {
  }

  QWidget *createEditor(QWidget *parent,
			const QStyleOptionViewItem &option,
			const QModelIndex &index) const
  {
    switch(index.column())
      {
      case static_cast<int> (biblioteq_dbenumerations::LocationsTable::Type):
	{
	  auto comboBox = new QComboBox(parent);
	  auto value(index.data().toString());

	  comboBox->addItem(tr("Book"));
	  comboBox->addItem(tr("DVD"));
	  comboBox->addItem(tr("Grey Literature"));
	  comboBox->addItem(tr("Journal"));
	  comboBox->addItem(tr("Magazine"));
	  comboBox->addItem(tr("Music CD"));
	  comboBox->addItem(tr("Photograph Collection"));
	  comboBox->addItem(tr("Video Game"));
	  connect(comboBox,
		  SIGNAL(activated(int)),
		  this,
		  SLOT(slotCurrentIndexChanged(int))
#ifdef Q_OS_MACOS
		  , Qt::QueuedConnection
#endif
		  );
	  biblioteq_misc_functions::sortCombinationBox(comboBox);

	  if(value == tr("Book"))
	    comboBox->setCurrentIndex(comboBox->findText(tr("Book")));
	  else if(value == tr("CD"))
	    comboBox->setCurrentIndex(comboBox->findText(tr("Music CD")));
	  else if(value == tr("DVD"))
	    comboBox->setCurrentIndex(comboBox->findText(tr("DVD")));
	  else if(value == tr("Grey Literature"))
	    comboBox->setCurrentIndex
	      (comboBox->findText(tr("Grey Literature")));
	  else if(value == tr("Journal"))
	    comboBox->setCurrentIndex(comboBox->findText(tr("Journal")));
	  else if(value == tr("Magazine"))
	    comboBox->setCurrentIndex(comboBox->findText(tr("Magazine")));
	  else if(value == tr("Photograph Collection"))
	    comboBox->setCurrentIndex
	      (comboBox->findText(tr("Photograph Collection")));
	  else if(value == tr("Video Game"))
	    comboBox->setCurrentIndex(comboBox->findText(tr("Video Game")));

	  if(comboBox->currentIndex() < 0)
	    comboBox->setCurrentIndex(0);

	  comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	  return comboBox;
	}
      default:
	{
	  break;
	}
      }

    return QStyledItemDelegate::createEditor(parent, option, index);
  }

 private slots:
  void slotCurrentIndexChanged(int index)
  {
    Q_UNUSED(index);

    auto comboBox = qobject_cast<QComboBox *> (sender());

    if(comboBox)
      {
	emit commitData(comboBox);
	emit closeEditor(comboBox); // Order is crucial.
      }
  }
};

#endif
