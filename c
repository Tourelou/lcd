#! /bin/bash

if [[ $(basename "$(pwd)") == "build" ]]; then cmake .. && make
elif [[ $0 == "./c" ]]; then
	mkdir -pv build && cd build
	if [[ $? == 0 ]]; then cmake .. && make; fi
else echo "Il faut être à l'intérieur du dossier de la compilation au moins."
fi
