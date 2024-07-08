
#include "previsione.h"
#include "GreenPlantModel.h"
#include <iostream>
#include <cstdlib> 
#include <thread>
#include <chrono>
#include "tempo.h"


int main(int argc, char* argv[]) {

    bool giornoNuovo=true;
    Tempo tempo;
    int giorno=-1;
    bool flag = false;

    

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <time in minutes>" << std::endl;
        return 1;
    }

    int time = std::atoi(argv[1]);
    if (time < 0) {
        std::cerr << "Tempo must be a non-negative integer." << std::endl;
        return 1;
    }

    GreenPlantModel model("./2019.csv");
    try {
        std::cout << "Produced power at time " << time << ": " << model.getProducedPowerByTime(time+1440) << std::endl;
        std::cout << "Produced power at time " << time+60 << ": " << model.getProducedPowerByTime(time+60+1440) << std::endl;
        std::cout << "Produced power at time " << time+(60*2) << ": " << model.getProducedPowerByTime(time+1440+60*2) << std::endl;

    } catch (const std::out_of_range& e) {
        std::cerr << "Tempo exceeds data range: " << e.what() << std::endl;
        return 1;
    }


    inizializzaMatrice();
    for(int i=0;i<DAYS;i++){
        // int temp=i*24*60;
        for(int j=0;j<NN;j++){
            // temp+=60;
            tempo.incrementsMinutes(60);
            setEner(i,j,model.getProducedPowerByTime(tempo.getTimeInMin()));
        }
    }
    printMatriceEnergetica();
    while(true){
        if(tempo.getDay()!=giorno){
            std::cout << "Nuovo giorno è il giorno" << tempo.getDay() <<std::endl;
            nuovoGiorno();
            flag=false;
            printMatriceEnergetica();
            std::cout << "calcolo le previsioni del giorno "<<tempo.getDay()<<std::endl;
            //previsioneDelGiorno(0);
            giorno=tempo.getDay();
            tempo.incrementsMinutes(60);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            // printPrevisioni();
        }
        else{
            int min=tempo.getTimeInMin();
            double misurazioni=model.getProducedPowerByTime(min);
            std::cout << "Produced power at time " << min << " : " << model.getProducedPowerByTime(min)<<std::endl;
            setEnergia(misurazioni,tempo.getMinutes()+tempo.getHours()*60);
            if(misurazioni>0 && !flag){
                flag = true;
                int slot=(tempo.getMinutes()+tempo.getHours()*60)/60;
                previsioneDelGiorno(slot-1,misurazioni);
                printPrevisioni();
            }
            tempo.incrementsMinutes(60);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }





    }


    return 0;
}
