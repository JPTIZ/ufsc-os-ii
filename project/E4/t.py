from time import time
import serial
import json
import sys

from hashlib import sha256
from Crypto.Cipher import AES

mote = serial.Serial(baudrate = 115200, port=sys.argv[1], timeout=0.5)
cloud_log = []
node_hash = []
initial_timestamp = 0
final_timestamp = 0

KEY = bytes(0xaa for _ in range(16))


def hexify(data):
    return ''.join([format(i, 'x') for i in data])


def check_logs():
    global cloud_log
    global node_hash
    global initial_timestamp
    global final_timestamp

    filtered = [x for x in cloud_log if x[2] >= initial_timestamp and
                                        x[2] <= (final_timestamp+300)]

    cloud_values = [int(inner[1]) for inner in filtered]

    node_crypto_parts = [int(inner[1]).to_bytes(8, byteorder='little', signed=True)
                         for inner in node_hash]

    hasher = sha256()
    for value in cloud_values:
        hasher.update(value.to_bytes(8, byteorder='little', signed=True))
    cloud_hash = hasher.digest()

    enigma = AES.new(KEY, AES.MODE_ECB)
    crypto_cloud_values = enigma.encrypt(bytes(cloud_hash))

    woof = ''.join([hexify(part) for part in node_crypto_parts])
    print('cloud_log: {}'.format(cloud_hash))
    print('filtered: {}'.format(filtered))
    print('node_hash: {}'.format(node_hash))
    print('node_crypto_parts: {}'.format(woof))
    print('crypto_cloud_values: {}'.format(hexify(crypto_cloud_values)))

    print(woof == hexify(crypto_cloud_values))

    node_hash = list()
    initial_timestamp = 0
    final_timestamp = 0


# write the data
epoch = '{time}X'.format(time=time())
mote.write(bytes(epoch, 'UTF-8'))

receiving = False
while True:
    msg = str(mote.readline(), errors='ignore')
    print(msg)
    if msg:
        response = json.loads(msg)
        if response[0] == "Temperature":
            cloud_log.append(response)
        else:
            if initial_timestamp == 0:
                initial_timestamp = response[1]
            elif final_timestamp == 0:
                final_timestamp = response[1]
            else:
                node_hash.append(response)
                if (len(node_hash) == 4):
                    print("Checando log")
                    check_logs()