#!/bin/bash


gcc -pg -o images_processing_ main.c utilities.c

./images_processing_

gprof  images_processing_ gmon.out> report.txt

cat report.txt
