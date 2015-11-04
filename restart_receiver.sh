#!/usr/bin/env sh
ps -e | grep ap_signal_receiver | awk '{print $1}'|xargs kill -9 
ps -e | grep user_tag_receiver | awk '{print $1}'|xargs kill -9 
sleep 2

nohup /etc/nginx/simple_DSP_cgi/bin/ap_signal_receiver &
nohup /etc/nginx/simple_DSP_cgi/bin/user_tag_receiver &
