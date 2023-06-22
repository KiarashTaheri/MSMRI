#import pydicom as py
import sys
#from PIL import Image
import os

keyIndex = 5
sequenceFile = sys.argv[1]
seq = str(sequenceFile)[keyIndex]
#print(seq)
chosenFile = os.path.basename(sequenceFile)

'''
try:
    out = sys.argv[2]
except:
    out = chosenFile.split('.')[0]

ds = py.dcmread(sequenceFile)
ds.remove_private_tags
##
'''

