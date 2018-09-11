#!/bin/sh

case $# in
   3) if test ! -d $1 -o ! -x $1 -o $2 -lt 1 
      then
          echo "error: invalid parameter(s)"
	  exit 1
      fi;;
   *) echo "usage: $0 [directory path] [number of candies] [name of candies box]"
      exit 2;;
esac

case $1 in
   */) startpath=$1;;
   *) startpath=$1/;;
esac

PATH=`pwd`:$PATH
export PATH
mkdir $3
cd $3
FCR.sh $1 $2 $3

exit 0
