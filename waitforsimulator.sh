#!/bin/bash

count=`xcrun simctl list | grep Booted | wc -l | sed -e 's/ //g'`
while [ $count -lt 1 ]
do
    sleep 1
    count=`xcrun simctl list | grep Booted | wc -l | sed -e 's/ //g'`
done
