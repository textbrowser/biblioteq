#ifndef _BIBLIOTEQ_COPY_EDITOR_H_
#define _BIBLIOTEQ_COPY_EDITOR_H_

#include "ui_biblioteq_copybrowser.h"

class QSpinBox;
class biblioteq;
class biblioteq_item;

class biblioteq_copy_editor: public QDialog
{
  Q_OBJECT

 public:
  biblioteq_copy_editor(QWidget *parent, biblioteq *biblioteq);
  biblioteq_copy_editor(QWidget *parent,
			biblioteq *biblioteq,
			biblioteq_item *bitemArg,
			const bool showForLendingArg,
			const int quantityArg,
			const QString &ioidArg,
			QSpinBox *spinboxArg,
			const QFont &font,
			const QString &itemTypeArg,
			const QString &uniqueIdArg);
  virtual ~biblioteq_copy_editor();
  void populateCopiesEditor(void);

 private:
  class copy_class
  {
  public:
    copy_class(const QString &copyid,
	       const QString &itemoid,
	       const QString &status)
    {
      m_copyid = copyid;
      m_itemoid = itemoid;
      m_status = status;
    };

  public:
    QString m_copyid;
    QString m_itemoid;
    QString m_status;
  };

  enum Columns
    {
     AVAILABILITY = 2,
     BARCODE = 1,
     COPY_NUMBER = 5,
     MYOID = 4,
     STATUS = 3,
     TITLE = 0
    };

  QList<copy_class *> m_copies;
  QSpinBox *m_spinbox;
  QString m_ioid;
  QString m_itemType;
  QVector<QString> m_columnHeaderIndexes;
  QWidget *m_parent;
  Ui_copybrowser m_cb;
  biblioteq *qmain;
  biblioteq_item *m_bitem;
  bool m_showForLending;
  int m_quantity;
  QString saveCopies(void);
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);

 protected:
  void clearCopiesList(void);
  void setGlobalFonts(const QFont &font);

 protected slots:
  void slotCloseCopyEditor(void);

 private slots:
  void slotCheckoutCopy(void);
  void slotDeleteCopy(void);
  void slotSaveCopies(void);
};

#endif
