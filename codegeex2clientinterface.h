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
//    QByteArray m_cache;
    QBuffer m_writeBuffer;
    QSharedPointer<QNetworkReply> m_reply;
    QJsonValue m_id;
    int m_pos;
    QJsonValue m_position;

    QSharedPointer<QNetworkAccessManager> m_manager;

    //QVector<std::pair<QPointer<QNetworkReply>,QString> > m_queue;
    void clearReply();
};

} // namespace Internal
} // namespace CodeGeeX2

#endif // CODEGEEX2_INTERNAL_CODEGEEX2CLIENTINTERFACE_H