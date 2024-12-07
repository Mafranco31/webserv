import os

# Specify the directory you want to list
directory = './www/1serv'

# List all entries in the directory
entries = os.listdir(directory)

# Filter out directories, only keeping files
files = [entry for entry in entries if os.path.isfile(os.path.join(directory, entry))]

print(files)