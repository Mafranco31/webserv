#!/usr/bin/env python3

import cgi
# import cgitb

# Enable debugging
# cgitb.enable()


# import cgi

form = cgi.FieldStorage()

print(form["file"].filename)

# if "file" in form:
#     file_item = form["file"]
#     if file_item.filename:
#         # Save the file to disk
#         print("<p>File uploaded:</p>")
#         with open(f"./www/1serv/cgi-bin/uploads/{file_item.filename}", "wb") as f:
#             f.write(file_item.file.read())
#         print(f"<p>File {file_item.filename} uploaded successfully.</p>")
#     else:
#         print("<p>No file was uploaded.</p>")
# else:
#     print("<p>No file field in the form.</p>")
exit(1)



# #!/usr/bin/env python3

# import cgi
# import os
# import cgitb

# # Enable debugging
# cgitb.enable()

# # Parse form data
# form = cgi.FieldStorage()

# # Retrieve the uploaded file
# file_item = form["file"]

# # Check if the file was uploaded
# if file_item.filename:
#     # Get the filename
#     filename = os.path.basename(file_item.filename)
#     # Save the file to the server
#     with open(f"/tmp/{filename}", "wb") as file:
#         file.write(file_item.file.read())
    
#     print(f"""
#     <!DOCTYPE html>
#     <html>
#     <head>
#         <title>File Uploaded</title>
#     </head>
#     <body>
#         <h1>File Uploaded Successfully</h1>
#         <p>Filename: {filename}</p>
#     </body>
#     </html>
#     """)
# else:
#     print("<h1>No file uploaded</h1>")
