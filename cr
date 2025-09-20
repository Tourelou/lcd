#! /bin/bash

if [[ $(basename "$(pwd)") == "build" ]]; then :
elif [[ -d "build" ]]; then cd build
else
	echo "Je ne trouve pas build."
	exit 1
fi

cmake .. && make && echo; ./$(awk -F '/' '{ print $(NF-1) }' <<<$(pwd))
