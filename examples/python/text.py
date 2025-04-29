import sys
import os

# Add the path to the directory containing the matrix .so file
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../../')))

import matrix as m
from PIL import Image, ImageDraw
import numpy as np

img = Image.new("RGB", (64, 32), (0, 0, 0))
draw = ImageDraw.Draw(img)

draw.text((2, 2), "Hello", fill="white")

np_img = np.array(img)
m.map_buffer("/pixel_mapper_buffer", 64, 32)
m.flush_buffer(np_img)