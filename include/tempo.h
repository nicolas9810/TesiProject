#ifndef VIRTUALTIME_H
#define VIRTUALTIME_H
#include "defines.h"
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <iostream>



class Tempo {
private:

    std::mutex mutex_;
    std::condition_variable read_cv_;
    std::condition_variable write_cv_;
    int reader_count_ = 0;
    int writer_count_ = 0;
    bool writing_ = false;



public:
    int sec;
    int min;
    int hour;
    int day;
    
    

    // Costruttori
    Tempo();
    Tempo(int d, int h, int m, int s);

    Tempo(Tempo&& other) noexcept;
    Tempo& operator=(Tempo&& other) noexcept;
    
    std::mutex mtx;
    // Getter
    int getSeconds();
    int getMinutes();
    int getHours();
    int getDay();
    int getTimeInMin();

    // Incrementatori
    void incrementSeconds(int s);
    void incrementsMinutes(int m);
    void incrementsHours(int h);
    void incrementsDay(int d);

    // Setter
    void settime(int d, int h, int m, int s);

    void lock_read();
    void unlock_read();
    void lock_write();
    void unlock_write();
};

#endif // VIRTUALTIME_H
