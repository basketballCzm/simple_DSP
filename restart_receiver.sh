#!/usr/bin/env sh
pkill ap_signal_receiver
pkill user_tag_receiver 
sleep 2

/etc/nginx/simple_DSP_cgi/bin/ap_signal_receiver &
/etc/nginx/simple_DSP_cgi/bin/user_tag_receiver &
