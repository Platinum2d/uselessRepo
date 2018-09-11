#!/bin/sh

dir=$1
f=$2

cd $dir

#provo a fare il matching di f.1 e f.2: deve essere esattamente 2
gc1=`ls | grep $f.1 | wc -l`
gc2=`ls | grep $f.2 | wc -l`

#controllo se c'è esattamente un file che si chiama $f.1 ed esattamente uno che si chiama $f.1 
if test "$gc1" -eq 1 -a "$gc2" -eq 1
then
   line1=`cat $f.1 | wc -l`
   line2=`cat $f.2 | wc -l`
   chars1=`cat $f.1 | wc -c`
   chars2=`cat $f.2 | wc -c`

   #controllo che questi due file abbiano lo stesso numero di caratteri e linee
   if test $line1 -eq $line2 -a $chars1 -eq $chars2 
   then
	   echo "[D] `pwd`/"$dir"($f.1 : $line1 line(s), $chars1 char(s); $f.2 : $line2 line(s), $chars2 char(s))"
   fi
fi

#chiamate ricorsiva alle subdirectory
for file in *
do
   if test -d "$file" -a -x "$file"
   then
	 $0 $file $2  
   fi
done

exit 0
