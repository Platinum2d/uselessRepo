#!/bin/sh

dir=$1
levels=$2
boxname=$3

for i in `seq $levels`
do
  mkdir $boxname$i
  cd $boxname$i 
done

exit 0
