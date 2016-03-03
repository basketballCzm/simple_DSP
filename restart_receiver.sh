#!/usr/bin/env sh
ps -e | grep ap_signal_recei | awk '{print $1}'|sudo xargs kill -9 
ps -e | grep user_tag_receiv | awk '{print $1}'|sudo xargs kill -9 
ps -e | grep user_phone_rece | awk '{print $1}'|sudo xargs kill -9 
sleep 2

sudo mkdir -p /var/log/wushuu_dsp
sudo chown caster:caster /var/log/wushuu_dsp

nohup ./bin/ap_signal_receiver 1>/var/log/wushuu_dsp/ap_signal_receiver.log 2>&1 &
nohup ./bin/user_tag_receiver 1>/var/log/wushuu_dsp/user_tag_receiver.log 2>&1 &
nohup ./bin/user_phone_receiver 1>/var/log/wushuu_dsp/user_phone_receiver.log 2>&1 &
