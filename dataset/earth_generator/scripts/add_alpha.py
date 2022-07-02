from PIL import Image
from glob import glob
import numpy as np

paths = glob("/home/vrlab/Downloads/Orthogonal/earth/images/0000.png")

for path in paths:
    image = np.array(Image.open(path))
    print(image.shape)
    # alpha = np.ones((image.shape[0], image.shape[1], 1), dtype=np.uint8) * 255
    # image = np.concatenate([image, alpha], axis=-1)

    # Image.fromarray(image).save(path)
    # print("{} finished!".format(path))
