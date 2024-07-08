
#include "defines.h"
#include <chrono>
#include <mutex>
class Tempo {
private:



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

};


