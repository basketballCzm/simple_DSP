#!/bin/bash
# An alternative to "links", run this script after starting or stopping any
# container. It's a hack to update the host machine (vagrant) /etc/hosts with
# the current active docker containers and tell dnsmasq to refresh. 
#
# Then start each machine with "-dns ${DOCKER_HOST_IP}", e.g.
#    $ docker run -d -name mycontainer1 -dns 10.0.3.1 MYPRODUCT
# You can't seem to set the DNS during "docker build".
#
# Diagnostic command to run in host or while logged into containers:
#    # dig @10.0.3.1 mycontainer1
# 
cd ${0%/*}
IFS=$'\n'

function dip() { docker inspect $1 | grep IPAddress | cut -d '"' -f 4 ; }


function sync()
{
	hosts=`sed -n -e '/# Below are the docker hosts running at/,$p' /etc/hosts`
	echo `echo $hosts | wc -l`
	docker exec $1 cp /etc/hosts /etc/hosts.bak
	docker exec $1 sed -i '/#DOCKER-DELETE-ME/d' /etc/hosts.bak
	echo $hosts
	for line in $hosts 
	do
		echo "sync in docker container, line is "$line
		docker exec $1 sed -i "\$a$line" /etc/hosts.bak
	done
	docker exec $1  cp /etc/hosts.bak /etc/hosts
}

cat /etc/hosts | grep -v '#DOCKER-DELETE-ME' > /etc/hosts.docker.tmp
RESULT="$?"
if [ ${RESULT} = 0 ]; then
   echo "Checking for running docker containers..."
else
   echo "Error modifying /etc/hosts, try running with sudo."
   exit 1
fi

echo "# Below are the docker hosts running at $(date). #DOCKER-DELETE-ME" >> /etc/hosts.docker.tmp


docker ps | awk '{print $1}'| grep -v "CONTAINER" | while read CONTAINERID
do
   IP=$(dip ${CONTAINERID})
   if [ -n "${IP}" ] ; then
      NAME=$(docker inspect ${CONTAINERID} | grep '"Hostname"'| cut -d '"' -f 4 | sed 's#^/##g')
      echo "${IP}    ${NAME}  #DOCKER-DELETE-ME" >> /etc/hosts.docker.tmp
   fi
done

mv -f /etc/hosts.docker.tmp /etc/hosts
killall -HUP dnsmasq
echo 'Updated /etc/hosts with current ("docker ps") entries...'


docker ps | awk '{print $1}'| grep -v "CONTAINER" | while read CONTAINERID
do
	sync ${CONTAINERID}
done
