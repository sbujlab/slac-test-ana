#!/bin/bash
echo "Enter the run number"
files=`analyzer -b -q ana.C`
echo "$files"
