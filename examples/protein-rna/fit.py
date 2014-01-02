#!/usr/bin/env python
"""Performs unweighted linear regression.  Can be invoked from command line by
providing data pairs through stdin.

Nathan Baker, 2003"""

import math
from sys import stdin, stdout, stderr

""" Accepts list of x,y pairs as input.  Returns dictionary of resuls """
def fit(data):
	outdict = {}

	# Get means
	xmean = 0
	ymean = 0
	ndata = len(data)
	for pair in data:
		x = pair[0]
		y = pair[1]
		xmean = xmean + x
		ymean = ymean + y
	xmean = xmean/float(ndata)
	ymean = ymean/float(ndata)
	outdict["x mean"] = xmean
	outdict["y mean"] = ymean
	
	# Get variances
	sxx = 0
	syy = 0
	sxy = 0
	for pair in data:
		x = pair[0]
		y = pair[1]
		sxx = sxx + (x-xmean)*(x-xmean)
		syy = syy + (y-ymean)*(y-ymean)
		sxy = sxy + (x-xmean)*(y-ymean)
	covxx = sxx/float(ndata)
	covyy = syy/float(ndata)
	covxy = sxy/float(ndata)
	outdict["xx covariance"] = covxx
	outdict["xy covariance"] = covxy
	outdict["yy covariance"] = covyy

	# Slope 
	b = sxy/sxx
	outdict["slope"] = b

	# Intercept
	a = ymean - b*xmean
	outdict["intercept"] = a

	# Correlation coefficient
	r2 = sxy*sxy/sxx/syy
	outdict["correlation coefficient (r^2)"] = r2

	# Residual variance
	s2 = (syy - b*sxy)/(float(ndata)-2)
	s = math.sqrt(s2)
	outdict["residual variance (s^2)"] = s2

	# Slope error
	eb = s/math.sqrt(sxx)
	outdict["slope error"] = eb

	# Intercept error
	ea = s*math.sqrt((1/float(ndata)) + xmean*xmean/sxx)
	outdict["intercept error"] = ea

	return outdict

"""Main driver; reads from stdin"""
def main():
	infile = stdin
	stdout.write("Reading data from %s...\n" % infile.name)

	data = []
	while (1):
		line = infile.readline()
		if line == "":
			break
		line.strip()
		words = line.split()
		try:
			pair1 = float(words[0])
			pair2 = float(words[1])
			data.append((pair1, pair2))
		except Exception, str:
			stderr.write("Ignoring unparseable line:  %s\n" % line)
	stdout.write("Read %d data points.\n" % len(data));
	fitdict = fit(data);
	keys = fitdict.keys()
	keys.sort()
	stdout.write("\nRESULTS:\n")
	for key in keys:
	    stdout.write("%s:  %g\n" % (key, fitdict[key]))



if __name__ == "__main__":
    main()
