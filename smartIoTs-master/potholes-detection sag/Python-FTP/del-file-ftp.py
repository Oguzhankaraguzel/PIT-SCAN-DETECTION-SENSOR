

from ftplib import FTP
from datetime import datetime
import sys
import os

#os.system("ls -l")

server = sys.argv[1];
user = sys.argv[2];
pas = sys.argv[3];
f = sys.argv[4];

start = datetime.now()
ftp = FTP(server)
ftp.login(user,pas)
ftp.retrlines('LIST')
# Get All Files
ftp.cwd("CAM-2")
files = ftp.nlst()

# Print out the files
for file in files:
	if file == f:
		ftp.delete(file)
		print("\n*****************************\n")
		print("Deleted..." + file)
		print("\n*****************************\n")

ftp.retrlines('LIST')
ftp.close()
