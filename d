#! /bin/bash

if [[ $(basename "$(pwd)") == "build" ]]; then :
elif [[ -d "build" ]]; then cd build
else
	echo "cd au moins dans le répertoire de compilation."
	exit 1
fi

sudo rm -rf ./*
sudo rm -rf ./.c*
