#import pydicom as py
import sys
#from PIL import Image
import os
'''
keyIndex = 5
sequenceFile = sys.argv[1]
seq = str(sequenceFile)[keyIndex]
print(seq)
'''
keyInfo = [[10,20,30], [10, 0 ,20]]
import csv


file = open("astKeyData.csv", 'w')
writer = csv.writer(file)
for row in keyInfo:
    writer.writerow(row)
file.close

'''
try:
    out = sys.argv[2]
except:
    out = chosenFile.split('.')[0]

ds = py.dcmread(sequenceFile)
ds.remove_private_tags
##
'''

