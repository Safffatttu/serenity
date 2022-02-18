#!/bin/sh
profile -e -a -t read; test-js; profile -d -a -t read; pv /proc/profile