#include <QString>

class marc
{
 public:
  enum PROTOCOL
  {
    SRU = 0,
    Z3950
  };

  enum RECORD_SYNTAX
  {
    MARC21 = 0,
    UNIMARC
  };

  marc(const PROTOCOL protocol, const RECORD_SYNTAX recordSyntax);
  ~marc();
  void setData(const QString &data);

 private:
  PROTOCOL m_protocol;
  QString m_data;
  RECORD_SYNTAX m_recordSyntax;
  void parseSRU(void);
  void parseZ3950(void);
};
