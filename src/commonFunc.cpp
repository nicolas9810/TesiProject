#include "commonFunc.h"
int getTimeInMin(){
    time_t now = time(0); // get current date and time  
    tm* ltm = localtime(&now);  
    int hour= ltm->tm_hour;
    int min= ltm->tm_min;
    min=min+hour*60;
    return min;
}