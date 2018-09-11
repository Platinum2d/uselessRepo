#!/bin/sh

#acquisisco i parametri

#mi chiedo se la directory corrente è quella che sto cercando
   #se sì, inizio a scorrerne i contenuti
   #mi chiedo se il contenuto corrente è un file e supera di dimensione quella data
      #se sì, creo il file (usando l'append) SOMMA nella cartella e contemporaneamente lo aggiorno
      #aggiorno il file temp con il numero di file correntemente modificati

#se no, inizio a scorrere tutte le cartelle contenute in quella corrente ed eseguo la chiamata ricorsiva su ognuna di loro

d=$1
dir=$2
n=$3

cd $d

if test "${PWD##*/}" = "$dir"
then
   for file in *
   do
      if test -f "$file" 
      then
	 size=`cat "$file" | wc -c` 
	 if test $size  -gt "$n" 
	 then
            touch /tmp/tmplist
            echo $file >> /tmp/tmplist
	    tmp=`cat /tmp/cont`
	    echo `expr $tmp + 1` > /tmp/cont
	    echo "[$size > $n] "`pwd`/$file
         fi
      fi
   done
   cat /tmp/tmplist > SOMMA
   rm -r /tmp/tmplist
fi

for file in *
do
   if test -d "$file" -a -x "$file"
   then
       $0 $file $2 $3
   fi
done

exit 0
