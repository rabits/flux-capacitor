#!/usr/bin/env python
# -*- coding: utf-8 -*-
from os import path
from gimpfu import *
from gimpenums import *
from Pebble_Time import colors as pebble_time

def createPalette(name, colors_dict):
    name = pdb.gimp_palette_new(name)
    for color_name in colors_dict:
        pdb.gimp_palette_add_entry(name, color_name, colors_dict[color_name])

    return name

def rewriteImage(image):
    pdb.gimp_file_save(image, image.active_drawable, image.filename, image.filename)

def doCrop(image, x, y, w, h):
    pdb.gimp_image_crop(image, w, h, x, y)

def doPebbleIndexed(image):
    if pdb.gimp_drawable_is_indexed(image.active_drawable) == FALSE:
        pdb.gimp_image_convert_indexed(image, NO_DITHER, CUSTOM_PALETTE, 64, FALSE, TRUE, palette_name)

def doDiff(image, bg_image):
    image.add_layer(pdb.gimp_layer_new_from_visible(bg_image, image, "diff_with"))
    image.lower_layer(image.active_layer)
    pdb.gimp_image_convert_rgb(image)
    pdb.gimp_image_set_active_layer(image, image.layers[0])
    pdb.gimp_layer_set_mode(image.active_layer, DIFFERENCE_MODE)
    pdb.gimp_edit_copy_visible(image)
    pdb.gimp_layer_set_mode(image.active_layer, NORMAL_MODE)
    image.active_layer.add_mask(image.active_layer.create_mask(ADD_BLACK_MASK))
    pdb.gimp_layer_set_edit_mask(image.active_layer, TRUE)
    pdb.gimp_floating_sel_anchor(pdb.gimp_edit_paste(image.active_drawable, False))
    pdb.gimp_curves_spline(image.active_drawable, HISTOGRAM_VALUE, 6, [0,0, 1,255, 255,255])
    pdb.gimp_image_remove_layer_mask(image, image.active_layer, MASK_APPLY)

    image.remove_layer(image.layers[1])
    doPebbleIndexed(image)
    pdb.gimp_selection_none(image)
    return

palette_name = createPalette("batch_PebbleTime", pebble_time)

# Diff & Crop imgs up to 46
diff_images = crop_images = ["img_%03d.png" % i for i in range(46+1)]

images = gimp.image_list()
background = images.pop(0)

doPebbleIndexed(background)
rewriteImage(background)

for image in images:
    doPebbleIndexed(image)
    if path.basename(image.filename) in diff_images:
        doDiff(image, background)
    if path.basename(image.filename) in crop_images:
        doCrop(image, 26, 10, 92, 112)
    rewriteImage(image)

pdb.gimp_palette_delete(palette_name)
