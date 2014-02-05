import time
import serial
import random

from flask import Flask, render_template, request, redirect, url_for

app = Flask('spaceticker')
#app.debug = True

import logging
log = logging.getLogger('spaceticker')

class Spaceticker(object):
    def __init__(self, devices=[]):
        self.devices = devices
        self.serial = None

    def _reconnect(self):
        self.serial = serial.Serial(random.choice(self.devices), 115200, timeout=5)

    def write(self, data):
        while True:
            try:
                if self.serial is None:
                    self._reconnect()
                self.serial.write(data)
            except Exception, ex:
                log.error(ex)
                self.serial = None
                time.sleep(5)
            else:
                break

    def status(self, is_open):
        if is_open:
            cmd = 'so\n'
        else:
            cmd = 'sc\n'
        self.write(cmd)

    def send(self, msg):
        cmd = 'm%s\n' % msg
        self.write(cmd)

@app.route('/')
def index():
    return render_template('index.html', is_open=current_status)

@app.route('/msg')
def msg():
    msg = request.args.get('s')
    ticker.send(msg)
    return redirect(url_for('index'))

current_status = False
ticker = None

def init_app(serial_dev=None):
    def status_callback(is_open):
        global current_status
        log.debug('status changed to: %s', 'open' if is_open else 'closed')
        current_status = is_open
        ticker.status(current_status)
    from status import SpaceStatus
    s = SpaceStatus(status_callback)
    s.start()
    return app

if __name__ == '__main__':
    import sys
    log.setLevel(logging.DEBUG)
    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    ch.setFormatter(formatter)
    log.addHandler(ch)

    ticker = Spaceticker(sys.argv[1:])
    app = init_app()
    app.run(host="0.0.0.0")
