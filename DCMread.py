import pydicom
import sys

ds = pydicom.dcmread(sys.argv[1])

ds.remove_private_tags()
print(str(ds))

#ds.save_as('test/test_out')
