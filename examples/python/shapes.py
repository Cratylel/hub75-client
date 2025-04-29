import matrix as m
from PIL import Image, ImageDraw
import numpy as np

img = Image.new("RGB", (64, 32), (0, 0, 0))
draw = ImageDraw.Draw(img)

draw.rectangle((16, 16, 16+16, 16+16), fill="red")


np_img = np.array(img)
m.map_buffer("/pixel_mapper_buffer", 64, 32)
m.flush_buffer(np_img)