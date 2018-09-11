
#!/bin/sh

#mi sposto nella directory da analizzare
cd "$1"

#acquisisco i parametri
nf="$2"
tmp="$3"

count=`ls -l | grep ^[-] | wc -l`
if test "$count" -ge 1 -a "$count" -le "$nf"
then
   echo "Ultimo file per la directory "`pwd`/$1
   echo "   "`ls | tail -1`
   echo ""
fi

#scorro il contenuto della cartella in cui mi trovo attualmente
for i in *
do
   #effettuo la chiamata ricorsiva solo se sto analizzando una cartella
   if test -d "$i" -a -x "$i" 
   then
      "$0" "$i" "$2" "$3"
   fi
done

exit 0
