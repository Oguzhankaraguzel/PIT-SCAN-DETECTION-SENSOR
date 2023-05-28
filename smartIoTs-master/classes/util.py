#!/usr/bin/env python
# -*- coding: utf-8 -*-

import requests

from classes.sensor import Sensor
from .trash import Trash
from .cam import Cam


def state_of_comp(state_number):
    switcher = {
        1: "Empty",
        2: "Full",
        3: "Error",
        4: "Pending",
        5: "Disconnected"
    }
    return switcher.get(state_number, "Unknown")


def post_data(object_instance, comp, comp_session):
    if isinstance(object_instance, Trash):
        # post url and json format User and SubEquipmentId part are affects data
        r = requests.post(comp_session.route,
                          data={
                              "InitiatorId": object_instance.trash_id,
                              "State": object_instance.state,
                              "User": comp_session.user,
                              "SubEquipmentId": comp.comp_id,
                              "SubEquipmentState": comp.state,
                              "Info": comp.info}
                          )

        print("HTTP Response from Tubitak-" + object_instance.trash_id + " : " + comp.comp_id + " : " + r.text + "\n")

    """
    if object_instance == Cam():
        for comp in component_list:
            post_data(object_instance.trash_id, object_instance.state, comp)
    """


# posting data by iterating created list
def post_loop(component_list, object_instance, comp_session):
    if isinstance(object_instance, Trash):
        for comp in component_list:
            post_data(object_instance, comp, comp_session)
    """
    if object_instance == Cam():
        for comp in component_list:
            post_data(object_instance.trash_id, object_instance.state, comp)
    """


def trash_decision_tree(trash, comp_session):
    decided_list = []

    trash_comps = trash.trash_comps
    true_data_list = trash.true_data_list
    false_data_list = trash.false_data_list

    maximum_delay = comp_session.maximum_delay
    disconnected_loop_count = comp_session.disconnected_loop_count

    data_received = list(set(true_data_list + false_data_list))

    if len(false_data_list) != 0 or len(true_data_list) != 0:  # if any data exists
        if (len(false_data_list) + len(true_data_list)) > len(trash_comps):  # if data is more than expected
            info = "Too much data..."
            state = state_of_comp(3)
            for comp in trash_comps:
                s = Sensor(state_of_comp(0), "Too much data...", comp)
                decided_list.append(s)
        elif len(trash_comps) == len(false_data_list):  # if false data equals to sensor count
            info = "Success"
            state = state_of_comp(1)
            for comp in trash_comps:
                s = Sensor(state_of_comp(1), "Success", comp)
                decided_list.append(s)
            disconnected_loop_count = 0
        elif len(trash_comps) == len(true_data_list):  # if true data equals to sensor count
            info = "Success"
            state = state_of_comp(2)
            for comp in trash_comps:
                s = Sensor(state_of_comp(2), "Success", comp)
                decided_list.append(s)
            disconnected_loop_count = 0
        else:
            # here we are using three pre created lists for find disconnected sensor and their states
            state = state_of_comp(3)
            info = "data exists but component "
            for comp in trash_comps:
                if comp in data_received:
                    if comp in true_data_list:
                        s = Sensor(state_of_comp(2), "Data exists this " + state_of_comp(2), comp)
                        decided_list.append(s)
                    else:
                        s = Sensor(state_of_comp(1), "Data exists this " + state_of_comp(1), comp)
                        decided_list.append(s)
                else:
                    s = Sensor(state_of_comp(5), state_of_comp(5), comp)
                    decided_list.append(s)
                    info += comp + ", "
            info += " can't send data"
            disconnected_loop_count = 0
    else:
        # if not any data exists delay count is increasing
        disconnected_loop_count += 1
        info = "Waiting for data about " + str(
            (disconnected_loop_count * 5)) + " seconds."
        state = state_of_comp(4)
        for comp in trash_comps:
            s = Sensor(state_of_comp(4), state_of_comp(4), comp)
            decided_list.append(s)

    # if delay count is bigger than decided maximum delay we are sending disconnected message
    if disconnected_loop_count > maximum_delay:
        for comp in trash_comps:
            s = Sensor(state_of_comp(5), state_of_comp(5), comp)
            decided_list.append(s)

    trash.info = info
    trash.state = state
    comp_session.disconnected_loop_count = disconnected_loop_count

    return decided_list, trash, comp_session


def cam_decision_tree():
    v = 3


def object_router(object_instance, comp_session):
    if isinstance(object_instance, Trash):
        decided_list, trash, comp_session = trash_decision_tree(object_instance, comp_session)
    # if object_type == Cam():
    #   cam_decision_tree(object_type, comp_session)

    return decided_list, trash, comp_session
