#pragma once
#include <string>
using namespace std;

#define CONFIG_PATH "plugins/ScheduleCmd/schedule.json"

bool ConfAddSchedule(const string& cmd, const string& cronStr);
bool ConfRemoveSchedule(const string& cmd);
bool LoadConfigFile(const string& path);