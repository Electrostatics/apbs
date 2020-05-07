#!/bin/python

"""
Create files for the protein-rna tests
"""

MY_LIST = ('0.025', '0.050', '0.075', '0.100', '0.125', '0.150', '0.175',
           '0.200', '0.225', '0.250', '0.275', '0.300', '0.325', '0.400', '0.500',
           '0.600', '0.700', '0.800')

with open("template.txt", "r") as temp:
    TEMPLATE_TEXT = temp.read()

for item in MY_LIST:
    input_txt = TEMPLATE_TEXT.replace("IONSTR", item)
    file_name = "apbs-" + item + ".in"
    print("Creating file now:", file_name)
    with open(file_name, "w") as temp:
        temp.write(input_txt)

with open("dxmath.txt", "r") as temp:
    TEMPLATE_2_TEXT = temp.read()

for item in MY_LIST:
    input_2_txt = TEMPLATE_2_TEXT.replace("IONSTR", item)
    file_2_name = "dxmath-" + item + ".in"
    print("Creating file_2 now: %s" % file_2_name)
    with open(file_2_name, "w") as temp:
        temp.write(input_2_txt)
