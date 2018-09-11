
#!/bin/sh
#descrizione dei parametri

#mi sposto nella cartella prossima alla ricorsione e acquisisco i parametri
cd "$1"

#acquisisco i parametri
PAT="$2"
tmpfile="$3"
tmpc="$4"

#scorro il contenuto della directory corrente
for i in *
do
   #controllo che il file trovato sia una directory e sia attraversabile. Nel caso, effettuo su di essa la chiamata ricoriva
   if test -d "$i" -a -x "$i"
   then
      "$0" "$i" "$PAT" "$tmpfile" "$tmpc" #parametri 
   fi

   #controllo che i sia un file e sia leggibile
   if test -f "$i" -a -r "$i"
   then
      #controllo che il file abbia l'estensione desiderata
      case "$i" in
         *$PAT)	actual=`cat "$tmpc"`
		echo `expr "$actual" + 1` > "$tmpc"
		echo "`expr "$actual" + 1`"") "`pwd`/"$i" >> "$tmpfile"
		echo `pwd`/"$i" >> "$tmpfile".onlynames;;
      esac
   fi
done

exit 0
