import pydicom as dcm
import sys
import csv
import pathlib
#from PIL import Image
from glob import glob
import os


#Open file and create writer


#Set Directory to iterate through
directory = sys.argv[1]
keyList = []
header = ["PATH", "Echo Time", 'Repetition Time' , 'Study Date', 'Study Time']
#files = [f for f in glob('/ISILON/home/kierasht/**', recursive=True) if dcm.dcmread(f)]
#print(files)
#for dirpath, dirnames, filenames in os.walk(directory, topdown=True, followlinks= True):

    #for dicom in filenames:
        #filelist = (dicom.partition(".")[0])
for files in os.path.join(directory):
    if dcm.dcmread(files):
        print("yes")
        #ds = dcm.dcmread(dicom)
        #path = os.path.abspath(os.getcwd())
        #keyInfo = [path, ds['EchoTime'].value, ds['RepetitionTime'].value , ds['StudyDate'].value, ds['StudyTime'].value]
       #keyList.append(keyInfo)

'''
if open("/ISILON/home/kierasht/astKeyData.csv", "w"):
    file = open("/ISILON/home/kierasht/astKeyData.csv", "w")
    writer = csv.writer(file)
    print(keyList)
    writer.writerow(header)
    for row in keyList:
        writer.writerow(row)
    file.close
    '''