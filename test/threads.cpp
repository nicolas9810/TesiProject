#include <pthread.h>
#include <iostream>
#include <sched.h>
#include <unistd.h>

#include <boost/process.hpp>

namespace bp = boost::process;

using namespace std;
string cfgfilename = "/home/nicolas/Codice/TesiProject/cfg/fmxcku115r1_3.cfg";
string comandoUp ="profpga_run "+cfgfilename+" --up";



void* threadFunction(void* arg) {
    std::cout << "Thread con priorità elevata in esecuzione" << std::endl;
    // Esegui le operazioni necessarie...
    std::cout << "Inizio della parte critica" << std::endl;
     pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, nullptr);
    try {
        bp::system(comandoUp);  // Esegue il comando
    } catch (const std::exception &e) {
        std::cerr << "Errore durante l'esecuzione del comando: " << e.what() << "\n";
    }
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
    std::cout << "Fine della parte critica" << std::endl;
        sleep(1);

    return nullptr;
}

void* normalThreadFunction(void* arg) {
    std::cout << "Thread con priorità normale in esecuzione" << std::endl;
    // Esegui le operazioni necessarie...
    for(int i = 0; i < 5; i++) {
        std::cout << "Thread con priorità normale, iterazione: " << i << std::endl;
        sleep(1);
    }
    return nullptr;
}


int main() {
    pthread_t highPriorityThread1;
    pthread_t highPriorityThread2;

    pthread_t normalThread1;
    pthread_t normalThread2;

    pthread_attr_t attr;
    struct sched_param param;

    // Inizializza gli attributi del thread
    pthread_attr_init(&attr);

    // Imposta lo scheduling policy su SCHED_FIFO (First In First Out)
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

    // Imposta la priorità del thread
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    pthread_attr_setschedparam(&attr, &param);

    // Crea il thread con gli attributi specificati
    int result = pthread_create(&highPriorityThread1, &attr, threadFunction, nullptr);
    if (result != 0) {
        std::cerr << "Errore nella creazione del thread ad alta priorità" << std::endl;
        return 1;
    }
    // result = pthread_create(&highPriorityThread2, &attr, threadFunction, nullptr);
    // if (result != 0) {
    //     std::cerr << "Errore nella creazione del thread ad alta priorità" << std::endl;
    //     return 1;
    // }

    // Crea un thread con priorità normale
    result = pthread_create(&normalThread1, nullptr, normalThreadFunction, nullptr);
    if (result != 0) {
        std::cerr << "Errore nella creazione del thread a priorità normale" << std::endl;
        return 1;
    }
     // Crea un thread con priorità normale
    result = pthread_create(&normalThread2, nullptr, normalThreadFunction, nullptr);
    if (result != 0) {
        std::cerr << "Errore nella creazione del thread a priorità normale" << std::endl;
        return 1;
    }

    // Attendi che entrambi i thread terminino
    pthread_join(highPriorityThread1, nullptr);
    // pthread_join(highPriorityThread2, nullptr);
    
    pthread_join(normalThread1, nullptr);
    pthread_join(normalThread2, nullptr);



    // Distruggi gli attributi del thread
    pthread_attr_destroy(&attr);

    return 0;
}
