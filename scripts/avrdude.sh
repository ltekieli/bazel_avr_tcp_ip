#!/bin/bash

set -e

firmware=""

OPTIND=1
while getopts "h?f:" opt; do
    case "$opt" in
    h|\?)
        echo "Use wisely"
        exit 0
        ;;
    f)  firmware=$OPTARG
        ;;
    esac
done
shift $((OPTIND-1))
[ "${1:-}" = "--" ] && shift

if [ -z "$firmware" ]; then
    echo "ERROR: Firmware file not specified"
    exit 1
fi

avrdude -p atmega32u4 -P usbasp -c usbasp -U flash:w:"$firmware"
