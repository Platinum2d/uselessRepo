#!/bin/sh
#controllo sul numero dei parametri(deve essere >= 3), adatta il seguente case ai vari casi
case $# in
   0 | 1 | 2 ) echo " usage : $0 [positive integer] [absolute directory path 1] ... [absolute directory path N]"
	              exit 1;;
   
esac

X=$1
shift
#controllo sulle gerarchie: devono essere indirizzi assoluti
dlist=$*
for d in $dlist 
do
   case "$d" in
   /*) if test ! -d "$d" -o ! -x "$d"
       then
           echo " error : $d is not an accessible directory"
           exit 2
       fi ;;
   *) echo " error : $d is not an absolute path"
      exit 3;;
    esac
done
      
#controllo che il numero sia un intero positivo
expr "$X" + "0" > /dev/null 2>&1
case $? in 
   0 | 1) if test "$X" -le "0"
          then 
             echo " error : $X should be positive"
             exit 4
           fi ;;
   2 | *) echo " error : $X should be a number"
          exit 5 ;;
esac

#aggiorno la varibale d'ambiente PATH per facilitare l'esecuzione ricorsiva
   PATH=`pwd`:"$PATH"
   export PATH

#creo il file temporaneo utile ad ogni esecuzione ricorsiva
   tmpname=/tmp/t$$
   touch "$tmpname"

#eseguo la chiamata al file ricorsivo per ogni gerarchia specificata
for i
do
   FCR.sh "$i" "$X" "$tmpname"
done 

#stampo il numero di file trovati (pari al numero di linee del file temporaneo
printf "\n\n"
echo `wc -l < $tmpname`" files found"

#scorro il file temporaneo e stampo tutti gli indirizzi assoluti dei file trovati
echo
for i in `cat $tmpname`
do
   echo "[F] $i"
   #chiedo il numero K all'utente
   printf "Please type a number (it must be > 0 and < $X) : "
   read K

   #controllo sulla validità del numero
   expr "$K" + 0 > /dev/null 2>&1
   case $? in
      0 | 1) if test "$K" -le 0 -o "$K" -ge "$X"
             then
	         echo " error : $K should be less than $X"
		 #elimino comunque il file temporaneo
		 rm "$tmpname"
		 exit 6
             fi;;
      *) echo " error : $K should be a number" 
	      #elimino comunque il file temporaneo
	      rm "$tmpname"
	      exit 7;;
   esac
   echo "line $K: "`head -$K $i | tail -1`
   echo""
done

#elimino il file temporaneo
rm "$tmpname"

exit 0
