
#ifndef _BIBLIOTEQ_COVERS_H_
#define _BIBLIOTEQ_COVERS_H_

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QString>

class biblioteq_covers
{
public:
  static QString directory(const QString &databasePath)
  {
    const QFileInfo info(databasePath);

    return QDir(info.absolutePath()).filePath("Covers");
  }

  static QString coverPath(const QString &databasePath,
                           const QString &isbn13)
  {
    QString isbn(isbn13);

    isbn.remove('-');
    isbn = isbn.trimmed();

    // ISBN-13 must contain exactly 13 digits.
    if(isbn.length() != 13)
      return QString();

    for(const auto c : isbn)
      if(c < '0' || c > '9')
        return QString();

    return QDir(directory(databasePath)).filePath(isbn + ".jpg");
  }

  static QByteArray load(const QString &databasePath,
                         const QString &isbn13)
  {
    const QString path(coverPath(databasePath, isbn13));

    if(path.isEmpty())
      return QByteArray();

    QFile file(path);

    if(!file.exists() || !file.open(QIODevice::ReadOnly))
      return QByteArray();

    const QByteArray bytes(file.readAll());

    // Reject invalid image files.
    if(QImage::fromData(bytes).isNull())
      return QByteArray();

    return bytes;
  }
};

#endif

