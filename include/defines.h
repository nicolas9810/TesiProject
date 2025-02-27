/*
Frequenza di misurazione
valori prei dal paper Evaluation and Design Exploration of Solar Harvested-Energy Prediction Algorithm
Le possibilità sono
.288 -> ogni 5 min 
.96  -> ogni 15 min
.72  -> ogni 20 min 
.48  -> ogni 30 min
.24  ->  ogni 60 min
*/
#define N 288
#define FREQUENZA 24*60/N
/* Numero ideale tra 10 e 11*/
#define DAYS 10

#define CHARGING 0
#define DISCHARGING 1

#define MAXPOWER 3
#define MEDPOWER 2
#define MINPOWER 1

#define OFF      0
#define ON       1


#define SOLEGGIATO  0
#define NUVOLOSO    1

#define PREDWINDOW  60/FREQUENZA

//numero di intterrupt che devo gestire
/*
0 = cambio configurazione
1 = batteria < 30%
*/
#define INTERRUPTS     10
#define INTSPEGNI       0
#define INTCONFIG       1

// #define DEBUG false
#define DEBUG_MODE

