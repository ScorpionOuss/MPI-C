import os, sys

rootPath = os.path.abspath('.')
def readData(path):
    """ Routine that loads the indexes from the ascii file"""
    fp = open(rootPath + "\out\\" + path, 'r')
    data = []
    for line in fp:
        dataLine = line.split()
        data.append(dataLine)
    return data

print(readData("color.txt"))