#!/bin/sh

#controllo sul numero dei parametri sia almeno n.
case $# in
   0 | 1 | 2 ) echo " Utilizzo : $0 intero/file indirizzo_assoluto_1 indirizzo_assoluto_2 indirizzo_assoluto_3 [indirizzo_assoluto_4] ... [indirizzo_assoluto_N] intero/file"
	              exit 1;;
esac

#controllo che ogni gerarchia sia espressa in forma assoluta.
#se il parametro fissato è il primo: memorizza il primo parametro, fai uno shift e sei a posto.
 #n=$1
 #shift

#se il parametro fissato è l'ultimo:
#decommentare le righe in cui si controlla che il parametro attualmente analizzato non sia l'ultimo.
   
#utilizzo una variabile apposita per separare i primi N-1 parametri dal N-esimo.
 #dlist=""

#scorro tutti i parametri per eseguire gli opportuni controlli su ognuno.
#utilizzo una variabile numerica per lavorare su tutti i parametri, ad eccezione dell'ultimo.
 #num=1
for d
do
   #if test "$num" -ne "$#"
   #then 
      #dlist="$dlist $d"
      #controllo che l'elemento corrente sia espresso in forma assoluta.
      case $d in
         /*) #controllo che l'elemento corrente sia un direttorio.
             if test ! -d $d
             then
                echo " Errore : $d non è una directory"
                exit 2
	     fi

	     #controllo che l'elemento corrente sia un direttorio accessibile.
             if test ! -x $d 
             then
                echo " Errore : $d non è accessibile"
                exit 3
	     fi;;
	 *) echo " Errore : $d non è un percorso assoluto"
	       exit 4;;
      esac
      #else
      #acquisisco l'ultimo parametro.
         #n=$d
      #fi
      #num=`expr $num + 1`
done
      
#controllo che n sia un numero intero.
expr $n + 0 > /dev/null 2>&1
case $? in 
   0 | 1) #controllo che il numero sia strettamente positivo.
	  if test $n -le 0
          then 
             echo " Errore : $n non è positivo"
             exit 5 
          fi ;;
   *) echo " Errore : $n non è un numero intero"
          exit 6 ;;
esac

#controllo che n sia espresso come nome relativo semplice.
case "$n" in
   */*) echo " Errore : $n non è un nome relativo semplice"
	exit 7;;
esac

#aggiorno la varibale d'ambiente PATH per facilitare l'esecuzione ricorsiva.
PATH=`pwd`:$PATH
export PATH

#creo il file temporaneo, fruibile ad ogni esecuzione ricorsiva. Il PID in coda al nome del file serve a rendere quest'ultimo esclusivo del processo che lo esegue, scongiurando conflitti con altre esecuzioni dello script.
tmpname=/tmp/t$$ 
touch $tmpname

#eseguo la chiamata al file ricorsivo per ogni gerarchia specificata.
for d #in $dlist 
do
   FCR.sh $d $n $tmpname
done 

#stampe varie.
echo ""
echo `wc -l < $tmpname` "occorrenze trovate"
cat $tmpname

#elimino il file temporaneo.
rm $tmpname

#esco dallo script con valore 0 in quanto l'esecuzione non ha presentato alcun tipo di errore se giunta fino a questo punto.
exit 0
