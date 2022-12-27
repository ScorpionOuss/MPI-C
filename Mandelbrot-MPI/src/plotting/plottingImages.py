import os, sys
from PIL import Image
from readTxt import readData

path = "color.txt"
width = 2048
height = 2048



def save_image(image, path):
    "Save the generated image in the specified path"
    image.save(path, 'png')



def createImage():
    """ Create a new image """
    image = Image.new("RGB", (width, height), "white")
    print(type(image))
    return image

def plotFigure():
    """ Load the ascii file in data and construct
    the image pixel by pixel"""
    image = createImage()
    pixels = image.load()
    data = readData(path)
    for j in range(height):
        for i in range(width):
            pixels[i, j] = (int(data[j][i]), int(data[j][i]), int(data[j][i]))
    return image

def main():
    image = plotFigure()
    save_image(image, 'Graphic2.png')

main()
