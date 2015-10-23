#!/usr/bin/env python
# -*- coding: utf-8 -*-
from gimpfu import *
from gimpenums import *
from Pebble_Time import colors as pebble_time

def createPalette(name, colors_dict):
    name = pdb.gimp_palette_new(name)
    for color_name in colors_dict:
        pdb.gimp_palette_add_entry(name, color_name, colors_dict[color_name])

    return name

def doProcess(image, bg_image, palette):
    image.add_layer(pdb.gimp_layer_new_from_visible(bg_image, image, "diff_with"))
    image.lower_layer(image.active_layer)
    pdb.gimp_image_convert_indexed(image, NO_DITHER, CUSTOM_PALETTE, 64, FALSE, FALSE, palette)
    pdb.gimp_image_convert_rgb(image)
    pdb.gimp_image_set_active_layer(image, image.layers[0])
    pdb.gimp_layer_set_mode(image.active_layer, DIFFERENCE_MODE)
    pdb.gimp_edit_copy_visible(image)
    pdb.gimp_layer_set_mode(image.active_layer, NORMAL_MODE)
    pdb.gimp_image_convert_indexed(image, NO_DITHER, CUSTOM_PALETTE, 64, FALSE, FALSE, palette)
    pdb.gimp_image_set_active_layer(image, image.layers[1])
    pdb.gimp_floating_sel_anchor(pdb.gimp_edit_paste(image.active_drawable, False))
    pdb.gimp_image_select_contiguous_color(image, CHANNEL_OP_REPLACE, image.active_drawable, image.width-1, image.height-1)
    pdb.gimp_image_set_active_layer(image, image.layers[0])
    pdb.gimp_item_set_visible(image.layers[1], False)
    pdb.gimp_layer_add_alpha(image.active_layer)
    pdb.gimp_edit_clear(image.active_drawable)
    pdb.gimp_selection_none(image)
    return

palette_name = createPalette("batch_PebbleTime", pebble_time)

images = gimp.image_list()
background = images.pop(0)

for image in images:
    doProcess(image, background, palette_name)
    pdb.gimp_file_save(image, image.active_drawable, image.filename, image.filename)

pdb.gimp_palette_delete(palette_name)
