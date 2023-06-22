import pydicom
import sys
from PIL import Image
import os

in_path = sys.argv[1]
base_img = os.path.basename(in_path)
try:
    out = sys.argv[2]
except:
    out = base_img.split('.')[0]

ds = pydicom.dcmread(in_path)
img_data = ds.pixel_array
img_data = img_data/10
img_shape = img_data.shape
os.mkdir(out)

im = Image.fromarray(img_data)
im = im.convert('RGB')
im.save(os.path.join(out, str(base_img).rjust(len(str(img_shape[0])),'0')+'.png'))

print('saved image to ' + os.path.join(out, str(base_img).rjust(len(str(img_shape[0])),'0')+'.png'))