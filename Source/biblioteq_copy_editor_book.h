#ifndef _BIBLIOTEQ_COPY_EDITOR_BOOK_H_
#define _BIBLIOTEQ_COPY_EDITOR_BOOK_H_

#include <QPointer>

#include "biblioteq_copy_editor.h"
#include "ui_biblioteq_bookcopybrowser.h"

class biblioteq_copy_editor_book: public biblioteq_copy_editor
{
  Q_OBJECT

 public:
  biblioteq_copy_editor_book(QWidget *parent,
			     biblioteq *biblioteq,
			     biblioteq_item *bitemArg,
			     const bool showForLendingArg,
			     const int quantityArg,
			     const QString &ioidArg,
			     QSpinBox *spinboxArg,
			     const QFont &font,
			     const QString &uniqueIdArg);
  ~biblioteq_copy_editor_book();
  void populateCopiesEditor(void);

 private:
  class copy_class
  {
  public:
    copy_class(const QString &condition,
	       const QString &copyid,
	       const QString &copynumber,
	       const QString &itemoid,
	       const QString &originality,
	       const QString &status)
    {
      m_condition = condition;
      m_copyid = copyid;
      m_copynumber = copynumber;
      m_itemoid = itemoid;
      m_originality = originality;
      m_status = status;
    };

  public:
    QString m_condition;
    QString m_copyid;
    QString m_copynumber;
    QString m_itemoid;
    QString m_originality;
    QString m_status;
  };

  enum Columns
    {
      AVAILABILITY = 2,
      BARCODE = 1,
      CONDITION = 4,
      COPY_NUMBER = 7,
      MYOID = 6,
      ORIGINALITY = 3,
      STATUS = 5,
      TITLE = 0
    };

  QList<copy_class *> m_copies;
  QPointer<QWidget> m_parent;
  QPointer<biblioteq> qmain;
  QSpinBox *m_spinbox;
  QString m_ioid;
  QString m_itemType;
  QString m_uniqueIdArg;
  QVector<QString> m_columnHeaderIndexes;
  Ui_bookcopybrowser m_cb;
  biblioteq_item *m_bitem;
  bool m_showForLending;
  int m_quantity;
  QString saveCopies(void);
  void changeEvent(QEvent *event);
  void clearCopiesList(void);
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);

 private slots:
  void slotDeleteCopy(void);
  void slotSaveCopies(void);
};

#endif
