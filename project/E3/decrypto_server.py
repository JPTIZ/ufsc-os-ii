'''Module for server-side decryption handling.'''
import json
import sys

from collections import namedtuple
from hashlib import sha256
from Crypto.Cipher import AES
import serial



Message = namedtuple('Message', ['type', 'value', 'time'])


class AESCipher:
    def __init__(self, key):
        self.key = key

    def encrypt(self, msg):
        cipher = AES.new(self.key, AES.MODE_ECB)
        return cipher.encrypt(msg)

    def decrypt(self, msg):
        decipher = AES.new(self.key, AES.MODE_ECB)
        return decipher.decrypt(bytes(msg))


def hexify(data):
    return ''.join([format(i, 'x') for i in data])


def print_line(title):
    print('='*60 + '{:^23}'.format(title) + '='*60)


def print_hex(desc, data):
    print('    {desc:<17} ({size}): {data}'.format(
            desc=desc,
            size=len(data),
            data=hexify(data)
    ))


def print_cypher(enigma, crypto_msg, type='EXPECTED'):
    print_hex('HEX CYPHERED HASH', crypto_msg)

    print('    {type:<17} ({size}): {cypher}'.format(
        type=type,
        size=len(crypto_msg),
        cypher=hexify(enigma.decrypt(crypto_msg))))


def end():
    print_line('Server exited')
    for _ in range(5):
        print()


if __name__ == '__main__':
    for _ in range(5):
        print()

    print_line('Initialization')

    RAW_MSG = 'hey, I have a secret (not drugs)'
    MSG = sha256(bytes(RAW_MSG.encode('utf8'))).digest()
    KEY = bytes(0xaa for _ in range(16))

    print('raw-msg ({size}): {msg}'.format(
        size=len(RAW_MSG),
        msg=RAW_MSG
    ))
    print('sha-256 ({size}): {msg}'.format(
        size=len(MSG),
        msg=hexify(MSG)
    ))
    print('aes-key ({size}): {key}'.format(
        size=len(KEY),
        key=''.join([format(k, 'x') for k in KEY])
    ))

    enigma = AESCipher(KEY)

    print_line('Connecting to mote...')

    print('Getting port...', end='')
    try:
        port = sys.argv[1]
    except IndexError:
        port = '/dev/ttyACM1'
    print('Done ({}).'.format(port))

    print('Connecting to port...', end='')
    try:
        mote = serial.Serial(baudrate=115200, port=port, timeout=0.5)
    except Exception as e:
        print('Failed ({})'.format(e))
        end()
        exit(1)
    print('Done.')

    print('Connection stablished succesfully.')

    print_line('The EPOS Reading')

    hash_parts = []
    while True:
        response = str(mote.readline(), errors='ignore')

        if len(response) == 0:
            continue

        print('Message event: {response}'.format(response=response), end='')

        if response:
            type, value, time = json.loads(response)
            response = Message(type, value, time)

            if response.type == 'Hash':
                hash_parts += [response.value]
                if len(hash_parts) == 8:
                    print_line('Finished gathering')
                    print('hash-parts ({:<2}): {}'.format(len(hash_parts), hash_parts))
                    byte_parts = [part >> i & 0xff
                                  for part in hash_parts
                                  for i in (0,8,16,24)]
                    parts = bytes(byte_parts)
                    print_cypher(enigma, parts, type='DECYPHERED RESULT')

                    if enigma.decrypt(parts) == msg:
                        print_line('They Match!')
                    else:
                        print_line('They Do not Match...')
                    break

    end()
