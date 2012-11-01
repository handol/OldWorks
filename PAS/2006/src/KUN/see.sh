#!/bin/bash

cat /dev/null > info.log
while true;
do
 date >> info.log
 vmstat >> info.log
 cat sysinfo-current-9090.1020.log >> info.log
 sleep 1
done
