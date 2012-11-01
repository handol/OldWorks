#!/bin/bash

pkill -x "pasgw"
sleep 3

if [ -f pasgw.log ];
then
	today=`date +%Y%m%d_%H%M%S`
	mv pasgw.log pasgw_${today}.log
fi;

./pasgw
