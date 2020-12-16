#include "configmanager.h"
#include "json/json.h"
#include <iostream>
#include <fstream>
using namespace std;

ConfigManager::ConfigManager()
{

}

QString ConfigManager::getValueConfig(QString key)
{
    Json::Reader reader;
    Json::Value root;
    QString volue;

    //从文件中读取，保证当前文件有demo.json文件
    ifstream in(URLCONFIGPATH, ios::binary);

    if (!in.is_open())
    {
        return "";
    }

    if (reader.parse(in, root))
    {
        volue = root[key.toStdString().c_str()].asString().c_str();
    }
    else
    {
        in.close();
        return "";
    }

    in.close();
    return volue;
}

bool ConfigManager::setValueConfig(QString volue)
{
    Json::Reader reader;
    Json::Value root;
    Json::StyledWriter  writer;

    //从文件中读取，保证当前文件有demo.json文件
    ifstream in(URLCONFIGPATH, ios::binary);

    if (!in.is_open())
    {
        return false;
    }

    if (reader.parse(in, root))
    {
        //root[SECRETKEY] = volue.toStdString();
    }
    else
    {
        in.close();
        return false;
    }

    string jsonFile = writer.write(root);
//    printf("jsonFile:%s\n", jsonFile.c_str());

    ofstream ofs(URLCONFIGPATH, ios::binary);
    if (!ofs.is_open())
    {
        return false;
    }
    ofs << jsonFile;

    ofs.close();
    in.close();
    return true;
}
