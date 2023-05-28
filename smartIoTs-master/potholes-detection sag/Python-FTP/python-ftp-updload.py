import ftplib
import sys

server = sys.argv[1];
user = sys.argv[2];
pas = sys.argv[3];
f = sys.argv[2];

print(server + user + pas + f)

session = ftplib.FTP(server)
file = open(f,'rb')                  # file to send
session.storbinary('STOR ' + f, file)     # send the file
file.close()                                    # close file and FTP
session.quit()