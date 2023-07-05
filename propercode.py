import pydicom as pyd
import os
import re
import csv

### Function that gets all relevant DICOM tags from dicome file with inputs being:
#str(path to DICOM) and letter(Key letter of the MIF File)

def getInfo(path, letter):
    try:
        keyList = []
        #try:
        dicom = os.path.join(path)
        ds =  pyd.dcmread(dicom)
        keyInfo = [letter, ds['EchoTime'].value, ds['RepetitionTime'].value , ds['Manufacturer'].value, ds['SeriesDescription'].value]
        keyList.append(keyInfo)
        print(keyList)
    except Exception:
        print("ISSUE: Not DICOM, CAN'T OPEN")
    return keyList

header = ['Sequence','Echo Time', 'Repetition Time', 'Manufacturer', 'Series Description']
keyList = []
keyIndex = 5
multiEchoImages= ['Q','W','V','S']
directory = '/ISILON/convert-new'
dir_list = os.listdir(directory)
usefulSTRING = ''
try:
    for directoryIN in dir_list:
        if directoryIN[0] == 'c':
            #print("yes cast")
            castDIR = os.path.join(directory,directoryIN)
            castDIR_List = os.listdir(castDIR)
            #print(castDIR_List)
            for cast in castDIR_List:
                if os.path.isdir(os.path.join(castDIR,cast)):
                    #print('yes directory')
                    castPATH = os.path.join(castDIR, cast)
                    filesCast = os.listdir(castPATH)
                    for mifFILES in filesCast:
                        if mifFILES.upper().endswith('.MIF'):
                            mifFILESPATH = os.path.join(castPATH, mifFILES)
                            with open(mifFILESPATH, encoding= "ISO-8859-1") as input_file:
                                    
                                        head = [next(input_file) for _ in range(150)]
                                        for line in head:
                                            if mifFILES[keyIndex] in multiEchoImages: #Multiple Images
                                                #asts = re.('/asts', line)
                                                    if re.search('/asts/ast', line):
                                                        matchedString = line.strip()
                                                        matchedString = matchedString[:-9]
                                                        matchedString = matchedString[8:]
                                                        if matchedString[0:5] != '/asts':
                                                            splitMatchedString = matchedString.split('/asts')
                                                            newMatchedString = "/asts" + splitMatchedString[1].split(' ')[0]
                                                            matchedString = newMatchedString
                                                        print(matchedString)
                                                        #matchPATH = str(match.group()).remove
                                                        #prefix('<string>')
                                                        #matchPATH = str(match.group())[8:]
                                                        #print(matchPATH + "))))")
                                                        #matchPATH = matchPATH.removesuffix('</string>')
                                                        #matchPATH = matchPATH[:-9]
                                                        #print(matchPATH + "OOOOOO")
                                                        keyLetter= mifFILES[keyIndex]
                                                        keyList.append(getInfo(matchedString, keyLetter))
                                            else:
                                                #if not usefulSTRING:
                                                if re.search('/asts/ast', line):
                                                    print("YES")
                                                    #astLine = re.search('/asts', line)
                                                    #print(line)
                                                    usefulSTRING = line.strip()
                                                    usefulSTRING = usefulSTRING[:-9]
                                                    usefulSTRING = usefulSTRING[8:]
                                                    if usefulSTRING[0:5] != '/asts':
                                                        splitUsefulString = usefulSTRING.split('/asts')
                                                        newMatchedString = "/asts" + splitUsefulString[1].split(' ')[0]
                                                        usefulSTRING = newMatchedString
                                                    print(usefulSTRING)
                                                    #astLine = str(astLine.group()).removeprefix('<string>')
                                                    #astMATCH = astLine.group(0)
                                                    #print(astMATCH)
                                                    #print(astLine.group(1))
                                                    
                                                    #print(astLine + "PPPP")
                                                    #astLine = astLine[:-9]
                                                    #print(astLine + "XXXXX")
                                                    keyLetter = mifFILES[keyIndex]
                                                    if not getInfo(usefulSTRING,keyLetter):
                                                        keyList.append(getInfo(usefulSTRING, keyLetter))
                                                #else: 
                                                    #break   
except StopIteration:               
    print("Stop iteration")

try:
    csvFile = open("/ISILON/home/kierasht/machineLearningDATA.csv", "w")
    writer = csv.writer(csvFile, delimiter=';')
    writer.writerow(header)
    print(keyList)
    for row in keyList:
        writer.writerow(row)
        csvFile.close
except IOError:
    print("ERROR")
