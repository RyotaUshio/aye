import numpy as np
import matplotlib.pyplot as plt

def imshow(filename):
    image = np.loadtxt(filename)
    plt.imshow(image, cmap='gray')
