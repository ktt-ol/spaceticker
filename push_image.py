import serial
import sys
from time import sleep
from PIL import Image, ImageDraw

magic_bytes = [159, 170, 85, 241]


def push_image(img):
    for c in reversed(img.tostring()[:336]):
        ser.write(c)

# 115200
if __name__ == '__main__':
    ser = serial.Serial(sys.argv[1], baudrate=115200)
    sleep(5)
    print ser.readline()
    # ser.write(''.join(chr(x) for x in magic_bytes))
    # ser.write(chr(10))

    img = Image.new('1', (192, 14), 0)
    draw = ImageDraw.Draw(img)
    for y in range(0, 14, 2):
        draw.line((0, y, 191, y), fill=1)
        for c in img.tostring():
            ser.write(c)
        sleep(2)
    sleep(2)

