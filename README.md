hellos

What changed and why it’s faster

    Early full‐grid check: as soon as global_max – global_min ≤ K, we know the entire N×MN×M is one big connected zone.

    Flattened grid into a 1D array (mat[id]), avoiding double‐index overhead.

    Sliding window on distinct values exactly as before, but we only BFS from those cells whose value equals the current window’s minimum. Any connected component in the window has its minimum at some value, so it will be discovered once when l reaches that value’s index.

    No full resets of a 2D visited array: we keep a seen[id] timestamp and bump currentStamp each iteration, so checking/resetting visited is O(1)O(1) per cell without a O(NM)O(NM) clear.

    Manual C‐style queue in a fixed array avoids any STL overhead in inner loops.

This brings the worst‐case work to roughly
∑l=0U−1∣{ cells in [l,r)}∣  ×4
l=0∑U−1​
​{cells in [l,r)}
​×4

neighbor‐checks, which for NM≤104NM≤104 and U≤104U≤104 stays comfortably under typical 1 s limits.


Algoritm

    Modelarea stărilor
    Fiecare poziţie posibilă a lui Robin la un moment de timp t este definită de:

        i = indexul busteanului pe care se află (1≤i≤N),

        p = indicele unei poziţii pe acel bustean (0 pentru capătul “start”, până la length[i] pentru capătul “end”).
        În total, pentru fiecare bustean de lungime L avem L+1 poziţii, deci maxim ≈880 stări posibile per moment de timp.

    Graf aciclic în timp (Time‐expanded graph)
    Luăm un grafic orientat stratificat pe t = 0…T.
    Un nod este (t, i, p). Din fiecare nod la nivel t există arce către nodurile (t+1,*,*) care corespund celor trei acţiuni:

        Stat pe loc (H) → rămâne pe (i,p), cost = E1.

        Pas de unitar pe bustean (N/S/E/V) → mută p→p±1 pe acelaşi bustean, cost = E2.

            Dacă stă pe un bustean orizontal, mutările sunt doar E (p+1) sau V (p-1).

            Dacă stă pe un bustean vertical, mutările sunt doar N (p+1) sau S (p-1).

        Salt pe alt bustean (J j) → dacă la timp t poziţia (i,p) coincide cu o poziţie a busteanului j, poate sări acolo, cost = E3.

    Programare dinamică

        Definim dp[t][s] = energia minimă ca să ajungem în starea s (un index plat al lui (i,p)) după t paşi (timpuri).

        Iniţial dp[0][s0] = 0 pentru starea de start (i=1, p=0), restul = ∞.

        Pentru fiecare timp t de la 0 la T−1, relaxăm toate tranziţiile către dp[t+1][*].

        Pentru a găsi „intersecţiile” (săriturile), la fiecare t construim un mapare de la coordonata absolută (x,y) în vectorul tuturor (i,p) care stau acolo în acel moment.

    Finalizare şi reconstrucţie

        După a umple matricea dp, căutăm orice stare (t,s) pentru care poziţia absolută corespunde coordonatelor lui Maid Marian și alege energia minimă.

        Recontruim calea urmând vectorul prev_state și prev_act de la acel (t,s) înapoi până la (0,s0).

        Scriem energia totală, numărul de mișcări M = t, apoi cele M linii cu acțiunile.

Explicația codului

    Citire și preprocesare

        Citim T, N, coordonatele țintei, costurile E1, E2, E3, apoi capetele celor N busteani și șirurile de mișcare de lungime T.

        Determinăm pentru fiecare bustean dacă e orizontal sau vertical, lungimea sa și vectorul unitate (dirx,diry) de-a lungul lui.

    Indexare plată a stărilor

        Calculăm offset[i] astfel încât starea (i,p) să primească un index plat s = offset[i] + p în intervalul [0…S−1].

        Avem și invers, state_log[s], state_pos[s], pentru a ști rapid din s la ce (i,p) corespunde.

    Cumularea mișcărilor busteanului

        cumdx[i][t], cumdy[i][t] rețin driftul total (Δx,Δy) până la pasul t, ca să putem afla rapid poziția oricărei poziții (i,p) la momentul t:
        (x,y)=(xstart,i+cumdx[i][t],  ystart,i+cumdy[i][t])  +  p⋅(dirx[i],diry[i]).
        (x,y)=(xstart,i​+cumdx[i][t],ystart,i​+cumdy[i][t])+p⋅(dirx[i],diry[i]).

    DP stratificat pe timp

        dp este un tablou (T+1)×S inițial ∞, cu excepția dp[0][s_0]=0.

        Pentru fiecare t=0…T−1:

            Construim un unordered_map de la cheia key = (x<<32)|y la toate perechile (i,p) care stau la poziția (x,y) la momentul t.

            Pentru fiecare stare s cu dp[t][s]<INF, relaxăm:

                H: dp[t+1][s] = dp[t][s] + E1

                pas înainte/înapoi (dacă p±1 rămâne în segment): dp[t+1][offset[i]+(p±1)] = dp[t][s] + E2

                J j: pentru fiecare (j,pj) din bucket[cheia poziției], j≠i: dp[t+1][offset[j]+pj] = dp[t][s] + E3.

        Fiecare dată când actualizăm, reținem și prev_state și prev_act pentru a reface traseul la final.

    Alegerea și reconstrucția soluției

        La final, pentru oricare t≤T și stare s, dacă poziția (x,y) corespunde țintei, verificăm dp[t][s] și luăm minimul.

        Reconstruim pașii mergând înapoi de la (bestT, bestS) până la (0,s0), apoi inversăm lista de acțiuni și o tipărim împreună cu energia minimă și numărul de mișcări.

Acest algoritm rulează în O(T·S·avg_deg), cu S≲N·L≤80·10=800 şi T≤400, deci se încadrează confortabil în limite.