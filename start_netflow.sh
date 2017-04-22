#!/bin/sh

PATH=/usr/local/sbin:/usr/local/bin:/usr/bin:/usr/lib/jvm/default/bin:/home/dn/bin

flow-capture -R /home/dn/bin/netflow_handler.py -w /var/db/flows -n1 -N0 0.0.0.0/0.0.0.0/8888
