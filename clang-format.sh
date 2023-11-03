#!/bin/bash

find . -name "*.c" -exec clang-format -i {} \;
find . -name "*.h" -exec clang-format -i {} \;
