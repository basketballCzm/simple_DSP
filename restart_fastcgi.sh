pkill adstat
/usr/local/nginx/sbin/spawn-fcgi -a 127.0.0.1 -p 9000 -C 25 -f  /usr/local/nginx/simple_DSP_cgi/bin/adstat -F 10

pkill ad_register
/usr/local/nginx/sbin/spawn-fcgi -a 127.0.0.1 -p 9001 -C 25 -f  /usr/local/nginx/simple_DSP_cgi/bin/ad_register -F 10
