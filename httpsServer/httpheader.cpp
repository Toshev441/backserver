#include "httpheader.h"
#include <QDebug>
#include <QJsonObject>


HttpHeader::HttpHeader(http_parser_type headerType, QObject *parent) : QObject(parent)
{
    hType = headerType;
    parser.data = this;
    http_parser_init(&parser, hType);
}

bool HttpHeader::parse(QByteArray &raw)
{
    int s = http_parser_execute(&parser, &parserSettings, raw.data(), raw.size());
    if(s < raw.size()){
        error = Error::errParseHeader;
        lastError = "Parse header error";
        return false;
    }
    return true;
}

const QString &HttpHeader::getHost() const
{
    return host;
}

const QByteArray &HttpHeader::getBody() const
{
    return body;
}

QList<HttpHeader::headerItem> *HttpHeader::getHeaders()
{
    return &headers;
}

const QString &HttpHeader::getStatus() const
{
    return status;
}

http_method HttpHeader::getMethod() const
{
    return static_cast<http_method>(parser.method);
}

void HttpHeader::parseUrl()
{
    http_parser_url parserUrl;
    if(http_parser_parse_url(url.data, url.size, 0, &parserUrl) != 0){
        error = Error::errParseUrl;
        lastError = "Parse url error";
    }
    else
    {
        urlPath = QByteArray(&url.data[parserUrl.field_data[UF_PATH].off], parserUrl.field_data[UF_PATH].len);
        urlQuery = QByteArray(&url.data[parserUrl.field_data[UF_QUERY].off], parserUrl.field_data[UF_QUERY].len);
    }

}

const QString &HttpHeader::getLastError() const
{
    return lastError;
}

HttpHeader::Error HttpHeader::getError() const
{
    return error;
}

const QString &HttpHeader::getUrlPath() const
{
    return urlPath;
}

QJsonObject HttpHeader::getUrlQuery()
{
    QJsonObject obj;
    QStringList parts = urlQuery.split("&");
    foreach(auto p , parts){
        QStringList param = p.split("=");
        obj.insert(param[0], param[1]);
    }
    return obj;
}

const QString HttpHeader::getHeaderValue(QString field) const
{
    for(auto f : headers){
        QString fName = QByteArray(f.field, f.fieldSize);
        if(fName.compare(field, Qt::CaseInsensitive) == 0){
            return QByteArray(f.value, f.valueSize);
        }
    }
    return "";
}

HttpHeader::State HttpHeader::getState() const
{
    return state;
}

int HttpHeader::onMessageBegin(http_parser *p)
{
    HttpHeader *res = static_cast<HttpHeader*>(p->data);
    res->state = State::OnMessageBegin;
 return 0;
}

int HttpHeader::onUrl(http_parser *p, const char *at, size_t length)
{
    HttpHeader *res = static_cast<HttpHeader*>(p->data);
    res->url.data = at;
    res->url.size = length;
    res->state = State::OnUrl;
    res->parseUrl();
    return 0;
}

int HttpHeader::onStatus(http_parser *p, const char *at, size_t length)
{
    HttpHeader *res = static_cast<HttpHeader*>(p->data);
    res->status = QString(QByteArray(at, length));
    res->state = State::OnStatus;
    return 0;
}

int HttpHeader::onHeaderField(http_parser *p, const char *at, size_t length)
{
    HttpHeader *res = static_cast<HttpHeader*>(p->data);
    res->lastField = QString(QByteArray(at, length));
    headerItem item;
    item.field = at;
    item.fieldSize = length;
    res->headers.append(item);
    res->state = State::OnHeaders;
    return 0;
}

int HttpHeader::onHeaderValue(http_parser *p, const char *at, size_t length)
{
    HttpHeader *res = static_cast<HttpHeader*>(p->data);
    if(res->lastField.compare(QByteArrayLiteral("host"), Qt::CaseInsensitive) == 0){
        res->host = QString(QByteArray(at, length));
    }
    else
    {
        res->headers.last().value = at;
        res->headers.last().valueSize = length;
    }
    res->lastField.clear();
    res->state = State::OnHeaders;
    return 0;
}

int HttpHeader::onHeadersComplete(http_parser *p)
{
    HttpHeader *res = static_cast<HttpHeader*>(p->data);
    res->state = State::OnHeadersComplete;
    return 0;
}

int HttpHeader::onBody(http_parser *p, const char *at, size_t length)
{
    HttpHeader *res = static_cast<HttpHeader*>(p->data);
    res->body.append(at, length);
    res->state = State::OnBody;
    return 0;
}

int HttpHeader::onMessageComplete(http_parser *p)
{
    HttpHeader *res = static_cast<HttpHeader*>(p->data);
    res->state = State::OnMessageComplete;
    return 0;
}

int HttpHeader::onChunkHeader(http_parser *p)
{
    HttpHeader *res = static_cast<HttpHeader*>(p->data);
    res->state = State::OnChunkHeader;
    return 0;
}

int HttpHeader::onChunkComplete(http_parser *p)
{
    HttpHeader *res = static_cast<HttpHeader*>(p->data);
    res->state = State::OnChunkComplete;
    return 0;
}
