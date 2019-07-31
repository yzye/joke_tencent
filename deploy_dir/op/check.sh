#!/bin/bash

true_value=2

num=`ps aux | grep joke_ccd |wc -l`
if [ $num -lt $true_value ]
then
    ./start_ccd.sh
fi

num=`ps aux | grep joke_mcd |wc -l`
if [ $num -lt $true_value ]
then
    ./start_mcd.sh
fi
