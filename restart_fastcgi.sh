pkill user_op
env spawn-fcgi -a 127.0.0.1 -p 9003 -C 25 -f  /etc/nginx/simple_DSP_cgi/bin/user_op -F 10

pkill ad_op
env spawn-fcgi -a 127.0.0.1 -p 9004 -C 25 -f  /etc/nginx/simple_DSP_cgi/bin/ad_op -F 10
