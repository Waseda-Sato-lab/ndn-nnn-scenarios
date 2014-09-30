#!/bin/bash

WAFDIR=./
WAF=${WAFDIR}waf


for  speed in 5 10 20 30 40 50 60 70 80
do

echo "run $WAF --run "fake -speed=$speed -trace""
$WAF --run "icc-scenario -speed=$speed -trace"

echo "run $WAF --run "fake -speed=$speed -trace -fake""
$WAF --run "icc-scenario -speed=$speed -trace -fake"

done

