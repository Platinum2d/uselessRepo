# Operating System Course - DIEF UNIMORE #
Esercizi avanzati (ricerche ricorsive) risolti mediante l'utilizzo di due script. Il primo per controllare i parametri, il secondo per effetuare la ricerca ricorsiva. Variabile d'ambiente PATH.

Note: per utilizzare questa guida con vim utilizzare i seguenti due comandi 
```
:set wrap 
:set linebreak
```

## Riferimenti ##
http://tldp.org/HOWTO/Bash-Prog-Intro-HOWTO.html

## Esercizi ##
01. (es31.sh) La parte in Shell prevede un parametro che deve essere il nome assoluto di un direttorio che identifica una gerarchia (G) all'interno del file system. Il programma deve determinare il livello corrente di profondita' raggiunto in ogni invocazione ricorsiva e riportare sullo standard output il massimo livello della gerarchia compresa la radice della gerarchia G. Per conservare, il massivo livello di profondita' raggiunto utilizzare un file temporaneo.

02. (es32.sh) Si svolga l'esercizio precedente utilizzando, al posto di un file temporaneo, i valori di ritorno delle invocazioni ricorsive.

02. (es33.sh) La parte in Shell deve prevedere tre parametri: il primo deve essere il nome assoluto di un direttorio che identifica una gerarchia (G) all'interno del file system, mentre il secondo parametro e il terzo parametro devono essere considerati singoli caratteri numerici Cx e Cy. Il programma deve cercare nella gerarchia G specificata (compresa la radice) tutti i direttori che contengono almeno un file che contiene sia nel nome che nel contenuto entrambi i caratteri Cx e Cy. Si riporti il nome assoluto di tali direttori sullo standard output. In ogni direttorio trovato, si deve invocare la parte in C, passando come parametri i nomi dei file trovati (F0... FN-1) e i caratteri numerici Cx e Cy.

03. (es34.sh) La parte in Shell deve prevedere due parametri: il primo parametro deve essere il nome assoluto di un direttorio che identifica una gerarchia (G) all'interno del file system, mentre il secondo parametro deve essere considerato un numero intero strettamente positivo (K). Il programma deve cercare nella gerarchia G specificata tutti i direttori (compresa la radice) che contengono un numero di file (solo file e non direttori) compreso fra 1 e K (estremi inclusi): si riporti il nome assoluto dell'ultimo file trovato (Fi) sullo standard output. Al termine dell'intera esplorazione ricorsiva di G, si deve invocare la parte C passando come parametri tutti i nomi assoluti dei file Fi trovati.

04. (es35.sh) La parte in Shell deve prevedere tre parametri: il primo deve essere il nome assoluto di un direttorio che identifica una gerarchia (G) all'interno del file system, il secondo deve essere il nome relativo semplice di un direttorio (D), mentre il terzo parametro deve essere considerato un numero intero (N) strettamente positivo. Il programma deve cercare nella gerarchia G specificata tutti i direttori di nome D che contengono file (F) che hanno dimensione in byte strettamente maggiore di N: si riporti il nome assoluto di tali direttori sullo standard output e si contino globalmente i file F che soddisfano la condizione precedente. In ogni direttorio trovato, si deve creare un file di nome SOMMA avente come contenuto la concatenazione di tutti i file F del direttorio che soddisfano la condizione precedente. La parte in Shell, dopo aver calcolato nella gerarchia G il numero totale di file che soddisfano la condizione indicata, lo deve stampare sullo standard output.

05. (es36.sh) La parte in Shell deve prevedere due parametri: il primo parametro deve essere il nome assoluto di un direttorio che identifica una gerarchia (G) all'interno del file system e il secondo parametro deve essere una stringa (F). Il programma deve cercare nella gerarchia G specificata tutti i direttori (compresa la radice) che contengono, fra gli altri, 2 file i cui nomi sono uguali a F.1 e F.2 e la cui lunghezza in linee e in caratteri deve essere la stessa: si riporti il nome assoluto di tali direttori sullo standard output. Per ogni direttorio che soddisfa la condizione precedente, si deve invocare la parte C passando i due file trovati F.1 e F.2 come parametri.

06. (es37.sh) La parte in Shell deve prevedere due parametri: il primo deve essere il nome assoluto di un direttorio che identifica una gerarchia (G) all'interno del file system; mentre il secondo deve essere il nome relativo (semplice) di un direttorio (D). Il programma deve cercare nella gerarchia G specificata (compresa la radice) tutti i direttori che hanno nome D e che contengono un numero di file pari al numero di sotto-direttori piu' 2: si riporti il nome assoluto di tali direttori sullo standard output. In ogni direttorio trovato, si deve creare un numero di sotto-direttori in modo che venga pareggiato il numero di file e sotto-direttori: i nomi dei sotto-direttori da creare devono essere chiesti interattivamente all'utente. La parte in Shell deve calcolare il numero totale di sotto-direttori effettivamente creati nella gerarchia G e stamparlo sullo standard output.

07. (es38.sh) La parte in Shell deve prevedere un numero variabile N+1 di parametri: il primo deve essere il nome assoluto di un direttorio che identifica una gerarchia (G) all'interno del file system; mentre gli altri N parametri devono essere considerati singoli caratteri C1, C2, . . . CN. Il programma deve cercare nella gerarchia G specificata tutti i file che siano di dimensione pari e che contengono (nel contenuto) tutti i caratteri Ci. Si riporti il nome assoluto di tali file sullo standard output. Per ogni file trovato F, si deve invocare la parte in C, passando come parametri F e i caratteri C1, C2, . . . CN.

08. (es39.sh) La parte in Shell deve prevedere un numero variabile di parametri (maggiore o uguale a 3): il primo parametro deve essere il nome relativo semplice F di un file, mentre gli altri N devono essere nomi assoluti di direttori (G1, G2, ..., Gn) all'interno del file system. Per ognuna della N gerarchie Gi, il programma deve esplorare la gerarchia corrispondente e cercare tutti i file leggibili il cui nome sia F. Per ognuno di tali file, il programma deve produrre un nuovo file (di nome sorted) con lo stesso contenuto di F, ma ordinato in ordine alfabetico. Per ogni file sorted creato, si deve riportare sullo standard output il suo nome assoluto. Per ogni gerarchia analizzata, il programma riporta su standard output il numero di file sorted creati. Il programma riporta infine su standard output il numero complessivo di file sorted creati all'interno di tutte le gerarchie analizzate.





