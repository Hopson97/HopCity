#!/bin/bash

if [ "$1" = "release" ]
then
    ./build/release/bin/HopCity 
else
    ./build/debug/bin/HopCity 
fi