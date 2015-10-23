#!/bin/sh -ex

export PATH="$HOME/local/pebble/arm-cs-tools/bin:$HOME/local/pebble/PebbleSDK-3.6/bin:$PATH"
BASEDIR=$(dirname `readlink -f $0`)

# Requirements
which blender # > 2.74
which gimp # >= 2.8
which pebble # >= 3.6 SDK
which arm-none-eabi-gcc # Pebble arm cs tools

cd "${BASEDIR}"

# Rendering & processing images will be started only if render dir is not exists or older then flux_capacitor.blend file
if [ ! -d render -o flux_capacitor.blend -nt render ]
then
    rm -rf render watchface/resources/images-gen
    mkdir -p render watchface/resources/images-gen

    blender -b flux_capacitor.blend -o "//render/img_###.png" -F PNG -a

    mv render/img_031.png watchface/resources/images-gen/background.png
    gimp -i --batch-interpreter python-fu-eval -b 'import sys, os; sys.path.append(os.getcwd()); execfile("gimp_batch.py")' -b 'pdb.gimp_quit(0)' render/* watchface/resources/images-gen/background.png
    mv render/img_*.png watchface/resources/images-gen
fi

# Build watchface, output in watchface/build/watchface.pbw
cd watchface
pebble clean
pebble build
