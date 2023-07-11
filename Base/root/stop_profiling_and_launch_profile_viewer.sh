#!/bin/Shell

echo "putting the lotions in the basket"

# Stop profiling
profile -d -a

# Copy the profile files to the host
cp /sys/kernel/profile "p6"

# Shut down the VM
sync
shutdown
