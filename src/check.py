import os  
import os.path  

# Check tool, python3+
# check newline at the end of file
for parent,dirnames,filenames in os.walk(".\\"):  
	for rawfilename in filenames:
		suffix = os.path.splitext(rawfilename)[1][1:]
		if suffix != "cpp" and suffix != "h" and suffix != "hpp" and suffix != "txt" and suffix != "md": continue
		filename = os.path.join(parent, rawfilename)
		file = open(filename, 'r', encoding = "utf-8")
		filecontent = file.read()
		if filecontent[-1] != "\n":
			print("No newline at the end of file: " + filename)
		file.close()

print("Finished!")
