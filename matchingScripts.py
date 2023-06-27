import pydicom as dcm
import sys
import csv
import pathlib
#from PIL import Image
#import pandas as pd
from glob import glob
import os
from collections import defaultdict
mif = []
#directory = "convert-new/ISILON/home/kierasht"
fileSSS = sys.argv[1]
matchingMIFtoDCM = []
astValues = defaultdict(list)
'''
for path, directories, allFiles in os.walk(directory, topdown=True, followlinks= True):
        for file in allFiles:a
            fileJoin = os.path.join(path, file)
            if file.lower().endswith(".sss"):
                df = pd.read_fwf(fileJoin,usecols=[0,5,6], header=None, index_col=False)
        
'''
with open(fileSSS) as inFile:
    sentence = []
    importanINFO = []
    SSSKeyInfo = []
    for line in inFile:
        sssColumns = line.split('\\s+')
        sentence.append(sssColumns)
        #for i in sssColumns:
            #print(i.split()[])
            #for num,string in enumerate(splitSentence):
                #if num == 0 and string == 'Summary' or string == 'Scan':
                #print(splitSentence[3])
    keyinfoList = sentence[6:-1]
    for stringList in keyinfoList:
        for keyString in stringList:
            #print(keyString)
            keyStringInfo = keyString.split()
            #print(keyStringInfo)
            if keyStringInfo[0] == '<':
                SSSKeyInfo = [keyStringInfo[1], keyStringInfo[6], keyStringInfo[7]]
                #print(SSSKeyInfo)
                keyLetter = SSSKeyInfo[0]
            else:
                SSSKeyInfo = [keyStringInfo[0], keyStringInfo[4], keyStringInfo[5]]
                #print(SSSKeyInfo)
                keyLetter = str(keyStringInfo[0])[5]
            csvFile = open("/ISILON/home/kierasht/astKeyData.csv", newline='')
            data = csv.reader(csvFile, delimiter=',')
            next(data)
            #included_cols = [3, 4]
            for row in data:
                for (i,v) in enumerate(row):
                    astValues[i].append(v)
                    for index in range(len(astValues[4])):
                        if SSSKeyInfo[1] == astValues[3][index] and SSSKeyInfo[2] == astValues[4][index]:
                            mif.append([keyLetter,'c' + astValues[1][index]])
                            print(mif)
                        else:
                            print("nothing matched")
