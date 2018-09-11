#!/bin/bash

#Number of parameters check
if test $# -ne 3; then
	echo "usage: "$0" G D N"
	exit 1
fi

# Variables assignment
 G="$1"
 D="$2"
 N="$3"

#Parameters check

case "$G" in
	/*) ;;
	*) echo "usage: "$0" G D N"
	   exit 2
	   ;;
esac

if test ! -d "$G" -o ! -x "$G"; then
	echo "usage: "$0" G D N"
	exit 3
fi

case "$D" in 
	*/*)  echo "usage: "$0" G D N"
	      exit 4
	      ;;
	*) ;;
esac

expr "$N" + 0 > /dev/null 2>&1
case $? in
	0 | 1) ;; #risultato != 0
	*) echo "N must be a number"; exit 5 ;; #non - numeric argument , es. N = 'a' 
esac

if test $N -le 0; then
	echo "N must be > 0"
	exit 6
fi

#Recursive call

export PATH=$(pwd):$PATH
ese35r.sh "$G" "$D" "$N" 
echo "found $? files! Bye!"

exit 0




 
