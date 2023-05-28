#!/usr/bin/env python
# -*- coding: utf-8 -*-

# import predict
import os
import shutil

from .util import state_of_comp


class Cam:

    def __init__(self, home, route, user, sleep_seconds, last_state, disconnected_loop_count, maximum_delay):
        self.home = home
        self.route = route
        self.user = user
        self.sleep_seconds = sleep_seconds
        self.last_state = last_state
        self.disconnected_loop_count = disconnected_loop_count
        self.maximum_delay = maximum_delay

    def pathole_cam(self):
        files = os.listdir(self.home)  # list of folders each CAM folder represents one real cam
        for file in files:
            if file.startswith("CAM"):
                new_dir = "Results-" + file
                if not os.path.isdir(new_dir):
                    os.mkdir(new_dir)
                    print("\n" + new_dir + " Directory created!")
                cam_id = file
                for d in os.listdir(file):
                    if d.startswith("true"):
                        for f in os.listdir(file):
                            if f.endswith(".jpeg"):
                                if not os.path.exists(f):
                                    print("\nMoving.." + file + "/" + f + " to " + new_dir + "/")
                                    shutil.move(file + "/" + f, new_dir + "/" + f)
                                    print("\nDeleted..." + f)
                                    print("\n Predicting... " + f)
                                    os.chdir("../")
                                    pothole_count = predict._main_("config.json", "trained_wts.h5",
                                                                   "FTP/" + new_dir + "/" + f)
                                    os.chdir("FTP")
                if pothole_count > 0:
                    print("\nHTTP Posting result for Tubitak-" + cam_id + " Pathole Count: " + str(
                        pothole_count) + " Source file: " + f)
                    r = requests.post("http://tubitaklinuxapi.azurewebsites.net/api/cam/processrequest",
                                      data={"InitiatorId": cam_id, "PatholeCount": pothole_count})
                    print("\nHTTP Response from Tubitak-CAM: " + r.text)
