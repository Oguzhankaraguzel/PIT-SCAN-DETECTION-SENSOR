#!/usr/bin/env python
# -*- coding: utf-8 -*-


class Sensor(object):

    def __init__(self, state=None, info=None, comp_id=None):
        self.state = state
        self.info = info
        self.comp_id = comp_id

