#include "Scheduler.h"
#include "ConfigFile.h"
#include <llapi/Global.h>
#include <llapi/EventAPI.h>
#include <llapi/LoggerAPI.h>
using namespace std;

extern Logger logger;

// Helper
vector<string> SplitCmdParas(const string& paras)
{
    if (paras.empty())
        return vector<string>();

    vector<string> res;
    string now, strInQuote = "";
    istringstream strIn(paras);
    while (strIn >> now)
    {
        if (!strInQuote.empty())
        {
            strInQuote = strInQuote + " " + now;
            if (now.back() == '\"')
            {
                strInQuote.pop_back();
                res.push_back(strInQuote.erase(0, 1));
                strInQuote = "";
            }
        }
        else
        {
            if (now.front() == '\"')
                strInQuote = now;
            else
                res.push_back(now);
        }
    }
    if (!strInQuote.empty())
    {
        istringstream leftIn(strInQuote);
        while (leftIn >> now)
            res.push_back(now);
    }
    return res;
}

vector<string> SplitWithSymbol(const string &str, const string &part)
{
    vector<string> resVec;

    if ("" == str)
        return resVec;

    string strs = str + part;
    size_t pos = strs.find(part);
    size_t size = strs.size();

    while (pos != string::npos)
    {
        string x = strs.substr(0, pos);
        resVec.push_back(x);
        strs = strs.substr(pos + 1, size);
        pos = strs.find(part);
    }
    return resVec;
}
// Helper

bool CmdAddSchedule(const string& cmd, const string& cronStr)
{
    if (AddSchedule(cmd, cronStr) && ConfAddSchedule(cmd,cronStr))
    {
        logger.info("计划命令已成功添加。");
        return true;
    }
    else
    {
        logger.info("计划命令添加失败！");
        return false;
    }
}

bool CmdRemoveSchedule(const string& cmd)
{
    if(RemoveSchedule(cmd) && ConfRemoveSchedule(cmd))
    {
        logger.info("计划命令已删除。");
        return true;
    }
    else
    {
        logger.info("计划命令删除失败！此命令无计划进行中");
        return false;
    }
}


#define FAIL_END(e) { logger.error(e); return false; }


void RegisterCmdProcess()
{
    Event::ConsoleCmdEvent::subscribe([](const Event::ConsoleCmdEvent& ev)
    {
        if(ev.mCommand.substr(0, 11) == "schedulecmd")
        {
            auto paras = SplitCmdParas(ev.mCommand.substr(12));
            if (!paras.empty())
            {
                if (paras[0] == "add")
                {
                    if (paras.size() == 8)
                    {
                        //add backup * * * * * *
                        CmdAddSchedule(paras[1], paras[2] + " " + paras[3] + " " + paras[4] + " " + paras[5] + " " + paras[6] + " " + paras[7]);
                    }
                    else if (paras.size() == 3)
                    {
                        if (paras[2].find(":") != string::npos)
                        {
                            auto times = SplitWithSymbol(paras[2], ":");
                            char str[20] = { 0 };
                            if (times.size() == 3)
                            {
                                //add backup 12:34:00
                                sprintf_s(str, "%s %s %s * * ?", times[2].c_str(), times[1].c_str(), times[0].c_str());
                                CmdAddSchedule(paras[1], string(str));
                            }
                            else if (times.size() == 2)
                            {
                                //add backup 12:34
                                sprintf_s(str, "0 %s %s * * ?", times[1].c_str(), times[0].c_str());
                                CmdAddSchedule(paras[1], string(str));
                            }
                            else
                                FAIL_END("命令执行失败！参数格式错误");
                        }
                        else
                            FAIL_END("命令执行失败！参数格式错误");
                    }
                    else
                        FAIL_END("命令执行失败！参数数量错误");
                }
                else if (paras[0] == "remove")
                {
                    if (paras.size() == 2)
                    {
                        //remove backup
                        CmdRemoveSchedule(paras[1]);
                    }
                    else
                        FAIL_END("命令执行失败！参数数量错误");
                }
                else if (paras[0] == "reload")
                {
                    RemoveAllSchedule();
                    LoadConfigFile(CONFIG_PATH);
                    logger.info("配置文件已重新加载");
                }
                else
                    FAIL_END("命令执行失败！不存在此操作");
            }
            return false;
        }
        return true;
    });
}