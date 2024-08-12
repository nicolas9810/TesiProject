#include "tempo.h"

// Costruttore di default
Tempo::Tempo() : sec(0), min(0), hour(0), day(0) {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time);
    hour = now_tm->tm_hour;
    day = now_tm->tm_mday;
}


// Costruttore con parametri
Tempo::Tempo(int d, int h, int m, int s) : day(d), hour(h), min(m), sec(s) {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time);
    hour = now_tm->tm_hour;
    day = now_tm->tm_mday;
}

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
     auto now = std::chrono::system_clock::now();

    // Converti il time_point in un time_t che rappresenta il tempo in secondi dal 1970-01-01 00:00:00 UTC
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    // Converte time_t in una struttura tm leggibile
    std::tm* now_tm = std::localtime(&now_time);

    // Estrai l'ora e il minuto corrente come interi
    int current_sec = now_tm->tm_sec;

    return current_sec;
}

int Tempo::getMinutes() {
    


    auto now = std::chrono::system_clock::now();

    // Converti il time_point in un time_t che rappresenta il tempo in secondi dal 1970-01-01 00:00:00 UTC
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    // Converte time_t in una struttura tm leggibile
    std::tm* now_tm = std::localtime(&now_time);

    // Estrai l'ora e il minuto corrente come interi
    int current_minute = now_tm->tm_min;

    return current_minute;
}

int Tempo::getHours() {
    auto now = std::chrono::system_clock::now();

    // Converti il time_point in un time_t che rappresenta il tempo in secondi dal 1970-01-01 00:00:00 UTC
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    // Converte time_t in una struttura tm leggibile
    std::tm* now_tm = std::localtime(&now_time);

    // Estrai l'ora e il minuto corrente come interi
    int current_hour = now_tm->tm_hour;

    current_hour=current_hour-hour;
    if(current_hour<0){
        current_hour=24+current_hour;
    }
    return current_hour;
}
int Tempo::getDay(){
    auto now = std::chrono::system_clock::now();

    // Converti il time_point in un time_t che rappresenta il tempo in secondi dal 1970-01-01 00:00:00 UTC
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    // Converte time_t in una struttura tm leggibile
    std::tm* now_tm = std::localtime(&now_time);

    // Estrai l'ora e il minuto corrente come interi
    int current_mday = now_tm->tm_mday;

    return current_mday;
}
int Tempo::getTimeInMin(){
    // #ifdef VIRTUALE
    // //lock_read();
    // m=getMinutes()+getHours()*60+getDay()*24*60;
    // //unck_read();
    // #endif
    // #ifndef VIRTUALE
    // #endif
    int h=getHours();
    int m=getMinutes();
    m=m+h*60;
    
    return m;

}
int Tempo::getTimeInSec(){
    // #ifdef VIRTUALE
    // //lock_read();
    // m=getMinutes()+getHours()*60+getDay()*24*60;
    // //unck_read();
    // #endif
    // #ifndef VIRTUALE
    // #endif
    int s=getSeconds();
    int h=getHours();
    int m=getMinutes();
    s+=m*60+h*60*60;
    
    return s;
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

