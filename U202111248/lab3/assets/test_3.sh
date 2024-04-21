#!/bin/bash
cd ./big-data

export END_POINT=http://ip:port

cargo test --package big-data --test concurrent -- test_3 --exact --nocapture > test_3.log

