#!/bin/sh
#FCP.sh: file per il controllo dei parametri e per l'invocazione del file ricorsivo

#controllo sul numero dei parametri (deve essere >= 5)
case $# in
      0 | 1 | 2 | 3 | 4) echo " usage: $0 absolute directory path 1, 2, 3, 4 ... [absolute directory path N] positive integer"
	      exit 1;;
esac

#stablisco quante sono le gerarchie specifiate
h=`expr $# - 1`

#separo le gerarchie dal parametro finale
for d in $*
do
   #utilizzo un file temporaneo come contenitore di tutti i parametri
   echo $d >> /tmp/plist
done

#solo l'ultima riga non contiene una gerarchia, quindi seleziono solo le prime h righe del file
dlist=`cat /tmp/plist | head -$h`

#l'ultima riga del file temporaneo è quella che contiene il noem del file passato come ultimo parametro
n=`cat /tmp/plist | tail -1`
rm /tmp/plist

#controllo sulle gerarchie: devono essere tutte indicate da percorsi assoluti, oltre a dover essere accessibili
for d in $dlist
do 
   case $d in
      /*) if test ! -d $d -o ! -x $d
          then
              echo " error : $d must be an accessible directory"
	      exit 2
          fi;; 
      *) echo " error : $d should be an absolute path"
          exit 3;;
   esac
done

#controllo che l'ultimo parametro sia un numero e che sia un intero positivo
expr $n + 0 > /dev/null 2>&1 
case $? in
   0 | 1) if test $n -le 0
          then
	     echo " error : last parameter must be a positive integer"
	     exit 4
          fi;;
   2 | *) echo " error : last parameter must be a positive integer"
	   exit 5;;
esac

#aggiorno la variable d'ambiente PATH per dare luogo all'esecuzione ricorsiva
PATH=$PATH:`pwd`
export PATH

#creo il file temporaneo da usare in ogni chiamata al file ricorsivo
tmpname=/tmp/t
touch $tmpname

#eseguo la chiamata al file ricorsivo per ognuna delle gerarchie specificate
for i in $dlist
do
  FCR.sh "$i" "$n" "$tmpname"
done

echo "ho trovato "`wc -l < $tmpname`" file"

for line in `cat $tmpname`
do
   echo "[F] $line"
   echo "   linea richiesta: "`tail -$n $line | head -1`
   read pause
done

rm $tmpname

exit 0
