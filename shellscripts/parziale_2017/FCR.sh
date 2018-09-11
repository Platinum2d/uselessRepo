#!/bin/sh

cd "$2"
f=$1
t=$3

for fname in *; do
  if test -d "$fname" -a -x "$fname" 
  then
    "$0" "$1" "$fname" "$t"
  fi

  #controllo che il file corrente sia un file leggibile con lo stesso nome di quello desiderato
  if test -f "$fname" -a -r "$fname" -a "$fname" = "$f"
  then
    #creo il file sorted
    sort -f "$fname" > sorted

    #aggiorno il file temp con l'indirizzo assoluto del file trovato
    echo `pwd`/sorted >> $t
  fi 
done

exit 0
