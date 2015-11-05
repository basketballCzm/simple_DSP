#!/usr/bin/env sh
ps -e | grep ap_signal_recei | awk '{print $1}'|xargs kill -9 
ps -e | grep user_tag_receiv | awk '{print $1}'|xargs kill -9 
sleep 2

nohup ./ap_signal_receiver 1>/dev/null 2>&1 &
nohup ./user_tag_receiver 1>/dev/null 2>&1 &
