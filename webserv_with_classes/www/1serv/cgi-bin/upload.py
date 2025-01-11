#!/usr/bin/env python3

import os

with open("./tmp/body", 'rb') as file:
    request_data = file.read()

content_type_header = b'Content-Type: multipart/form-data; boundary='
content_type_start = request_data.find(content_type_header)

if content_type_start == -1:
    raise ValueError("Multipart form-data boundary not found in the request.")

boundary_start = content_type_start + len(content_type_header)
boundary_end = request_data.find(b'\r\n', boundary_start)
boundary = request_data[boundary_start:boundary_end]

boundary_str = boundary.decode()

parts = request_data.split(b'--' + boundary)

for part in parts:
    if b'Content-Disposition' in part:  # This indicates it is a part with content
        # Find where the actual content starts (skip headers and blank line)
        body_start = part.find(b'\r\n\r\n') + 4  # Skip the headers and the blank line
        body = part[body_start:].strip()  # Get the body of the part

        # Find the filename from the Content-Disposition header (if it's a file)
        content_disposition_start = part.find(b'Content-Disposition: form-data; name="file"; filename="')
        if content_disposition_start != -1:
            filename_start = content_disposition_start + len(b'Content-Disposition: form-data; name="file"; filename="')
            filename_end = part.find(b'"', filename_start)
            filename = part[filename_start:filename_end].decode()
            output_file_path = "./uploads/" + filename
            # Save the file content to the specified output file path
            with open(output_file_path, 'wb') as output_file:
                output_file.write(body)
            print(f"<p>File saved as: {output_file_path}</p>")
            break

print("<p>Parsing completed.<p/>")

