#include "codegeex2clientinterface.h"

#include "codegeex2settings.h"
#include <QJsonDocument>
#include <QNetworkReply>

namespace CodeGeeX2 {
namespace Internal {

CodeGeeX2ClientInterface::CodeGeeX2ClientInterface()
    :LanguageClient::BaseClientInterface()
{
}

CodeGeeX2ClientInterface::~CodeGeeX2ClientInterface()
{
}

Utils::FilePath CodeGeeX2ClientInterface::serverDeviceTemplate() const
{
    return "";
}

void CodeGeeX2ClientInterface::replyFinished()
{
    if(m_reply==nullptr){
        return;
    }
    QByteArray qba=m_reply->readAll();
    m_reply->disconnect();
    m_reply=nullptr;
    if(qba.isEmpty()){
        QJsonObject errorObj;
        errorObj.insert("code",-32603);
        errorObj.insert("message","Request failed!");
        QJsonObject obj;
        obj.insert("id",m_id);
        obj.insert("error",errorObj);
        LanguageServerProtocol::JsonRpcMessage errMsg(obj);
        emit messageReceived(errMsg);
        return;
    }
    QJsonParseError err;
    QJsonDocument doc=QJsonDocument::fromJson(qba,&err);
    if(err.error!=QJsonParseError::NoError){
        QJsonObject errorObj;
        errorObj.insert("code",-32603);
        errorObj.insert("message","Request failed!");
        QJsonObject obj;
        obj.insert("id",m_id);
        obj.insert("error",errorObj);
        LanguageServerProtocol::JsonRpcMessage errMsg(obj);
        emit messageReceived(errMsg);
    }else{
        QJsonObject obj=doc.object();
        QJsonArray ary=obj.value("data").toArray();
        QString str=ary.first().toString();
        int firstLnPos=str.indexOf('\n');
        if(firstLnPos>0&&str.left(firstLnPos).contains("language")){
            str=str.mid(firstLnPos+1);
        }
        int headI=0;
        if(m_row>0){
            int r=0;
            for(int i=0;i<qMin(m_pos,str.length());i++){
                if(str.at(i).unicode()=='\n'){
                    r++;
                    if(r==m_row){
                        headI=i+1;
                        break;
                    }
                }
            }
        }
        if(m_row>0&&headI==0){
            str=str.mid(m_pos);
        }else{
            str=str.mid(headI+m_col);
        }
        QJsonObject responseRangeObj;
        responseRangeObj.insert("start",m_position);
        responseRangeObj.insert("end",m_position);
        QJsonObject responseSubObj;
        responseSubObj.insert("position",m_position);
        responseSubObj.insert("range",responseRangeObj);
        responseSubObj.insert("text",str);
        responseSubObj.insert("displayText",str);
        responseSubObj.insert("uuid",QUuid::createUuid().toString());
        QJsonArray responseAry;
        responseAry.push_back(responseSubObj);
        QJsonObject objResult;
        objResult.insert("completions",responseAry);
        QJsonObject responseObj;
        responseObj.insert("id",m_id);
        responseObj.insert("result",objResult);
        LanguageServerProtocol::JsonRpcMessage responseMsg(responseObj);
        emit messageReceived(responseMsg);
    }
}

void CodeGeeX2ClientInterface::sendData(const QByteArray &data)
{
    m_writeBuffer.open(QBuffer::Append);
    m_writeBuffer.write(data);
    m_writeBuffer.close();
    LanguageServerProtocol::BaseMessage baseMsg;
    QString parseError;
    m_writeBuffer.open(QBuffer::ReadWrite);
    LanguageServerProtocol::BaseMessage::parse(&m_writeBuffer,parseError,baseMsg);
    m_writeBuffer.close();
    if(baseMsg.isValid()){
        if(baseMsg.isComplete()){
            LanguageServerProtocol::JsonRpcMessage msg(baseMsg);
            QJsonObject objSend=msg.toJsonObject();
            if(objSend.value("method")=="initialize"){
                QJsonObject InfoObj;
                InfoObj.insert("name","Fake server for CodeGeeX2");
                InfoObj.insert("version","0.1");
                QJsonObject capObj;
                capObj.insert("completionProvider",QJsonObject());
                capObj.insert("textDocumentSync",0);
                QJsonObject objResult;
                objResult.insert("capabilities",capObj);
                objResult.insert("serverInfo",InfoObj);
                QJsonObject obj;
                obj.insert("id",objSend.value("id"));
                obj.insert("result",objResult);
                LanguageServerProtocol::JsonRpcMessage responseMsg(obj);
                emit messageReceived(responseMsg);
            }else if(objSend.value("method")=="shutdown"){
                clearReply();
                QJsonObject obj;
                obj.insert("id",objSend.value("id"));
                obj.insert("result",QJsonValue());
                LanguageServerProtocol::JsonRpcMessage responseMsg(obj);
                emit messageReceived(responseMsg);
            }else if(objSend.value("method")=="getCompletionsCycling"){
                clearReply();
                //qInfo("%s",msg.toRawData().data());
                QJsonObject objParams=objSend.value("params").toObject();
                QUrl url(CodeGeeX2Settings::instance().url.value());
                QNetworkRequest req(url);
                req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
                QJsonArray ary;
                m_pos=objParams.value("pos").toInt();
                m_position=objParams.value("doc").toObject().value("position");
                int maxLen=CodeGeeX2Settings::instance().contextLimit.value();
                QString context=objParams.value("txt").toString().left(m_pos);
                bool trimmed=false;
                if(context.length()>maxLen){
                    trimmed=true;
                    int offset=context.length()-maxLen;
                    int indN=context.indexOf('\n',offset);
                    if(indN>=0){
                        offset=indN+1;
                    }else{
                        int indSpace=context.indexOf(' ',offset);
                        if(indSpace>=0){
                            offset=indSpace+1;
                        }
                    }
                    m_pos-=offset;
                    context=context.mid(offset);
                }
                if(!trimmed&&CodeGeeX2Settings::instance().expandHeaders.value()){
                    QUrl fileURI(objParams.value("doc").toObject().value("uri").toString());
                    if(fileURI.isLocalFile()){
                        static const QRegularExpression reHeaderQ("#include\\s+\"([^\"]+)\"");
                        static const QRegularExpression reHeaderA("#include\\s+<([^>]+)>");
                        QFileInfo qfi(fileURI.toLocalFile());
                        QStringList searchPaths;
                        QDir dir=qfi.dir();
                        searchPaths.push_back(dir.absolutePath());
                        while(dir.cdUp()){
                            searchPaths.push_back(dir.absolutePath());
                        }
                        QStringList headerNames;
                        QStringList headerStrings;
                        {
                            QRegularExpressionMatchIterator it=reHeaderQ.globalMatch(context);
                            while(it.hasNext()){
                                QRegularExpressionMatch match=it.next();
                                QString header=match.captured(1);
                                if(header.isEmpty()){
                                    continue;
                                }
                                headerNames.push_back(header);
                                headerStrings.push_back(match.captured(0));
                            }
                        }
                        {
                            QRegularExpressionMatchIterator it=reHeaderA.globalMatch(context);
                            while(it.hasNext()){
                                QRegularExpressionMatch match=it.next();
                                QString header=match.captured(1);
                                if(header.isEmpty()){
                                    continue;
                                }
                                headerNames.push_back(header);
                                headerStrings.push_back(match.captured(0));
                            }
                        }
                        int space=maxLen-context.length();
                        bool full=false;
                        for(int i=0;i<headerNames.size();i++){
                            const QString &nm=headerNames.at(i);
                            QFileInfo tmpFI(nm);
                            if(tmpFI.baseName()==qfi.baseName()){
                                for(const QString &dirPath:searchPaths){
                                    QFileInfo qfiHeader(dirPath+"/"+nm);
                                    if(qfiHeader.exists()){
                                        if(!expandHeader(context,headerStrings.at(i),dirPath+"/"+nm,space,m_pos)){
                                            i=headerNames.size();
                                            full=true;
                                            break;
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                        if(!full){
                            for(int i=0;i<headerNames.size();i++){
                                const QString &nm=headerNames.at(i);
                                QFileInfo tmpFI(nm);
                                if(tmpFI.baseName()==qfi.baseName()){
                                    continue;
                                }
                                for(const QString &dirPath:searchPaths){
                                    QFileInfo qfiHeader(dirPath+"/"+nm);
                                    if(qfiHeader.exists()){
                                        if(!expandHeader(context,headerStrings.at(i),dirPath+"/"+nm,space,m_pos)){
                                            i=headerNames.size();
                                            break;
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                m_row=0;
                m_col=0;
                for(int i=0;i<m_pos;i++){
                    if(context.at(i).unicode()=='\n'){
                        m_row++;
                        m_col=0;
                    }else{
                        m_col++;
                    }
                }
                ary.push_back(context);
                ary.push_back("C++");
                ary.push_back(CodeGeeX2Settings::instance().seed.value());
                ary.push_back(CodeGeeX2Settings::instance().length.value());
                ary.push_back(CodeGeeX2Settings::instance().temperarure.value());
                ary.push_back(CodeGeeX2Settings::instance().topK.value());
                ary.push_back(CodeGeeX2Settings::instance().topP.value());
                QJsonObject obj;
                obj.insert("data",ary);
                obj.insert("event_data",QJsonValue());
                obj.insert("fn_index",0);
                m_id=objSend.value("id");
                if(m_manager==nullptr){
                    m_manager=QSharedPointer<QNetworkAccessManager>(new QNetworkAccessManager());
                }
                m_reply=QSharedPointer<QNetworkReply>(m_manager->post(req,QJsonDocument(obj).toJson()));
                connect(m_reply.get(),&QNetworkReply::finished,this,&CodeGeeX2ClientInterface::replyFinished);
            }
            QByteArray &bufferRaw=m_writeBuffer.buffer();
            bufferRaw.remove(0,bufferRaw.indexOf(baseMsg.header())+baseMsg.header().size()+baseMsg.contentLength);
        }
    }else{
        QJsonObject errorObj;
        errorObj.insert("code",-32700);
        errorObj.insert("message",parseError);
        QJsonObject obj;
        obj.insert("id",QJsonValue());
        obj.insert("error",errorObj);
        LanguageServerProtocol::JsonRpcMessage errMsg(obj);
        emit messageReceived(errMsg);
    }
}

void CodeGeeX2ClientInterface::clearReply()
{
    if(m_reply!=nullptr){
        m_reply->disconnect();
        m_reply=nullptr;
        QJsonObject errorObj;
        errorObj.insert("code",-32603);
        errorObj.insert("message","Request canceled.");
        QJsonObject obj;
        obj.insert("id",m_id);
        obj.insert("error",errorObj);
        LanguageServerProtocol::JsonRpcMessage errMsg(obj);
        emit messageReceived(errMsg);
    }
}

bool CodeGeeX2ClientInterface::expandHeader(QString &txt, const QString &includeStr, const QString &path, int &space, int &pos)
{
    QFileInfo qfiHeader(path);
    if(qfiHeader.size()-includeStr.length()>space){
        return false;
    }
    int ind=txt.indexOf(includeStr);
    if(ind<0){
        return false;
    }
    QFile qf(path);
    QTextStream qts(&qf);
    qf.open(QIODevice::ReadOnly|QIODevice::Text);
    QString content=qts.readAll();
    qf.close();
    txt.replace(ind,includeStr.length(),content);
    space-=(content.length()-includeStr.length());
    pos+=(content.length()-includeStr.length());
    return true;
}

} // namespace Internal
} // namespace CodeGeeX2
