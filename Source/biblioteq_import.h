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

#ifndef _BIBLIOTEQ_IMPORT_H_
#define _BIBLIOTEQ_IMPORT_H_

#include "ui_biblioteq_import.h"

class QProgressDialog;
class biblioteq;

class biblioteq_import: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_import(biblioteq *parent);
  void reset(void);
  void show(QMainWindow *parent);

 private:
  enum Columns
    {
      BIBLIOTEQ_TABLE_FIELD_NAME = 2,
      CSV_COLUMN_NUMBER = 0,
      CSV_PREVIEW = 1,
      SUBSTITUTE_VALUE = 3
    };

  enum Templates
    {
      // Zero is the literal text Templates.

      TEMPLATE_1 = 1, // Books (All Fields)
      TEMPLATE_2,     // Books (Some Fields)
      TEMPLATE_3      // Patrons
    };

  QMap<int, QPair<QString, QString> > m_mappings;
  QStringList m_previewHeaders; // Ignore QTableWidget.
  Ui_importBrowser m_ui;
  biblioteq *m_qmain;
  void changeEvent(QEvent *event);
  void importBooks(QProgressDialog *progress,
		   QStringList &errors,
		   const Templates importTemplate,
		   const int idIndex, // ISBN-10
		   qint64 *imported,
		   qint64 *notImported);
  void importPatrons(QProgressDialog *progress,
		     QStringList &errors,
		     qint64 *imported,
		     qint64 *notImported);
  void loadPreview(void);

 private slots:
  void slotAddRow(void);
  void slotClose(void);
  void slotDeleteRow(void);
  void slotImport(void);
  void slotRefreshPreview(void);
  void slotReset(void);
  void slotSelectCSVFile(void);
  void slotSetGlobalFonts(const QFont &font);
  void slotTemplates(int index);
};

#endif
