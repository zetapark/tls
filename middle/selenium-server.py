#!/usr/bin/python3
# coding: utf-8

from zeta import Server
from selenium import webdriver

drv = webdriver.PhantomJS()
interval = 0

def f(url):
    global interval, drv
    interval += 1
    if interval == 20:
        interval = 0
        drv.close()
        drv = webdriver.PhantomJS()

    drv.get(url)
    return drv.page_source

sv = Server(3004)
sv.nokeep_start(f)

