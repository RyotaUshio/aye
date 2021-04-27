#!/usr/bin/env python3

import re
import sys
import argparse
from PIL import Image

import numpy as np
import matplotlib.pyplot as plt


def suffix(filename, suffix):
    if not suffix.startswith('.'):
        suffix = '.' + suffix
    return re.sub(r'(.*)\.(.*)?', r'\1' + suffix, filename)

def upper_bound_to_scale(input_size:tuple, upper_bound:int):
    longer_edge = max(input_size)
    scale = upper_bound / longer_edge
    return scale

def rescale(image, scale):
    return image.resize((np.asarray(image.size) * scale).astype(int))

def txt_to_array(filename):
    with open(filename) as f:
        shape = np.fromstring(f.readline(), dtype=int, sep=' ')
        array = np.loadtxt(f).reshape(shape)
    return array

def image_to_txt(input_fname, output_fname=None, size=None, scale=None, upper_bound=None):
    with Image.open(input_fname) as image:
        if upper_bound:
            scale = upper_bound_to_scale(image.size, upper_bound)
            image = rescale(image, scale)
        elif size:
            image = image.resize(size)
        elif scale:
            image = rescale(image, scale)
            
        array = np.asarray(image.convert('L'))
        if output_fname is None:
            output_fname = suffix(input_fname, '.txt')
        np.savetxt(output_fname, array, fmt='%d', newline=' ')

        with open(output_fname, 'r+') as f:
            content = f.read()
            f.seek(0)
            f.write(f"{image.size[0]} {image.size[1]}\n" + content)

def txt_to_image(input_fname, output_fname=None):
    if output_fname is None:
        output_fname = suffix(input_fname, '.jpg')
    image = Image.fromarray(txt_to_array(input_fname)).convert('RGB')
    image.save(output_fname)
    

def imshow(filename):
    image = txt_to_array(filename)
    plt.imshow(image, cmap='gray')
    plt.gca().xaxis.set_visible(False)
    plt.gca().yaxis.set_visible(False)
    plt.plot()

        
if __name__ == '__main__':

    parser = argparse.ArgumentParser(description='Image I/O')
    parser.add_argument('subcommand')
    parser.add_argument('input_filename')
    parser.add_argument('-o', dest='output_filename')
    parser.add_argument('--size', type=int, nargs=2)
    parser.add_argument('--scale', type=float)
    parser.add_argument('--upper-bound', type=int)
    args = parser.parse_args()
    
    if args.subcommand == 'to_txt':
        image_to_txt(args.input_filename, args.output_filename, size=args.size, scale=args.scale, upper_bound=args.upper_bound)
    elif args.subcommand == 'to_image':
        txt_to_image(args.input_filename, args.output_filename)
    elif args.subcommand in ['show', 'imshow']:
        imshow(args.input_filename)
