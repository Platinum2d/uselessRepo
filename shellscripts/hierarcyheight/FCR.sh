#!/bin/sh

#in $2 c'è il percorso in cui si trova il file con il conteggio massimo attuale, in $3 c'è il percorso attuale

cd $3 2>error_report.txt

maxl=`cat $2/"repval"`
#echo "Numero massimo: $maxl, io ho $1"
#read pause
if test $1 -gt $maxl
then
   echo `expr $maxl + 1` > $2/"repval"
fi

#echo "lista file:"
#echo `ls`
#read pause

for file in `ls` 
do
   #echo "analizzo $file"
   #test -d $file
   #tst=$?
   #echo "esito risultato: $tst, il percorso attuale è `pwd`"
   #read pause
   if test -d $file -a -x $file
   then
     #echo "analizzo la cartella $file"
     #read pause
     FCR.sh `expr $1 + 1` $2 $3""$file/
   fi
done

exit 0
