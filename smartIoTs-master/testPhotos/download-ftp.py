import pysftp

myHostname = "52.174.29.83"
myUsername = "ceyhuncoskun"
myPassword = "ceyhunZENO19"

cnopts = pysftp.CnOpts()
cnopts.hostkeys = None

with pysftp.Connection(host=myHostname, username=myUsername, password=myPassword,cnopts=cnopts) as sftp:
    # Define the file that you want to download from the remote directory
    files = sftp.listdir('pathole/FTP/Results-CAM-1')
    for file in files:
        print(file)
    remoteFilePath = 'pathole/FTP/Results-CAM-1'

    # Define the local path where the file will be saved
    # or absolute "C:\Users\sdkca\Desktop\TUTORIAL.txt"
    localFilePath = '/home/ceyhun/Desktop/FTP-Camera-Nodemcu/Zeno/Python-FTP/testPhotos'

    sftp.get_d(remoteFilePath, localFilePath, preserve_mtime=True)

# connection closed automatically at the end of the with-block'''