#ifndef CODEGEEX2_INTERNAL_CODEGEEX2CLIENTINTERFACE_H
#define CODEGEEX2_INTERNAL_CODEGEEX2CLIENTINTERFACE_H

#include <languageclient/languageclientinterface.h>

#include <QNetworkAccessManager>

class QNetworkReply;

namespace CodeGeeX2 {
namespace Internal {

class CodeGeeX2ClientInterface : public LanguageClient::BaseClientInterface
{
public:
    CodeGeeX2ClientInterface();
    ~CodeGeeX2ClientInterface();

    Utils::FilePath serverDeviceTemplate() const override;

public slots:
    void replyFinished();

protected:
    void sendData(const QByteArray &data) override;
private:
    QBuffer m_writeBuffer;
    QSharedPointer<QNetworkReply> m_reply;
    QJsonValue m_id;
    int m_pos;
    QJsonValue m_position;
    int m_row;
    int m_col;
    int m_braceLevel;

    QSharedPointer<QNetworkAccessManager> m_manager;

    void clearReply();
    bool expandHeader(QString &txt,const QString &includeStr,const QString &path,int &space,int &pos);
};

} // namespace Internal
} // namespace CodeGeeX2

#endif // CODEGEEX2_INTERNAL_CODEGEEX2CLIENTINTERFACE_H
