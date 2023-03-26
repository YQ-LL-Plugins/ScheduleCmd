#pragma once
#include <string>
using namespace std;

void CronTick();
bool AddSchedule(string cmd, string cronStr);
bool RemoveSchedule(string cmd);
bool RemoveAllSchedule();