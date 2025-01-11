#!/usr/bin/env python3
import os
import json

# Specify the directory you want to list
directory = './www/1serv'

# List all entries in the directory
entries = os.listdir(directory)

# Filter out directories, only keeping files
files = [entry for entry in entries if os.path.isfile(os.path.join(directory, entry))]

json_string = json.dumps(files, indent=4)
print("HTTP/1.1 200 OK")
print("Content-Type: text/html")
print("Connection: close")
print("Content-Length: " + str(len(json_string)))
print("")
print(json_string)