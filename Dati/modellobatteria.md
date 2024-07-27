# Modello Matematico di una Batteria

## Definizione delle Variabili

- $P_{in}(t)$: Potenza in ingresso alla batteria al tempo $t$ (Watt).
- $E_{max}$: Capacità massima della batteria (Wh).
- $E(t)$: Energia accumulata nella batteria al tempo $t$ (Wh).
- $\eta$: Efficienza di carica/scarica della batteria (adimensionale, tra 0 e 1).
- $P_{loss}(t)$: Perdite di potenza della batteria al tempo $t$ (Watt).

## Bilancio Energetico

Il bilancio energetico della batteria può essere scritto considerando che l'energia accumulata in un piccolo intervallo di tempo $dt$ dipende dalla potenza in ingresso, dall'efficienza e dalle perdite:

$$
dE(t) = \eta P_{in}(t) \, dt - P_{loss}(t) \, dt
$$

## Modellazione delle Perdite

Le perdite di potenza $P_{loss}(t)$ possono dipendere da diversi fattori, come la resistenza interna della batteria. Una semplice modellazione delle perdite potrebbe essere proporzionale alla potenza in ingresso:

$$
P_{loss}(t) = k P_{in}(t)
$$

dove $k$ è una costante che rappresenta le perdite proporzionali alla potenza.

## Equazioni Energia

Unendo il bilancio energetico e il modello delle perdite, otteniamo:

$$
dE(t) = (\eta P_{in}(t) - k P_{in}(t)) \, dt
$$

$$
dE(t) = (\eta - k) P_{in}(t) \, dt
$$

Integrando questa equazione nel tempo, otteniamo l'energia accumulata:

$$
E(t) = \int_0^t (\eta - k) P_{in}(\tau) \, d\tau
$$

## Stato di Carica (SoC)

Lo stato di carica è la proporzione dell'energia accumulata rispetto alla capacità massima della batteria:

$$
SoC(t) = \frac{E(t)}{E_{max}}
$$

## Modello Completo

Il modello matematico completo della batteria può essere riassunto come segue:

1. **Equazione differenziale dell'energia accumulata**:
  
    $$
    E(t) = \int_0^t (\eta - k) P_{in}(\tau) \, d\tau
    $$

2. **Stato di carica (SoC)**:
  
    $$
    SoC(t) = \frac{E(t)}{E_{max}}
    $$
