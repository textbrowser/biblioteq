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
  void show(QMainWindow *parent);

 private:
  enum Columns
    {
     BIBLIOTEQ_TABLE_FIELD_NAME = 1,
     CSV_COLUMN_NUMBER = 0,
     SUBSTITUTE_VALUE = 2
    };

  enum Templates
    {
     // Zero is the text Templates.

     TEMPLATE_1 = 1,
     TEMPLATE_2,
     TEMPLATE_3
    };

  QMap<int, QPair<QString, QString> > m_mappings;
  Ui_importBrowser m_ui;
  biblioteq *m_qmain;
  void changeEvent(QEvent *event);
  void importBooks(QProgressDialog *progress,
		   QStringList &errors,
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
