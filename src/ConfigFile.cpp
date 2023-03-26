#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include "Scheduler.h"
#include <nlohmann/json.hpp>
#include <llapi/LoggerAPI.h>
using namespace nlohmann;
using namespace std;

extern Logger logger;

json conf;
string confPath;

//Helper
bool FileReadFrom(const string& path, string& result)
{
    ifstream fRead(path);
    if (!fRead)
        return false;
    string data((std::istreambuf_iterator<char>(fRead)),
        std::istreambuf_iterator<char>());
    fRead.close();
    result = data;
    return true;
}
//Helper

bool FlushConfigFile()
{
    ofstream jsonFile(confPath);
    if (jsonFile.is_open())
    {
        jsonFile << conf.dump(4);
        jsonFile.close();
        return true;
    }
    else
    {
        logger.error("配置文件写入失败！");
        return false;
    }
}


bool ConfAddSchedule(const string& cmd, const string& cronStr)
{
    conf.push_back({ cmd,cronStr });
    FlushConfigFile();
    return true;
}

bool ConfRemoveSchedule(const string& cmd)
{
    conf.erase(cmd);
    FlushConfigFile();
    return true;
}

bool LoadConfigFile(const string& path)
{
    confPath = path;

	if (!filesystem::exists(path))
	{
        conf = json::object();

        filesystem::create_directories(filesystem::path(path).remove_filename());
        ofstream fout(path);
        fout << "{}" << flush;
        fout.close();

        logger.info("配置文件不存在！已自动创建。");
		return false;
	}

    string confData;
    if (!FileReadFrom(path, confData))
    {
        logger.error("配置文件读取失败！");
        conf = json::object();
        return false;
    }

    try
    {
        conf = json::parse(confData);
        if (!conf.is_object())
            throw exception("Wrong type of root element");
    }
    catch (exception& e)
    {
        logger.warn("配置文件解析失败！{}", e.what());
        conf = json::object();
        return false;
    }

    for (auto& [key, value] : conf.items())
    {
        if (!AddSchedule(key, value))
        {
            logger.error("计划命令项 {} 解析失败！", key);
        }
    }
    return true;
}