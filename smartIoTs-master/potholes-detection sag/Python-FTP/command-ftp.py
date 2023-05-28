

from ftplib import FTP
from datetime import datetime
import sys
import os

server = sys.argv[1];
user = sys.argv[2];
pas = sys.argv[3];
command = sys.argv[4];

start = datetime.now()
ftp = FTP(server)
ftp.login(user,pas)
#ftp.rmd("CAM-1")
#ftp.mkd("CAM-2")
ftp.cwd("CAM-2")
ftp.retrlines(command)
ftp.close()
