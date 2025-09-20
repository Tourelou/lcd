#! /bin/bash

cmake .. && make && echo; ./$(awk -F '/' '{ print $(NF-1) }' <<<$(pwd))
