from subprocess import call

my_list = (0.025, 0.05, 0.075, 0.1, 0.125, 0.15, 0.175, 0.225, 0.275,
0.325, 0.4, 0.5, 0.6, 0.7, 0.8)
with open("template.txt", "r") as temp:  
	template_text = temp.read()

for item in my_list:  
	input_txt = template_text.replace("IONSTR",str(item))  
	file_name = "apbs-" + str(item) + ".in"  
	print "Creating file now:", file_name  
	with open(file_name, "w") as temp:  
		temp.write(input_txt)
	call(["apbs", "apbs-" + str(item) + ".in"])

with open("dxmath.txt", "r") as temp:
	template_2_text = temp.read()

for item in my_list:
	input_2_txt = template_2_text.replace("IONSTR",str(item))
	file_2_name = "dxmath-" + str(item) + ".in"
	print "Creating file_2 now:", file_2_name
	with open(file_2_name, "w") as temp:
		temp.write(input_2_txt)
	call(["dxmath", "dxmath-" + str(item) + ".in"])
