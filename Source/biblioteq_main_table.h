#ifndef _BIBLIOTEQ_MAIN_TABLE_H_
#define _BIBLIOTEQ_MAIN_TABLE_H_

#include <QPointer>
#include <QTableWidget>

class biblioteq;

class biblioteq_main_table: public QTableWidget
{
  Q_OBJECT

 public:
  biblioteq_main_table(QWidget *parent);
  QHash<QString, QString> friendlyStates(void) const;

#if (QT_VERSION < QT_VERSION_CHECK(5, 10, 0))
  QModelIndex indexFromItem(QTableWidgetItem *item) const
#else
  QModelIndex indexFromItem(const QTableWidgetItem *item) const
#endif
  {
    return QTableWidget::indexFromItem(item);
  }

  QStringList columnNames(void) const;
  bool isColumnHidden(const int index,
		      const QString &type,
		      const QString &username) const;
  bool isColumnHidden(int index) const;
  int columnNumber(const QString &name) const;
  void parseStates(const QHash<QString, QString> &states);
  void recordColumnHidden(const QString &username,
			  const QString &type,
			  const int index,
			  const bool hidden);
  void resetTable(const QString &username,
		  const QString &t,
		  const QString &roles);
  void setColumnNames(const QStringList &list);
  void setQMain(biblioteq *biblioteq);
  void updateToolTips(const int row);

 private:
  QHash<QString, QList<int> > m_hiddenColumns;
  QPointer<biblioteq> m_qmain;
  QVector<QString> m_columnHeaderIndexes;
  void keyPressEvent(QKeyEvent *event);
  void setColumns(const QString &username,
		  const QString &type,
		  const QString &roles);

 signals:
  void enterKeyPressed(void);
};

#endif
