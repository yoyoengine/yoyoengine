"""
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.

    Simple UDP ping server for testing yoyoengine networking
    (generated almost entirely by chatgpt)
"""

import socket
import json
from datetime import datetime

def udp_echo_server(host, port):
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        s.bind((host, port))
        print(f"UDP echo server listening on {host}:{port}")
        while True:
            data, addr = s.recvfrom(1024)
            timenow = datetime.now()
            # Decode bytes to string
            data_str = data.decode('utf-8')
            try:
                # Parse JSON string to Python object
                json_data = json.loads(data_str)
                print(f"Received JSON data from {addr}: {json_data}")
            except json.JSONDecodeError:
                print(f"Received non-JSON data from {addr}: {data_str}")
            # Echo back the original data
            s.sendto(b'{"type":"pong"}', addr)

            print(f"time it took to send pong: {(datetime.now() - timenow).total_seconds() * 1000}ms")

if __name__ == "__main__":
    udp_echo_server("127.0.0.1", 1234)