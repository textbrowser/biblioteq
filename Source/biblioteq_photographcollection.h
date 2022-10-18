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

#ifndef _BIBLIOTEQ_PHOTOGRAPHCOLLECTION_H_
#define _BIBLIOTEQ_PHOTOGRAPHCOLLECTION_H_

#include "biblioteq_item.h"
#include "ui_biblioteq_photograph.h"
#include "ui_biblioteq_photographinfo.h"

class biblioteq_bgraphicsscene;

class biblioteq_photographcollection: public QMainWindow, public biblioteq_item
{
  Q_OBJECT

 public:
  biblioteq_photographcollection(biblioteq *parentArg,
				 const QString &oidArg,
				 const QModelIndex &index);
  ~biblioteq_photographcollection();
  void duplicate(const QString &p_oid, const int state);
  void insert(void);
  void modify(const int state, const QString &behavior = "");
  void search(const QString &field = "", const QString &value = "");

  void setPublicationDateFormat(const QString &dateFormat)
  {
    pc.publication_date->setDisplayFormat(dateFormat);
  }

  void updateWindow(const int state);

 private:
  QDialog *m_photo_diag;
  QString m_engWindowTitle;
  QString m_itemOid;
  Ui_pcDialog pc;
  Ui_photographDialog photo;
  biblioteq_bgraphicsscene *m_scene;
  bool verifyItemFields(void);
  int photographsPerPage(void);
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void loadPhotographFromItem(QGraphicsScene *scene,
			      QGraphicsPixmapItem *item,
			      const int percent);
  void loadPhotographFromItemInNewWindow(QGraphicsPixmapItem *item);
  void setSceneRect(const int size);
  void showPhotographs(const int page);
  void storeData(void);
  void updateTablePhotographCount(const int count);

 private slots:
  void slotAddItem(void);
  void slotCancel(void);
  void slotClosePhoto(void);
  void slotDeleteItem(void);
  void slotExportItem(void);
  void slotExportPhotographs(void);
  void slotGo(void);
  void slotImageViewSizeChanged(int index);
  void slotImportItems(void);
  void slotInsertItem(void);
  void slotModifyItem(void);
  void slotPageChanged(int index);
  void slotPrint(void);
  void slotQuery(void);
  void slotReset(void);
  void slotSaveRotatedImage(const QImage &image,
			    const QString &format,
			    const qint64 oid);
  void slotSceneSelectionChanged(void);
  void slotSelectAll(void);
  void slotSelectImage(void);
  void slotUpdateItem(void);
  void slotViewContextMenu(const QPoint &pos);
  void slotViewNextPhotograph(void);
  void slotViewPhotograph(void);
  void slotViewPreviousPhotograph(void);
};

#endif
