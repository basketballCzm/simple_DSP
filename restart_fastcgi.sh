#!/usr/bin/env sh
pkill user_op
pkill ad_op
sleep 2

env spawn-fcgi -a 0.0.0.0 -p 9003 -C 25 -f  ./bin/user_op -F 20
env spawn-fcgi -a 0.0.0.0 -p 9004 -C 25 -f  ./bin/ad_op -F 10
