# Modello Energetico

$$


\left\{
\begin{aligned}
& \text{Max} \{P_i\} \\
& E_P(t_n,f)-P_i*f+E(t_n)>c \\
& c=E(t_{n})-P_0*\tau \\
& zE(t{n}) - c >0 \\
& SoC(t_n)=\frac{E_(t_n)}{E_{max}}> 0 \\
& E(t_n) =\int_{0}^n{(\eta-k)P_{in}dt} \\
\end{aligned}
\right.
$$

con

$$
z=
\left\{
    \begin{aligned}
    & 1 \text{ se } \frac{E_d}{E_{max}} > 1 \\
    & \frac{E_d}{E_{max}} \text{      altrimenti} 
    \end{aligned}
\right.
$$

$z$ : lo utilizzo per capire se il giorno dopo ci sarà abbastanza energia per ricaricare tutta la batteria o in che proporzione ci sarà

$E_d$ : energia prevista per il giorno successivo

$\eta$ e $k$  costanti che rappresentano efficienza e perdite rispettivamente

$c$ : valore di energia minimo necessario per arrivare al giorno dopo con la configurazion $0$

$\tau$ = tempo che manca al primo momento in cui ci sarà di nuovo energia il giorno dopo.


$f$ = finestra temporale di previsione e controllo/monitoraggio della strategia

$P_{i}$ = Potenza utilizzata dalla configurazione $i$

$E_(t_n)$ = energia nella batteria al tempo $t_n$

$SoC(t_n)$ = percentuale batteria al tempo $t_n$


