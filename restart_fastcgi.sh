#!/usr/bin/env sh
pkill user_op
pkill ad_op
sleep 2

env spawn-fcgi -a 0.0.0.0 -p 9003 -C 25 -f  ./user_op -F 10
env spawn-fcgi -a 0.0.0.0 -p 9004 -C 25 -f  ./ad_op -F 10
