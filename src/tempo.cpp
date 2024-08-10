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




// void Tempo::lock_read() {
//     std::unique_lock<std::mutex> lock(mutex_);
//     read_cv_.wait(lock, [this]() { return writer_count_ == 0; });
//     ++reader_count_;
// }

// void Tempo::unlock_read() {
//     std::lock_guard<std::mutex> lock(mutex_);
//     if (--reader_count_ == 0) {
//         write_cv_.notify_one();
//     }
// }

// void Tempo::lock_write() {
//     std::unique_lock<std::mutex> lock(mutex_);
//     ++writer_count_;
//     write_cv_.wait(lock, [this]() { return reader_count_ == 0 & !writing_; ; });
//     writing_ = true;
// }

// void Tempo::unlock_write() {
//     std::lock_guard<std::mutex> lock(mutex_);
//     writing_ = false;
//     --writer_count_;
//     if (writer_count_ == 0) {
//         read_cv_.notify_all();
//     } else {
//         write_cv_.notify_one();
//     }
// }

// Getter
int Tempo::getSeconds() {
    //lock_read();
    int s= sec;
    //unck_read();
    return s;
}

int Tempo::getMinutes() {
    //lock_read();
    int m=min;
    //unck_read();
    return m;
}

int Tempo::getHours() {
    //lock_read();
    int h= hour;
    //unck_read();
    return h;
}
int Tempo::getDay(){
    //lock_read();
    int d= day;
    //unck_read();
    return d;
}
int Tempo::getTimeInMin(){
    int m=0;
    #ifdef VIRTUALE
    //lock_read();
    m=getMinutes()+getHours()*60+getDay()*24*60;
    //unck_read();
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
    //lock_write();
    int somma = sec + s;
    //unck_write();
    int tmp = somma / 60;
    sec = somma % 60;
    incrementsMinutes(tmp);
}

void Tempo::incrementsMinutes(int m) {
    if (m == 0) return;
    //lock_write();
    int somma = min + m;
    //unck_write();
    int tmp = somma / 60;
    min = somma % 60;
    incrementsHours(tmp);
}

void Tempo::incrementsHours(int h) {
    if (h == 0) return;
    //lock_write();
    int somma = hour + h;
    //unck_write();
    int tmp = somma / 24;
    this->hour = somma % 24;
    incrementsDay(tmp);
}

void Tempo::incrementsDay(int d) {
    if (d == 0) return;
    //lock_write();
    this->day += d;
    //unck_write();

}

// Setter
void Tempo::settime(int d, int h, int m, int s) {
    //lock_write();
    if (s >= 0 && s < 60) sec = s;
    if (m >= 0 && m < 60) min = m;
    if (h >= 0 && h < 24) hour = h;
    if (d >= 0) day = d;
    //unlock_write();

}

