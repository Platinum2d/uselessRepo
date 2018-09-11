
#!/bin/sh

#controllo sul numero dei parametri(deve essere >= n), adatta il seguente case ai vari casi
case $# in
   0 | 1 ) echo " Utilizzo : $0 indirizzo_assoluto_1 [indirizzo_assoluto_2] ... [indirizzo_assoluto_N] .estensione"
	              exit 1;;
esac

#controllo sulle gerarchie: devono essere indirizzi assoluti
   
#utilizzo una variabile apposita per separare i primi N-1 parametri dal N-esimo
dlist=""

#ciclo per il controllo effettivo su ogni valore passato come parametro
#utilizzo una variabile numerica per lavorare su tutti i parametri, ad eccezione dell'ultimo
num=1
for d
do
   if test "$num" -ne "$#"
   then 
      dlist="$dlist $d"
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
      else
      #acquisisco l'ultimo parametro
         PAT="$d"
      fi
      num=`expr $num + 1`
done
      
#controllo che l'ultimo parametro sia nella forma indicata (.<stringa>)
case "$PAT" in
   .*) ;;
   *) echo " Errore : $PAT non è espresso nella forma .<pattern>"
      exit 5;;
esac


#aggiorno la varibale d'ambiente PATH per facilitare l'esecuzione ricorsiva
PATH=`pwd`:"$PATH"
export PATH

#creo il file temporaneo utile ad ogni esecuzione ricorsiva. Il PID in coda il nome del file serve arendere quest'ultimo esclusivo del processo che lo esegue, scongiurando conflitti con altre esecuzioni.
tmpname="/tmp/t$$" 
touch "$tmpname"
tmpc="/tmp/c$$"
echo "0" > "$tmpc"

#eseguo la chiamata al file ricorsivo per ogni gerarchia specificata
for d in $dlist 
do
   FCR.sh "$d" "$PAT" "$tmpname" "$tmpc"
done 

#stampo le occorrenze e il loro numero in maniera leggibile
echo ""
occ="`wc -l < "$tmpname"`" 
echo $occ" occorrenze trovate"
more "$tmpname"

sc="a"
while test "$sc" = "a" -o "$sc" = "b"
do
   echo ""
   echo "a) Rivedere la lista"
   echo "b) Stampare il contenuto di un file"
   echo "altro) Terminare"
   echo ""
   printf "Selezionare un'opzione : "
   read sc
   case "$sc" in
      a) clear
	 more "$tmpname";;
      b) echo ""
	 printf "Inserire il numero del file (compreso tra 1 e $occ) : "
	 read prog
	 path=`head -$prog "$tmpname".onlynames | tail -1`
	 clear
	 echo " ## $path ##"
	 more "$path"
	 ;;
   esac

done
#elimino il file temporaneo
rm "$tmpc"
rm "$tmpname"

exit 0
