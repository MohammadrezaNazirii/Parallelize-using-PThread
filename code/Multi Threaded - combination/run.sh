#!/bin/bash


gcc -pg -o images_processing main.c utilities.c

./images_processing

gprof  images_processing gmon.out> report.txt

cat report.txt
