#!/bin/bash
sudo mkdir -p /opt/simple_DSP
sudo chown -R caster:caster /opt/simple_DSP
cp -rf /home/caster/simple_DSP/* /opt/simple_DSP
cd /opt/simple_DSP
./restart_fastcgi.sh
sleep 1
./restart_receiver.sh