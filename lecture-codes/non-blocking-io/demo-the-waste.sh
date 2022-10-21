#!/bin/bash

./expensive-server &
pid=$!
sleep 1
kill $pid
