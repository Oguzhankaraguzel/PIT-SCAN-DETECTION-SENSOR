#!/usr/bin/env python
# -*- coding: utf-8 -*-


import time
from classes.comp_session import CompSession

HOME = "/home/ceyhuncoskun/pathole/FTP/"
ROUTE = "http://tubitaklinuxapi.azurewebsites.net/api/trash/processrequest"
SLEEP_SECONDS = 0.5
USER = "Ceyhun"


def main():
    session = CompSession(HOME, ROUTE, USER)
    while True:
        try:
            session.disconnected_loop_count = session.trash()
            print("\nTry Attempt: " + str(session.disconnected_loop_count) + " Waiting for " + str(
                (session.disconnected_loop_count * 5)) + " seconds")
        except:
            print("Error at trash")
            pass

        print("\n*****  Every 5 seconds... ****\n")
        time.sleep(5)


if __name__ == "__main__":
    main()
