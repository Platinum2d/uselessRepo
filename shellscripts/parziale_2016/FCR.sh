#!/bin/sh
#FCR.sh
#descrizione dei parametri

#echo "i miei parametri: $1 $2 $3"
#read pause

cd "$1"
X=$2
tmpname=$3

#chiamata ricorsiva fissa su ogni directory. La parte di elaborazione personalizzata puoò essere messa sia qui che dentro al for: dipende dal problema

#controllo che la directory in cui mi trovo non contenga ulteriori sub-directory
##echo "analizzo $1"
if test `ls -l | grep ^[d] | wc -l` -eq 0
then
  #scorro tutti i file all'interno della directory
  if test `ls -l | wc -l` -gt 0
  then
     for file in `ls` 
     do
	if test -r "$file" 
	then
           #echo "    sto analizzando $1/$file"
           #controllo che il file corrente abbia almeno X linee
           lines=`wc -l < $file`
	   #echo "   ha $lines linee, ne ha più di $X?"
	   #read pause
           if test "$lines" -ge "$X"
           then
               #memorizzo l'indirizzo assoluto del file nel file temporaneo
               echo "`pwd`/$file" >> $tmpname
           fi
        fi
     done
  fi 
fi


for i in *
do
   if test -d "$i" -a -x "$i"
   then
      "$0" "$i" "$X" "$tmpname" #parametri 
   fi
done

exit 0
