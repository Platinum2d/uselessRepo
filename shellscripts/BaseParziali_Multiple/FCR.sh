
#!/bin/sh

#mi sposto nel direttorio da analizzare
cd $1

#acquisisco i parametri
tmp=$3

#scorro il contenuto del direttorio in cui mi trovo attualmente
for i in *
do
   #controllo che l'elemento in analisi sia un direttorio e che sia accessibile
   if test -d $i -a -x $i 
   then
      #effettuo la chiamata ricorsiva sul direttorio
      $0 `pwd`/$i $2 $3
   fi
   
   #controllo che l'elemento in analisi sia un file e che sia leggibile
   if test -f $i -a -r $i
   then
      echo Niente errori per ciclo vuoto
   fi
done

exit 0
