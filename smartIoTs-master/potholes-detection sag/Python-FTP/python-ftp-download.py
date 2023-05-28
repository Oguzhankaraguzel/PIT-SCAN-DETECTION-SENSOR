from ftplib import FTP
from datetime import datetime
import sys
import os
import time

server = sys.argv[1]
user = sys.argv[2]
pas = sys.argv[3]
#f = sys.argv[4]

start = datetime.now()
ftp = FTP(server)
ftp.login(user,pas)
# Get All Files
files = ftp.nlst()

print("\n")
while(True):
	try:
		for file in files:
			if file.startswith("CAM"):
				if not os.path.isdir(file):
					os.mkdir(file)
					print("\n" + file + " Directory created!")
				dirfiles = ftp.nlst(file)
				upToDate = True
				for f in dirfiles:
					if f.endswith(".jpeg"):
						"""
						fname = ""
						for ch in f:
							if ch != "/":
								fname += ch
							else:
								fname = ""
						"""
						realSize = ftp.size(f)
						if not (os.path.exists(f)) or realSize != os.path.getsize(f):
							print("\nDownloading..." + f)
							ftp.retrbinary("RETR " + f ,open(f, 'wb').write)
							print("\n" + str(realSize) + " bytes of " + str(os.path.getsize(f)) + " bytes Downloaded.")
							#print(os.path.getsize(f))
							upToDate = False
							#ftp.retrbinary("RETR " + f ,open(fname, 'wb').write)
							#ftp.delete(f)
							#print("\nDeleted..." + f)
				if upToDate:
					time_format="%Y-%m-%d-%H-%M"
					now = datetime.now()
					date = now.strftime(time_format)
					print(str(date) + " " + file + " Up to Date !")
		print("\n*****  Every 5 seconds... ****\n")
		time.sleep(5)
	except:
		pass

end = datetime.now()
diff = end - start
print('\nAll files downloaded for ' + str(diff.seconds) + 's')
