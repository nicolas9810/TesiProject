class Modello{

    float *previsioni;
    int dim;
public:
    Modello();
    void setPrevisioni(float *p,int dim);
    double getProducedPowerByTime(double time);

};