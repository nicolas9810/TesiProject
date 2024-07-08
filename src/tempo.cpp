#include "tempo.h"

// Costruttore di default
Tempo::Tempo() : sec(0), min(0), hour(0), day(0) {}

// Costruttore con parametri
Tempo::Tempo(int d, int h, int m, int s) : day(d), hour(h), min(m), sec(s) {}

// Costruttore di move
Tempo::Tempo(Tempo&& other) noexcept : sec(other.sec), min(other.min), hour(other.hour), day(other.day) {
    // Nota: non è necessario spostare il mutex, poiché non è copiabile o assegnabile.
}

// Operatore di assegnazione di move
Tempo& Tempo::operator=(Tempo&& other) noexcept {
    if (this != &other) {
        std::lock_guard<std::mutex> lock(mtx);  // Lock del mutex
        sec = other.sec;
        min = other.min;
        hour = other.hour;
        day = other.day;
    }
    return *this;
}

// Getter
int Tempo::getSeconds() {
    return sec;
}

int Tempo::getMinutes() {
    return min;
}

int Tempo::getHours() {
    return hour;
}
int Tempo::getDay(){
    return day;
}
int Tempo::getTimeInMin(){
    int m=0;
    #ifdef VIRTUALE
    m=getMinutes()+getHours()*60+getDay()*24*60;
    #endif
    #ifndef VIRTUALE
    time_t now = time(0); // get current date and time  
    tm* ltm = localtime(&now);  
    int h= ltm->tm_hour;
    m= ltm->tm_min;
    m=m+h*60;
    #endif
    return m;

}

// Incrementatori
void Tempo::incrementSeconds(int s) {
    int somma = sec + s;
    int tmp = somma / 60;
    sec = somma % 60;
    incrementsMinutes(tmp);
}

void Tempo::incrementsMinutes(int m) {
    if (m == 0) return;
    int somma = min + m;
    int tmp = somma / 60;
    min = somma % 60;
    incrementsHours(tmp);
}

void Tempo::incrementsHours(int h) {
    if (h == 0) return;
    int somma = hour + h;
    int tmp = somma / 24;
    this->hour = somma % 24;
    incrementsDay(tmp);
}

void Tempo::incrementsDay(int d) {
    if (d == 0) return;
    this->day += d;
}

// Setter
void Tempo::settime(int d, int h, int m, int s) {
    if (s >= 0 && s < 60) sec = s;
    if (m >= 0 && m < 60) min = m;
    if (h >= 0 && h < 24) hour = h;
    if (d >= 0) day = d;
}

