#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os, re, glob
from gimpfu import *
from gimpenums import *

def my_script_function(image) :
    image.add_layer(pdb.gimp_layer_new_from_visible(image, image, "diff_with"))
    image.lower_layer(image.active_layer)
    pdb.gimp_floating_sel_anchor(pdb.gimp_edit_paste(image.active_drawable, False))
    pdb.gimp_image_convert_rgb(image)
    pdb.gimp_image_set_active_layer(image, image.layers[0])
    pdb.gimp_layer_set_mode(image.active_layer, DIFFERENCE_MODE)
    pdb.gimp_edit_copy_visible(image)
    pdb.gimp_layer_set_mode(image.active_layer, NORMAL_MODE)
    pdb.gimp_image_convert_indexed(image, NO_DITHER, CUSTOM_PALETTE, 64, FALSE, FALSE, "Pebble Time")
    pdb.gimp_image_set_active_layer(image, image.layers[1])
    pdb.gimp_floating_sel_anchor(pdb.gimp_edit_paste(image.active_drawable, False))
    pdb.gimp_image_select_contiguous_color(image, CHANNEL_OP_REPLACE, image.active_drawable, image.width-1, image.height-1)
    pdb.gimp_image_set_active_layer(image, image.layers[0])
    pdb.gimp_item_set_visible(image.layers[1], False)
    pdb.gimp_layer_add_alpha(image.active_layer)
    pdb.gimp_edit_clear(image.active_drawable)
    pdb.gimp_selection_none(image)
    return

register(
    "python-fu-diff",
    "One bit difference with clean bg",
    "Make difference operation with image in clipboard",
    "Rabit",
    "Rabits",
    "Oct 2015",
    "<Image>/MyScripts/Difference",
    "INDEXED*",
    [
        (PF_IMAGE, "image",       "Input image", None)
    ],
    [],
    my_script_function,
    menu="<Image>/Filters/Python-Fu/Difference"
)

main()
