#include <QString>

class marc
{
 public:
  marc(const QString &protocol, const QString &recordSyntax);
  ~marc();

 private:
  QString m_protocol;
  QString m_recordSyntax;
};
