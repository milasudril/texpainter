#!/bin/bash

hexdump -n 16 -e '4/4 "%08x" 1 "\n"' /dev/random