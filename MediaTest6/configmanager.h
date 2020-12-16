#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QString>

#define URLCONFIGPATH "lc_url.json"

#define LOCALSERVERIP "LocalserverIP"
#define WEBIP "WebIP"
//#define SECRETKEY "SecretKey"
#define MEDIAADDR "MediaAddr"

#define SUBSCRIBER_PORT "5556"
#define REQUEST_PORT_STR "5581"
#define REQUEST_PORT_INT 5581



class ConfigManager
{
public:
    ConfigManager();

    static QString getValueConfig(QString key);
    static bool setValueConfig(QString key);
};

#endif // CONFIGMANAGER_H
