#include "strategiaVirtuale.h"
#include <thread>
#include <chrono>



StrategiaVirtuale::StrategiaVirtuale():Strategia::Strategia(nullptr){}
StrategiaVirtuale::StrategiaVirtuale(epever *dev, Tempo *t):Strategia::Strategia(dev){
    time=t;
    modelloPannello=new GreenPlantModel("/home/nicolas/Codice/TesiProject/csv/2019.csv");
    
    //std::thread scorreredeltempo(&StrategiaVirtuale::gestioneTempo,this);
}

/*StrategiaVirtuale::StrategiaVirtuale(epever *dev, Tempo *t):Strategia::Strategia(dev){
    tempo=t;
    modelloPannello=new GreenPlantModel("/home/nicolas/Codice/TesiProject/csv/2019.csv");
    
    //std::thread scorreredeltempo(&StrategiaVirtuale::gestioneTempo,this);
}*/

int StrategiaVirtuale::strategia(float batteryCharge){
    
    #ifdef VIRTUALE
    cout<<"[T STRATEGIA- straVirtuale] calcolo strategia "<<endl;
    #endif
    float outputPower = device->getOutputPower();
    #ifdef VIRTUALE
    cout<<"[T STRATEGIA- straVirtuale] potenza in uscita: "<< outputPower<<endl;
    #endif
    float inputPower = modelloPannello->getProducedPowerByTime(time->getTimeInMin());
    #ifdef VIRTUALE
    cout<<"[T STRATEGIA- straVirtuale] potenza in ingresso: "<< inputPower<<endl;
    #endif
    if(outputPower>inputPower){
        #ifdef VIRTUALE
        cout<<"[T STRATEGIA- straVirtuale] strategia migliore: OFF "<<endl;
        #endif
        return OFF;
    }
    else {
        #ifdef VIRTUALE
        cout<<"[T STRATEGIA- straVirtuale] strategia migliore: MAXPOWER"<<endl;
        #endif
        return MAXPOWER;
        }
}

