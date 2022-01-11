#!/usr/bin/env python
import os

def getsearchstring():
	searchstring = str(raw_input("Enter search string: "))
	if len(searchstring) == 0:
		print("Search string must have at least one character.")
		getsearchstring()
	return searchstring

search = getsearchstring()
casesen = raw_input("Case sensitive? (y/n): ").lower()

if casesen == "y":
	print("Case sensitive search is on.\n")
	casesen = True
else:
	print("Case sensitive search is off.\n")
	casesen = False
	search = search.lower()

lsdir = os.listdir('.')
flist = []
match = 0

for i in xrange(len(lsdir)):
	if os.path.isfile(lsdir[i]) and lsdir[i] != '500.txt':
		flist.append(lsdir[i])

flist.sort()

print("Files found: " + str(len(flist)) + "\n")

def lineread(_file):
	readstr = ""
	readbyte = " "
	while ord(readbyte) != 10:
		readstr = readstr + readbyte
		readbyte = _file.read(1)
		if len(readbyte) == 0:
			break
		if ord(readbyte) < 32:
			if ord(readbyte) != 9 and ord(readbyte) != 10 and ord(readbyte) != 13:
				return "BiNarY FiLe DeTeCtEd!"
	return readstr

for i in xrange(len(flist)):
	size = os.path.getsize(flist[i])
	searchrange = (size + 1) - len(search)
	if (size >= len(search)):
		print("Searching " + flist[i])
		f = open(flist[i], 'rb')
		linepos = 0
		while f.tell() < size - 1:
			linepos += 1
			fr = lineread(f)
			if fr == "BiNarY FiLe DeTeCtEd!":
				print("\tskipping binary file.")
				break
			if len(fr) > 0:
				if casesen:
					matchstr = fr
				else:
					matchstr = fr.lower()
				for i in xrange(len(fr)):
					if matchstr[i:i+len(search)] == search:
						print("\tFound match at line " + str(linepos) + " pos " + str(i))
						print("\t\t" + fr[i:])
						match += 1
		f.close()

print("\nTotal matches found: " + str(match))