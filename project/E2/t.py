from time import time
import serial
import json

mote = serial.Serial(baudrate = 115200, port='/dev/ttyACM0', timeout=0.5)
cloud_log = list()
node_log = list()
initial_timestamp = 0
final_timestamp = 0

def check_logs():
    global cloud_log
    global node_log
    global initial_timestamp
    global final_timestamp

    filtered = list(filter((lambda x : x[2] >= initial_timestamp and x[2] <= (final_timestamp+300)), cloud_log))

    test_cloud = list()
    test_node = list()
    for inner in filtered:
        test_cloud.append(inner[1])
    for inner in node_log:
        test_node.append(inner[1])

    print(test_node == test_cloud)

    node_log = list()
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
                node_log.append(response)
                if (len(node_log) == 3):
                    print("Checando log")
                    check_logs()
