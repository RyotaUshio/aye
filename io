#!/usr/bin/env python3

import re
import sys
import argparse
from PIL import Image

import numpy as np
import matplotlib.pyplot as plt

def make_iterable(obj):
    if not isinstance(obj, str) and hasattr(obj, '__iter__'):
        return obj
    return [obj]

def suffix(filename, suffix):
    if not suffix.startswith('.'):
        suffix = '.' + suffix
    return re.sub(r'(.*)\.(.*)?', r'\1' + suffix, filename)

def make_fnames(input_fnames, output_fnames=None):
    input_fnames = make_iterable(input_fnames)
    output_fnames = make_iterable(output_fnames)
    len_i = len(input_fnames)
    len_o = len(output_fnames)
    if len_i < len_o:
        raise Exception('input filenames insufficient')
    if len_i > len_o:
        output_fnames.extend([None] * (len_i - len_o))
    return input_fnames, output_fnames

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

def imread(filename):
    if any([filename.endswith(suffix) for suffix in ['.bmp', '.jpg', '.JPG', '.jpeg', '.JPEG', '.png', '.PNG']]):
        return np.asarray(Image.open(filename))
    return txt_to_array(filename)

def image_to_txt(input_fname, output_fname=None, size=None, scale=None, upper_bound=None):
    if len(input_fname) > 1 or len(output_fname) > 1:
        raise Exception('image_to_txt: Too many arguments')
    input_fname = input_fname[0]
    output_fname = output_fname[0]
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

def _txt_to_image_impl(input_fname, output_fname):
    if output_fname is None:
        output_fname = suffix(input_fname, '.bmp')
    image = Image.fromarray(txt_to_array(input_fname)).convert('RGB')
    image.save(output_fname)

def txt_to_image(input_fnames, output_fnames=None):
    input_fnames, output_fnames = make_fnames(input_fnames, output_fnames)
    for input_fname, output_fname in zip(input_fnames, output_fnames):
        _txt_to_image_impl(input_fname, output_fname)

def imshow(filenames, show=True):
    filenames = make_iterable(filenames)
    num = 1 # id of figures
    for filename in filenames:
        image = imread(filename)
        fig = plt.figure(num)
        ax = fig.add_subplot(111)
        ax.imshow(image, cmap='gray')
        ax.xaxis.set_visible(False)
        ax.yaxis.set_visible(False)
        ax.set_title(filename)
        num += 1
    if show:
        plt.show()
    return fig, ax

def DoG_show(input_fname, extrema_fname, show=True):
    coords = np.loadtxt(extrema_fname)
    fig, ax = imshow(input_fname, False)
    for coord in coords:
        ax.add_patch(plt.Circle((coord[2], coord[1]), coord[0], clip_on=False, fill=False, color='r'))
    if show:
        plt.show()

# def read_sift(input_fname):    
#     with open(input_fname) as f:
#         for line in f:
#             line.split()

        
if __name__ == '__main__':

    parser = argparse.ArgumentParser(description='Image I/O')
    parser.add_argument('subcommand')
    parser.add_argument('input_filename', nargs='+')
    parser.add_argument('-o', dest='output_filename', nargs='+')
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
    elif args.subcommand == 'DoG_show':
        DoG_show(*args.input_filename)
