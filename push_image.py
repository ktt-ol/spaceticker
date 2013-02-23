from __future__ import with_statement, division
try:
    from cv2 import cv
except ImportError:
    import cv

import serial
import sys
from time import sleep
from PIL import Image, ImageDraw, ImageEnhance, ImageSequence
from itertools import cycle, chain

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
        sleep(1/(250000/9))

def prepare_output(img):
    img = img.convert('L')
    img = Image.eval(img, lambda x: 255 if x > 100 else 0)
    return img.convert('1')

def wait(ser):
    for i in range(5):
        if ser.readline():
            return

def adaptive_threshold(pil_img):
    pil_img = pil_img.convert('L')
    cv_img = cv.CreateImageHeader(pil_img.size, cv.IPL_DEPTH_8U, 1)
    cv.SetData(cv_img, pil_img.tostring(), pil_img.size[0])
    # grayscale = cv.CreateImage((pil_img.size[0], pil_img.size[1]), 8, 1)
    # cv.CvtColor(cv_img, grayscale, cv.CV_RGB2GRAY)
    # cv.Threshold(cv_img, cv_img, 128, 255, cv.CV_THRESH_OTSU)
    cv.AdaptiveThreshold(cv_img, cv_img, 255, cv.CV_ADAPTIVE_THRESH_MEAN_C, cv.CV_THRESH_BINARY, 75, 5)
    return Image.frombuffer("L", (cv_img.width, cv_img.height),
        cv_img.tostring(), 'raw', "L", 0, 1)

def gif_frames(img):
    pal = img.getpalette()
    prev = img.convert('RGBA')
    prev_dispose = True
    for i, frame in enumerate(ImageSequence.Iterator(img)):
        dispose = frame.dispose

        if frame.tile:
            x0, y0, x1, y1 = frame.tile[0][1]
            if not frame.palette.dirty:
                frame.putpalette(pal)
            frame = frame.crop((x0, y0, x1, y1))
            bbox = (x0, y0, x1, y1)
        else:
            bbox = None

        if dispose is None:
            prev.paste(frame, bbox, frame.convert('RGBA'))
            yield prev
            prev_dispose = False
        else:
            if prev_dispose:
                prev = Image.new('RGBA', img.size, (0, 0, 0, 0))
            out = prev.copy()
            out.paste(frame, bbox, frame.convert('RGBA'))
            yield out

def prepare_frame(frame):
    frame.thumbnail((display_size[0], display_size[0]))
    y_offset = 35

    frame = frame.crop((0, y_offset, display_size[0], y_offset+display_size[1]))

    frame = adaptive_threshold(frame)
    return frame.convert('L')

# 115200
if __name__ == '__main__':
    ser = serial.Serial(sys.argv[1], baudrate=250000, timeout=1)
    wait(ser)
    img = Image.open(sys.argv[2])
    # img = adaptive_threshold(img)
    # img.show()
    frames = []

    for i, frame in enumerate(gif_frames(img)):
        frame = prepare_frame(frame)
        frame.convert('1').save('/tmp/frames/frame-%03d.gif' % i)
        frames.append(img.convert('1'))

    # for i, img in enumerate(anim_image(img)):
    #     frames.append(img.convert('1'))
    #     img.convert('1').save('/tmp/frames/frame-%03d.gif' % i)


    for frame in cycle(chain(frames, frames[::-1])):
        push_image(ser, frame)
        sleep(1/20)

    # for i, img in enumerate(pan_image(img)):
    #     push_image(ser, prepare_output(img))
    #     img.convert('1').save('/tmp/frames/frame-%03d.gif' % i)
    #     # sleep(1/50)
    #     # print '.'
    #     # sleep(0.1)

    sleep(1)
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
