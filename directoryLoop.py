import pydicom as dcm
import sys
import csv
import pathlib
#from PIL import Image
from glob import glob
import os


#Open file and create writer


#Set Directory to iterate through
directory = (sys.argv[1])

keyList = []
header = ["PATH", "AST #", "Directory","Echo Time", 'Repetition Time' , 'Study Date', 'Study Time']
#files = [f for f in glob('/ISILON/home/kierasht/**', recursive=True) if dcm.dcmread(f)]
#print(files)

for path, directories, allFiles in os.walk(directory, topdown=True, followlinks= True):
        for file in allFiles:
            dicoms = os.path.join(path, file)
            try:
                ds = dcm.dcmread(dicoms)
                specificPATH = os.path.abspath(ds)
                if 'EchoTime' in ds and 'RepetitionTime' in ds and 'StudyDate' in ds and 'StudyTime' in ds:
                    keyInfo = [specificPATH, specificPATH.split('/')[5], specificPATH.split('/')[6], ds['EchoTime'].value, ds['RepetitionTime'].value , ds['StudyDate'].value, ds['StudyTime'].value]
                    keyList.append(keyInfo)
                else:
                    print("Not enough INFO")
            except Exception:
                print("Not a DICOM")


if open("/ISILON/home/kierasht/astKeyData.csv", "w"):
    file = open("/ISILON/home/kierasht/astKeyData.csv", "w")
    writer = csv.writer(file)
    writer.writerow(header)
    for row in keyList:
        writer.writerow(row)
    file.close
