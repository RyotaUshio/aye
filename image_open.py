import re
import sys
import numpy as np
from PIL import Image

def txtname(filename):
    return re.sub(r'(.*)\.(.*)?', r'\1.txt', filename)

def image_to_txt(filename):
    with Image.open(filename) as image:
        array = np.asarray(image.convert('L'))
        np.savetxt(txtname(filename), array, delimiter=',', newline=',', fmt='%d',
                   header='', comments=''
        )

def txt_to_image(input_fname, output_fname):
    array = np.loadtxt(txtname(input_fname))
    image = Image.fromarray(array).convert('RGB')
    image.save(output_fname)

