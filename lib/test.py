import my_module
from PIL import Image, ImageDraw
import numpy as np

img = Image.new("RGB", (64, 32), (0, 0, 0))
draw = ImageDraw.Draw(img)

draw.text((2, 2), "WOOF ;3", fill="white")
draw.rectangle((16, 16, 16+16, 16+16), fill="red")

np_img = np.array(img)
my_module.map_buffer("/pixel_mapper_buffer", 64, 32)
my_module.flush_buffer(np_img)