#!/bin/bash

mkdir IDR_Jan
for i in m0m.01*log ;
do
	echo $i
	python stat2idr.py $i
	mv k_n_pasidr* IDR_Jan
done

tar cvf IDR_Jan.tar IDR_Jan
compress IDR_Jan.tar.Z

mkdir IDR_Feb
for i in m0m.02*log ;
do
	echo $i
	python stat2idr.py $i
	mv k_n_pasidr* IDR_Feb
done

tar cvf IDR_Feb.tar IDR_Feb
compress IDR_Feb.tar.Z
