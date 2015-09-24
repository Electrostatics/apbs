#!/bin/bash
# Usage: remove all utility bills pdf file password 
shopt -s nullglob

for f in apbs-*.in
	do
		echo "Running apbs on $f" 
		apbs $f	
done		
for f in dxmath-*.in
	do
		echo "Running dxmath on - $f"   
		dxmath $f		

done
