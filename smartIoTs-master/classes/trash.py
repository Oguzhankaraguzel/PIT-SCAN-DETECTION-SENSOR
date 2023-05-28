#!/usr/bin/env python
# -*- coding: utf-8 -*-


class Trash(object):

    def __init__(self, user=None, info=None, trash_id=None, state=None, trash_comps=None, true_data_list=None, false_data_list=None):
        self.user = user
        self.info = info
        self.state = state
        self.trash_id = trash_id
        self.trash_comps = trash_comps
        self.true_data_list = true_data_list
        self.false_data_list = false_data_list
