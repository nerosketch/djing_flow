# djing_flow
netflow dumper from flow-tools to mysql

If you want to export netflow data to the mysql database, then you may use
this application.

You can clone repository and compile it. Just run:
> $ make

Dependencies for successfully compile is a **flow-tools**.

For example, in order to get netflow data, you can run the flow-caption
like this:
> flow-capture -R /path/to/program/netflow_handler.sh -p /run/flow.pid -w /path/to/store/flows -n1 -N0 0/0/1234
Here you get the file in the folder */path/to/store/flows/*.
Run on port 1234, and dump the dump script "/path/to/program/netflow_handler.sh".

You can get a dump by sending a -HUP signal to the process like this:
> kill -HUP `cat /run/flow.pid.1234`

Then edit netflow_handler.sh and start_netflow.sh for your requirements.

for convert netflow data run:
> $ ./djing_flow -i |*path to table ip-userid*| -lc < your_netflow_data > result.sql

### Services
You may find units for systemd, that is: djing_rotate.service, and
djing_rotate.timer.
Edit that for your requirements

This app compress each session for ip for each one minute. This means that if
there are several sessions per minute, then the application will leave only one
record with the sum of bytes and packets of each session. For example, if you
have two sessions for ip 10.10.10.2 with two octets and bytes, application
leave one record with the sum of records of octets and bytes for each session.
The algorithm is realized through the binary tree. Left leaf is a ip that above
then next ip, and right ip that more then next ip. Address we take only one of
the source and destination addresses, the one that is on the subnet 10.0.0.0/8.
