
#!/bin/sh

#controllo sul numero dei parametri(deve essere >= n), adatta il seguente case ai vari casi
case $# in
   0 | 1 ) echo " Utilizzo : $0 indirizzo_assoluto carattere_1 [carattere_2] ... [carattere_n]"
	              exit 1;;
esac

#memorizzo l'indirizzo assoluto e faccio lo shift dei parametri, così da ottenere i soli caratteri
g="$1"
shift

#verifico che la directory passata come parametro sia in forma assoluta
case "$g" in
   /*) #verifico che sia una directory esistente e attraversabile
       if test ! -d "$g" 
       then
          echo " Errore : $g non è una directory"
          exit 2
       else
          if test ! -x "$g" 
          then
             echo " Errore : $g non è accessibile"
             exit 3
          fi
       fi;;
   *) echo " Errore : $g non è in forma assoluta"
      exit 4;;
esac

#creo un file temporaneo per contenere i caratteri da verificare
tmpch="/tmp/tc$$"
touch "$tmpch"

#verifico che tutti gli altri argomenti siano dei singoli caratteri
for c 
do
   case "$c" in
      ?) echo "$c" >> "$tmpch" ;;
      *) echo " Errore : $c non è un singolo carattere"
	 rm "$tmpch" 
	 exit 5;;
   esac
done

#aggiorno la varibale d'ambiente PATH per facilitare l'esecuzione ricorsiva
PATH=`pwd`:"$PATH"
export PATH

#creo il file temporaneo utile ad ogni esecuzione ricorsiva. Il PID in coda il nome del file serve arendere quest'ultimo esclusivo del processo che lo esegue, scongiurando conflitti con altre esecuzioni.
tmpname="/tmp/t$$" 
touch "$tmpname"

#eseguo la chiamata al file ricorsivo
FCR.sh "$g" "$tmpch" "$tmpname"

#elaboro il contenuto del file temporaneo
echo ""
echo `wc -l < "$tmpname"`" file trovati"
cat "$tmpname"

#elimino il file temporaneo
rm "$tmpname"
rm "$tmpch"

exit 0
