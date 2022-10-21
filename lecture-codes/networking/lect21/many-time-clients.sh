#!/bin/bash
# This script runs many time clients in the background all at the same time.
# The script requires that you specify 3 arguments:
#
# ./many-time-clients.sh [server-hostname] [server-port] [number-of-clients]
#
# E.g. ./many-time-clients.sh myth64.stanford.edu 12345 20
# would run 20 time clients concurrently that all try to connect to the time
# server at myth64.stanford.edu on port 12345.

for ((i = 0; i < $3; i++)); do
    ./time-client $1 $2 &
done