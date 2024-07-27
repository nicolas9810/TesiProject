# Modello Energetico

$$


\left\{
\begin{aligned}
& \text{Max} \{P_i\} \\
& E_P(t_n+1)-P_i*F+gap+E(t_n)>0 \\
& SoC=\frac{E_(t_n)}{E_{max}}> c \\
& E_C(t_n) =\int_{n-1}^n{P_{out}dt} \\
& E_A(t_n) =\int_{n-1}^n{P_{in}dt} \\
& E(t_n) =\int_{0}^n{(\eta-k)P_{in}dt} \\
\end{aligned}
\right.
$$

con


$\eta$ e $k$  costanti

$c$ = percentuale minima di batteria oltre la quale non andare

$F$ = finestra temporale di controllo/monitoraggio della strategia

$P_{in}$ = Potenza in ingresso

$P_{out}$ = Potenza in uscita

$E_A(t_n)$ = Energia accumulata tra il tempo $t_{n-1}$ e $t_{n}$

$E_C(t_n)$ = Energia consumata tra il tempo $t_{n-1}$ e $t_{n}$, vaore utilizzato in $gap$, da capire come

$E_(t_n)$ = energia nella batteria al tempo $t_n$

$SoC$ = percentuale batteria al tempo $t_n$


