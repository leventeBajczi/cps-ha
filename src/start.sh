#!/bin/bash

trap "trap - SIGTERM && kill -- -$$" SIGINT SIGTERM EXIT

cd Central && python main.py &
cd Driver\ Input\ \(HMI\) && python main.py &
cd Monitoring && python main.py &

$NANO_AGENT -a -U -c CPS.xml &
wait
