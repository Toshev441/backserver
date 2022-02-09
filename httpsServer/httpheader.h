#ifndef HTTPHEADER_H
#define HTTPHEADER_H

#include <QObject>
#include "http_parser.h"

class HttpHeader : public QObject
{
    Q_OBJECT

public:
    enum headerType {REQUEST, RESPONSE};
    explicit HttpHeader(http_parser_type headerType, QObject *parent = nullptr);
    explicit HttpHeader(http_parser_type headerType, QByteArray raw, QObject *parent = nullptr);

    void setData(QByteArray &raw);
    enum class Error {
        ok = 0,
        errParseHeader,
        errParseUrl,
    };

    enum class State {
        NotStarted,
        OnMessageBegin,
        OnUrl,
        OnStatus,
        OnHeaders,
        OnHeadersComplete,
        OnBody,
        OnMessageComplete,
        OnChunkHeader,
        OnChunkComplete
    };
    struct headerItem{
        const char* field;
        int fieldSize;
        const char *value;
        int valueSize;
    };

    const QString &getHost() const;
    const QByteArray &getBody() const;
    QList<headerItem> *getHeaders();
    const QString &getStatus() const;
    http_method getMethod() const;
    const QString &getLastError() const;
    Error getError() const;
    const QString &getUrlPath() const;
    QJsonObject getUrlQuery();
    const QString getHeaderValue(QString field) const;

private:
    struct Url{
        const char* data;
        int size;
    };

    Error error = Error::ok;
    QString lastError;
    State state = State::NotStarted;
    QString status;
    QString lastField;
    Url url;
    QString host;
    QByteArray body;
    QList<headerItem> headers;
    QString urlPath;
    QString urlQuery;

    static int onMessageBegin(http_parser* p);
    static int onUrl(http_parser* p, const char *at, size_t length);
    static int onStatus(http_parser* p, const char *at, size_t length);
    static int onHeaderField(http_parser* p, const char *at, size_t length);
    static int onHeaderValue(http_parser* p, const char *at, size_t length);
    static int onHeadersComplete(http_parser* p);
    static int onBody(http_parser* p, const char *at, size_t length);
    static int onMessageComplete(http_parser* p);
    static int onChunkHeader(http_parser* p);
    static int onChunkComplete(http_parser* p);

    void parseUrl();

    QByteArray data;
    http_parser_type hType;
    http_parser parser;
    http_parser_settings  parserSettings {
        &HttpHeader::onMessageBegin,
                &HttpHeader::onUrl,
                &HttpHeader::onStatus,
                &HttpHeader::onHeaderField,
                &HttpHeader::onHeaderValue,
                &HttpHeader::onHeadersComplete,
                &HttpHeader::onBody,
                &HttpHeader::onMessageComplete,
                &HttpHeader::onChunkHeader,
                &HttpHeader::onChunkComplete
    };

signals:

};

#endif // HTTPHEADER_H
