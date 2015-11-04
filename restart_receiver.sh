#!/usr/bin/env sh
ps -e | grep ap_signal_receiver | awk '{print $1}'|xargs kill -9 
ps -e | grep user_tag_receiver | awk '{print $1}'|xargs kill -9 
sleep 2

nohup ./ap_signal_receiver >/dev/null &
nohup ./user_tag_receiver >/dev/null &
