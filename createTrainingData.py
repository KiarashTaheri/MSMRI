import pydicom as pyd
import os
import re
import csv
import sys
import pandas as pd
from itertools import zip_longest

### Function that gets all relevant DICOM tags from dicome file with inputs being:
#str(path to DICOM) and letter(Key letter of the MIF File)
header_csv = [
    'Sequence letter', 'Sequence number','Echo Time', 'Repetition Time', 'Manufacturer', 'Series Description', 'Series Description', 'Scanning sequence', 
    'Sequence Name', 'Flip Angle', 'MR Acquisition Type', 'Sequence Variant', 'Scan Options', 
    'Magnetic Field Strength','Slice Thickness'
    ]
#'Angio Flag'
#'Inversion Time'
#'Contrast/Bolus agent'
# 'Contrast/Bolus agent'
keyList = []
keyIndex = 5
multiEchoImages= ['Q','W','V','S']
directory = '/ISILON/convert-new'
dir_list = os.listdir(directory)
usefulSTRING = ''

#ds['InversionTime'].value, 
#ds['ContrastBolusAgent'].value
def getInfo(path, letter):
    stringtoNum = 64
    #try:
    keyList = []
    try:
        dicom = os.path.join(path)
        ds =  pyd.dcmread(dicom)
        keyInfo = [letter, ord(letter)-stringtoNum, ds['EchoTime'].value, ds['RepetitionTime'].value , ds['Manufacturer'].value, ds['SeriesDescription'].value, 
                    ds['ScanningSequence'].value, ds['SequenceName'].value , ds['FlipAngle'].value, ds['MRAcquisitionType'].value, ds['SequenceVariant'].value, 
                    ds['MagneticFieldStrength'].value, ds['SliceThickness'].value, 
                    ]
        keyInfo = [str(s).replace(',', '') for s in keyInfo]
        keyList.append(keyInfo)
        #print(keyList)
    except KeyError:
        pass
    return keyList

#ds['AngioFlag'].value 
def MultiEchoFetch(file):
    head = [next(file) for _ in range(60, 150)]
    for line in head:
        if re.search('/asts/ast', line):
            matchedString = line.strip()
            matchedString = matchedString[:-9]
            matchedString = matchedString[8:]
            keyLetter= mifFILES[keyIndex]
            print(matchedString)
            if matchedString[0:5] == '/asts':
                if pyd.dcmread(matchedString):
                    keyList.append(getInfo(matchedString, keyLetter))
                else: 
                    print(matchedString)
            else:
                pass
                #print(matchedString)
                #splitMatchedString = matchedString.split('/asts')
                #newMatchedString = "/asts" + splitMatchedString[1]
                #print(newMatchedString)
                #print(newMatchedString)
                #matchedString = newMatchedString
                #print(matchedString)
                
                #keyList.append(getInfo(matchedString, keyLetter))
                #print(matchedString)
                #print(getInfo(matchedString, keyLetter))

def NOTMultiEchoFetch(file):
    head = [next(file) for _ in range(0,71)]
    for line in head:
        if re.search('/asts/ast', line):
            #print(line)
            matchedString = line.strip()
            matchedString = matchedString[:-9]
            matchedString = matchedString[8:]
            keyLetter= mifFILES[keyIndex]
            print(matchedString)
            if matchedString[0:5] == '/asts':
                if pyd.dcmread(matchedString):
                    keyList.append(getInfo(matchedString, keyLetter))
                    print("YES")
                else:
                    print("Couldn't open")
                    print(matchedString)
            else:
                print("not start with asts")
                print(matchedString)
                splitMatchedString = matchedString.split('/asts')
                newMatchedString = "/asts" + splitMatchedString[1]
                print(newMatchedString)
                matchedString = newMatchedString
                #print(matchedString)
                
                #print(matchedString)
                #print(getInfo(matchedString, keyLetter))   
                #keyList.append(getInfo(matchedString, keyLetter))
                #print(getInfo(matchedString, keyLetter))

try:
    for directoryIN in dir_list:
        if directoryIN[0] == 'c' and directoryIN[4] == '4':
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
                            #print(mifFILES[keyIndex])
                            with open(mifFILESPATH, encoding= "ISO-8859-1") as input_file:
                                keyLetter = mifFILES[keyIndex]
                                #print(input_file)
                                if keyLetter in multiEchoImages:
                                    MultiEchoFetch(input_file)
                                else:
                                    print("not multi")
                                    NOTMultiEchoFetch(input_file)
except StopIteration:               
    #print("Stop iteration")
    pass

try:
    csvFile = open("/ISILON/home/kierasht/NewData.csv", "w")
    writer = csv.writer(csvFile)
    writer.writerow(header_csv)
    #print(keyList)
    for row in keyList:
        if row:
            writer.writerow(row)
        else:
           writer.writerow(row) 
    csvFile.close
except IOError:
    pass

#print(keyList)
dataFrame = pd.DataFrame(keyList)
dataFrame.to_csv('MLdataNew.csv', header= header_csv)
