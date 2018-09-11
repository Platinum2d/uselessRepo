
#!/bin/sh

#controllo sul numero dei parametri(deve essere >= n), adatta il seguente case ai vari casi
case $# in
   0 | 1 | 2 ) echo " Utilizzo : $0 intero indirizzo_assoluto_1 indirizzo_assoluto_2 [indirizzo_assoluto_3]... [indirizzo_assoluto_n]"
	              exit 1;;
esac

#controllo sulle gerarchie: devono essere indirizzi assoluti
n=$1
shift

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
      
#controllo che il numero passato sia un numero strettamente positivo e minore di 255
expr "$n" + "0" > /dev/null 2>&1
case $? in 
   0 | 1) if test "$n" -le "0"
          then 
             echo " Errore : $n non è positivo"
             exit 5 
          fi 
	  if test "$n" -ge 255
	  then
	     echo " Errore : $n non è minore di 255"
	     exit 6
	  fi
	  ;;
   *) echo " Errore : $n non è un numero"
          exit 7 ;;
esac

#aggiorno la varibale d'ambiente PATH per facilitare l'esecuzione ricorsiva
PATH=`pwd`:"$PATH"
export PATH

#creo il file temporaneo utile ad ogni esecuzione ricorsiva (conterrà i nomi dei file). Il PID in coda il nome del file serve arendere quest'ultimo esclusivo del processo che lo esegue, scongiurando conflitti con altre esecuzioni.
tmpnames="/tmp/names$$" 
touch "$tmpnames"

#eseguo la chiamata al file ricorsivo per ogni gerarchia specificata
echo ""
echo "Directories trovate:"
for d 
do
   FCR.sh "$d" "$n" "$tmpnames" "$tmpsizes"
done 

#stampe varie
echo ""
echo `wc -l < "$tmpnames"`" file trovati"
#elimino il file temporaneo
rm "$tmpnames"

exit 0
