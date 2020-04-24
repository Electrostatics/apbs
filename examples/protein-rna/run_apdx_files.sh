#!/bin/bash
# Usage: remove all utility bills pdf file password
shopt -s nullglob

for f in apbs-*.in
	do
		echo "Running apbs on $f"
		N=$(echo $f | grep -oP "[0-9].[0-9]+")
		apbs $f > "apbs-$N.out"
done
for f in dxmath-*.in
	do
		echo "Running dxmath on - $f"
		dxmath $f

done
