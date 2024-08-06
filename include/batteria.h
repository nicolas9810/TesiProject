
class Batteria{
    private:

        float Emax;   //  Energia Massima batteria
        float E;            //  Energia Corrente
        float Ea;           //  Energia accumulata in una finestra temporale
        float Ec;           //  Energia consumata in una finestra temporale
        float Ep;           //  Energia Prevista
        float gap;

        float Pin;          //  Potenza in ingresso;
        float Pout;         //  Potenza in uscita;
        int F;              //  Finestra temporale di controllo strategia in minuti
    
        float eta;          //  Eficienza batteria
        float k;            //  costante che modella le perdite 
        float c;            //  livelli minimi di batteria


    public:

    Batteria();
    Batteria (int enMax,float e, float kappa, float con,int finestra); 

    void setEnergiaMax(float e);
    void setEnergia(float e);
    float setEnergiaAccumulata(float e);
    void setEnergiaConsumata(float e);
    void setEnergiaPrevista(float e);


    void setPotenzaIn( float p);
    void setPotenzaOut( float p);
    void setEfficienza( float e);
    void setCoefPerdite( float p);
   
    float getEnergiaMax();
    float getEnergia();
    float getEnergiaAccumulata();
    float getEnergiaConsumata();
    float getPotenzaIn();
    float getPotenzaOut();
    float getEfficienza();
    float getCoefPerdite();

    int SoC();

    float charge(float p, int time);
    float discharge(float p,int time);
    float chargeDischarge();
    float energiaAccumulata(int p,int time);
    float energiaConsumata(int p,int time);
    float energiaPrevista(int p,int time);


    int findConfiguration(float *potenze,int f);



};  