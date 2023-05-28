#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
from .util import *
from classes.trash import Trash


class CompSession:

    def __init__(self, home, route, user, sleep_seconds=0.5, disconnected_loop_count=-1, maximum_delay=10):
        self.user = user
        self.home = home
        self.route = route
        self.sleep_seconds = sleep_seconds
        self.maximum_delay = maximum_delay
        self.disconnected_loop_count = disconnected_loop_count

    def trash(self):

        files = os.listdir(self.home)  # list of folders each TRASH folder represents one real trash
        for file in files:
            if file.startswith("TRASH"):  # trash folders goes like TRASH-1, TRASH-2

                trash = Trash()  # create new Trash instance
                trash.trash_comps = []  # all components that we have for one trash
                trash.true_data_list = []  # all true returned sensors for one trash
                trash.false_data_list = []  # all false returned sensors for one trash

                f = open(self.home + file + "/comps.txt", "r")  # we have comps.txt for every TRASH folder
                json_comps = eval(f.read())

                for comp_id in json_comps:  # every comps.txt has sensor id's in json format
                    trash.trash_comps.append(comp_id["id"])

                trash.trash_id = file  # we are getting trash id from folder name
                dir_files = os.listdir(self.home + file)

                for d in dir_files:

                    if d.startswith("true"):  # if folder starts with true means sensor gives trash full signal
                        trash.true_data_list.append(d[5:])  # last part of the folder name is sensor id etc.true-1A2F
                        os.rmdir(self.home + file + "/" + d)

                    if d.startswith("false"):  # if folder starts with false means sensor gives trash full signal
                        trash.false_data_list.append(d[6:])
                        os.rmdir(self.home + file + "/" + d)

                # using method to create a list for posting
                decided_list, trash, comp_session = object_router(trash, self)

                print("\nHTTP Posting result for Tubitak-" + trash.trash_id)
                print("\nState: " + trash.state + "...")
                print("\nInfo: " + trash.info)
                print("\nTry Attempt: " + str(comp_session.disconnected_loop_count))

                # posting founded result vai method
                post_loop(decided_list, trash, comp_session)

                # returning last state for giving information and loop count for maximum disconnect delay
                return comp_session.disconnected_loop_count
