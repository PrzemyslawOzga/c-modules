# LINUX KEYLOGGER MODULE

A simple keylogger module for logging keyboard events (US type). The module captures keyboard events and writes them to a buffer with a default size of 16. Scripts available on the repository offer the ability to:
- Read the buffer's contents
- Change the buffer's size
- Clear the buffer of data (this option does not free the buffer, the buffer is freed when the module is turned off).

## How to enable module
To enable the module, download the repository on your Linux system (I used Ubuntu 20.04 LTS), and follow the steps below:
```bash
make  # build module 
sudo insmod build/keylogger.ko  # enabling the module
lsmod | grep keylogger  # check if module is enabled
```

You can build the module in debug mode, then add the following command to the module enable line (by default, the module is built without debugging options):
```bash
sudo insmod build/keylogger.ko debug_enabled=1
```
After enabling the debugging option, additional messages about enabling, operating and disabling the module will appear in the dmsg system log.

## How to disable module
To disable the module use the command:
```bash
sudo rmmod keylogger  # disable module
make clean  # clear build folder with module
lsmod | grep keylogger  # check if module is disabled
```



## How to use the scripts
To use scripts to support the keylogger module, build them:
```bash
cd scripts/
gcc -o clear_buffer_data clear_buffer_data.c
gcc -o read_buffer_data read_buffer_data.c
gcc -o set_buffer_size set_buffer_size.c
cd ..
```

It is important to run scripts with root privileges e.g.:
```bash
sudo ./scripts/read_buffer_data
sudo ./scripts/set_buffer_size  # After executing this command you will be asked to enter the buffer size
sudo ./scripts/clear_buffer_data
```

