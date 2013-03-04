import requests
import json
import socket
import threading
import time

STATUS_URL = "http://status.kreativitaet-trifft-technik.de/status-stream"

import logging
log = logging.getLogger('spaceticker')

def parsed_events(events):
    for event_lines in events:
        resp = {}
        for line in event_lines:
            key, value = line.split(': ', 1)
            try:
                resp[key] = json.loads(value)
            except ValueError:
                resp[key] = value
        yield resp

def event_chunks(resp):
    last_event_data = []
    for line in resp.iter_lines(1):
        if not line:
            if last_event_data:
                yield last_event_data
            last_event_data = []
        else:
            last_event_data.append(line)

class SpaceStatus(threading.Thread):
    def __init__(self, status_callback):
        threading.Thread.__init__(self)
        self.daemon = True
        self.status_callback = status_callback

    def run(self):
        while True:
            try:
                resp = requests.get(STATUS_URL, stream=True, timeout=60*60)
                for event in parsed_events(event_chunks(resp)):
                    if event['event'] == 'status':
                        self.status_callback(event['data']['open'])
            except socket.timeout:
                continue
            except requests.RequestException, ex:
                log.error('request error: %s' % ex)
                time.sleep(10)
            except Exception:
                log.exception('unknown request error:')
                time.sleep(60)

if __name__ == '__main__':
    def status_callback(open):
        if open:
            print "It's open!"
        else:
            print "It's closed!"
    s = SpaceStatus(status_callback)
    s.start()
    try:
        time.sleep(60)
    except (KeyboardInterrupt, SystemExit):
        pass
