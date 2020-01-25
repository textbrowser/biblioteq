#ifndef _BIBLIOTEQ_PHOTOGRAPH_VIEW_H_
#define _BIBLIOTEQ_PHOTOGRAPH_VIEW_H_

#include <QGraphicsView>

class biblioteq_photograph_view: public QGraphicsView
{
  Q_OBJECT

 public:
  biblioteq_photograph_view(QWidget *parent);
  void setBestFit(const bool bestFit);
  void setImage(const QImage &image, const qint64 oid);

 private:
  QImage m_image;
  bool m_bestFit;
  qint64 m_oid;
  qreal m_degrees;
  void resizeEvent(QResizeEvent *event);
  void rotateImage(const qreal degrees);

 private slots:
  void slotRotateLeft(void);
  void slotRotateRight(void);
  void slotSave(void);

 signals:
  void save(const QImage &image, const qint64 oid);
};

#endif
