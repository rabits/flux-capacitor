#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os, re, glob
from gimpfu import *
from gimpenums import *

def my_script_function(image) :
    pdb.gimp_image_scale(image, 144, 168)
    pdb.gimp_image_convert_indexed(image, NO_DITHER, CUSTOM_PALETTE, 64, FALSE, FALSE, "Pebble Time")
    return

register(
    "python-fu-topebble",
    "Indexing and scale image",
    "A simple Python Script to convert image to pebble time colors & size.",
    "Rabit",
    "Rabits",
    "Oct 2015",
    "<Image>/MyScripts/To Pebble",
    "RGB*, GRAY*",
    [
        (PF_IMAGE, "image",       "Input image", None)
    ],
    [],
    my_script_function,
    menu="<Image>/Filters/Python-Fu/To Pebble"
)

main()
