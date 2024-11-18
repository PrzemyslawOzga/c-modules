# LINUX MEMORY USAGE LOGGER MODULE

A simple Linux kernel module that logs memory usage information of all active processes. The module iterates through each running process, recording details such as the virtual memory size and Resident Set Size (RSS) in megabytes to the system log (`dmesg`). This module is useful for basic system monitoring or for understanding memory allocation in the Linux kernel.

## Features
- Logs each active process’s name, PID, virtual memory size, and RSS in MB.
- Outputs memory details to `dmesg` upon loading.
- Automatically unloads after logging memory usage.

## How to Build and Enable the Module
To enable the module, clone this repository onto your Linux system (tested on Ubuntu 20.04 LTS) and follow the steps below:
```bash
make  # build module 
sudo insmod build/memory_usage_logger.ko  # enabling the module
lsmod | grep memory_usage_logger  # check if module is enabled
```
This module outputs memory usage details to dmesg immediately upon being loaded.

## Automated Logging Script
To load and immediately unload the module, use the provided script, which also saves the output logs from dmesg:
```bash
./run_memory_logger.sh  # ensure this script is executable
```

The run_memory_logger.sh script:
1. Checks if the module is already loaded and removes it if so.
2. Loads the module to log memory usage.
3. Records logs from dmesg for the last few seconds into a .log file.
4. Unloads the module to clean up.

## How to Disable the Module
To manually disable the module, use the command:
```bash
sudo rmmod memory_usage_logger  # disable module
make clean  # clear build folder with module
lsmod | grep memory_usage_logger  # check if module is disabled
```

## Example Output
Upon loading, the module outputs logs similar to the following to dmesg:
```bash
--------------------------------------------
Process: bash (PID: 1234)
 Total Virtual Memory: 12.50 MB
 RSS (Resident Set Size): 3.20 MB
--------------------------------------------
```
