Arf-ID visualizer 

This python script reads in BLE data from the RFID reader and visualizes the spatial 
location of the tags.

It requires Python 3.11.9 and simplepyble, which can be installed through poetry. Yo will need a Python 3.11.9 interpreter downloaded (simplepyble fails to build on the latest python interpreter) and tell Poetry to use it via the command "poetry env use xxx" where "xxx" is the path to the interpreter. 


To use it, run poetry run python connect_ble.py in /src.

The script will ask which BLE interface to use on your computer. (Often the default 0 will work.) It will attempt to connect to the device and print out the EPC, RSSI, and timestamp of readings. 

Additionally, a GUI will pop up showing squares intended to visualize tags spread through an area. To use it, 

Modify lines 149-151 that look like this:

        self.grid[0][8].EPC = "0x111111222222333333444444"
        self.grid[0][0].EPC = "0x111133b2ddd9014000000000"
        self.grid[1][2].EPC = "0x300833b2ddd9014000000000"

where the indices correspond to the grid location and EPC corresponds to tag EPC (which can be read in the terminal). Add an entry to show a green square that glows red at the specific location when the corresponding tag at that location is scanned. This is meant to help show the tags that are scanned as the dog passes by them.

The script will handle connection drops and try to reconnect when possible. It can be run at any time that the device is online, and can be stopped and started at no interruption to the regular device function.


