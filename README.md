# djing_flow
netflow dumper from flow-tools to mysql

If you want to export netflow data to the mysql database, then you may use this application.

You can clone repository and compile it. Just run:
> $ make

Dependencies for successfully compile is a **flow-tools**.

For example, in order to get netflow data, you can run the flow-caption like this:
> flow-capture -R /path/to/program/netflow_handler.sh -p /run/flow.pid -w /path/to/store/flows -n1 -N0 0/0/6343
Here you get the file in the folder */path/to/store/flows/*. Run on port 6343, and dump the dump script "/path/to/program/netflow_handler.sh".

You can get a dump by sending a -HUP signal to the process like this:
> kill -HUP `cat /run/flow.pid`

Then edit netflow_handler.sh and start_netflow.sh for your requirements.

for convert netflow data run:
> $ ./djing_flow < your_netflow_data > result.sql

### Services
You may find units for systemd, that is: djing_rotate.service, and djing_rotate.timer.
Edit that for your requirements
