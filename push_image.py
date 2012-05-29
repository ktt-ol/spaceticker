from __future__ import with_statement, division
import serial
import sys
from time import sleep
from PIL import Image, ImageDraw, ImageEnhance


magic_bytes = [159, 170, 85, 241]

display_size = (192, 14)

def pan_image(img):
    x_diff = max(0, img.size[0] - display_size[0])
    y_diff = max(0, img.size[1] - display_size[1])
    max_diff = max(x_diff, y_diff)
    if max_diff < 1:
        yield img
        raise StopIteration

    x_step = x_diff / max_diff
    y_step = y_diff / max_diff
    x = y = 0
    for i in range(max_diff):
        x += x_step
        y += y_step
        yield img.crop(map(int, (x, y, x+display_size[0], y+display_size[1])))
    
def push_image(ser, img):
    ser.write(''.join(chr(x) for x in magic_bytes))
    ser.write(chr(100))
    # sleep(0.1)
    assert len(img.convert('1').tostring())
    for c in img.convert('1').tostring()[:336]:
        ser.write(c)
        # sleep(0.01)
        sleep(1/(115200/9))

def prepare_output(img):
    img = img.convert('L')
    img = Image.eval(img, lambda x: 255 if x > 200 else 0)
    return img.convert('1')

def wait(ser):
    for i in range(5):
        if ser.readline():
            return

# 115200
if __name__ == '__main__':
    ser = serial.Serial(sys.argv[1], baudrate=115200, timeout=1)
    wait(ser)
    img = Image.open(sys.argv[2])
    img.thumbnail((display_size[0], display_size[0]))
    for i, img in enumerate(pan_image(img)):
        push_image(ser, prepare_output(img))
        prepare_output(img).save('/tmp/frames/frame-%03d.gif' % i)
        sleep(1/25)
        # print '.'
        # sleep(0.1)

    sleep(500)
    # print ser.readline()
    # # ser.write(''.join(chr(x) for x in magic_bytes))
    # # ser.write(chr(10))

    # img = Image.new('1', (192, 14), 0)
    # draw = ImageDraw.Draw(img)
    # for y in range(0, 14, 2):
    #     draw.line((0, y, 191, y), fill=1)
    #     for c in img.tostring():
    #         ser.write(c)
    #     sleep(2)
    # sleep(2)
