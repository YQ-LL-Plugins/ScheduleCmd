#include <libcron/Cron.h>
#include <llapi/mc/Level.hpp>
#include <llapi/ScheduleAPI.h>
#include <queue>
#include <string>
#include <windows.h> 
#include <iostream>
#include <vector>
using namespace libcron;
using namespace std;

static int taskid = 0;
libcron::Cron cron;

// 植入tick
void CronTick()
{
    cron.tick();
}

bool AddSchedule(string cmd, string cronStr)
{
    cron.add_schedule(cmd + to_string(++taskid), cronStr, [cmd](auto&) {
        Schedule::nextTick([cmd]() {
            Level::runcmd(cmd);
        });
    });
    return true;
}

bool RemoveSchedule(string cmd)
{
    return true;
}

bool RemoveAllSchedule()
{
    cron.clear_schedules();
    return true;
}