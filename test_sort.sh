#!/bin/bash
for i in ./traces/traces_perf_sort/*.cmd
do
    perf stat --repeat 5 -o "${i%.cmd}"_report -e cache-misses,branches,instructions,context-switches ./qtest -v 0 -f "$i"
done
