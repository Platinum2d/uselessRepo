
#!/bin/sh

#controllo sul numero dei parametri(deve essere >= n), adatta il seguente case ai vari casi
case $# in
   0 | 1 ) echo " Utilizzo : $0 indirizzo_assoluto_1 indirizzo_assoluto_2 [indirizzo_assoluto_4] ... [indirizzo_assoluto_N]"
	              exit 1;;
esac

#controllo che tutti i percorsi siano espressi in forma assoluta
for d
do
   case "$d" in
      /*) if test ! -d "$d"
          then
             echo " Errore : $d non è una directory"
             exit 2
          fi
          if test ! -x "$d" 
          then
             echo " Errore : $d non è accessibile"
             exit 3
	  fi;;
       *) echo " Errore : $d non è un percorso assoluto"
          exit 4;;
   esac
done

#aggiorno la varibale d'ambiente PATH per facilitare l'esecuzione ricorsiva
PATH=`pwd`:"$PATH"
export PATH

#creo il file temporaneo utile ad ogni esecuzione ricorsiva. Il PID in coda il nome del file serve arendere quest'ultimo esclusivo del processo che lo esegue, scongiurando conflitti con altre esecuzioni.
tmpname="/tmp/t$$" 
touch "$tmpname"

#eseguo la chiamata al file ricorsivo per ogni gerarchia specificata
for d 
do
   FCR.sh "$d" "$tmpname"
done 

#stampe varie
echo ""
cat "$tmpname" | more
echo `wc -l < "$tmpname"`" occorrenze trovate"

#elimino il file temporaneo
rm "$tmpname"

exit 0
