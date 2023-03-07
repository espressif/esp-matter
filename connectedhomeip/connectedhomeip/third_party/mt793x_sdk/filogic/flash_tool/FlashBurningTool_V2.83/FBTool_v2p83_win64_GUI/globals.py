#!/usr/bin/env python3
# -*- coding: utf-8 -*
def initialize():
    global total_max,now_process,flash_burning_status,quickabort,gui_phase,test_phase,time_inc,tempdata,test_DA,dwarray_data,scatter_loca
    global cmds_tab_cycle,total_cmds,readback_txt_yview
    print("globals_init")
    readback_txt_yview=1
    total_max=0
    now_process=0
    flash_burning_status=2
    quickabort=0
    gui_phase=0
    time_inc=0
    test_phase=0
    tempdata=0
    dwarray_data = [0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
    test_DA=0
    scatter_loca=""
    cmds_tab_cycle=0
    total_cmds=0




