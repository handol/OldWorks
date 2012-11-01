#!/bin/bash

while true;
do
 pmap -x $1 | egrep "heap|total"
 sleep 1
done
