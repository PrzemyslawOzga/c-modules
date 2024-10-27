#!/bin/bash

LOG_FILE="dmesg_memory_usage.log"
TIMESTAMP=$(date +"%b %d %H:%M:%S" --date='1 second ago')

if lsmod | grep -q "memory_usage_logger"; then
    echo "Module is already loaded. Removing..."
    sudo rmmod memory_usage_logger
    echo "Module removed."
fi

echo "Inserting the module..."
sudo insmod ./build/memory_usage_logger.ko

sleep 1

dmesg -T | grep "$TIMESTAMP" > $LOG_FILE

echo "Removing the module..."
sudo rmmod memory_usage_logger
echo "Module removed."

echo "Log entries saved to $LOG_FILE."
