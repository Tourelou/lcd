#! /usr/bin/env bash

if [[ $# -ge 1 ]]; then
	echo "Nous avons $# argument(s)."
#	exit
	PRGNAME="$(basename "$(pwd)")"

	if [[ -f "CMakeLists.txt" ]]; then
		for toolchain in $@; do
			architecture=$(echo $toolchain | awk -F'[_.]' '{print $1}')
			echo -n "Nous compilons pour une architecture $architecture "
			echo "=> $PRGNAME.$architecture"
			cmake -S . -B build.$architecture -DCMAKE_TOOLCHAIN_FILE=$toolchain
			cmake --build build.$architecture
			if [[ $? == 0 ]]; then
				mv "build.$architecture/$PRGNAME" "./$PRGNAME.$architecture"
				rm -rf build.$architecture
			else
				rm -rf build.$architecture
			fi
		done
	else
		echo "Aucun CMakeLists.txt dans ce répertoire."
		exit 5
	fi
else
	echo "Nous n'avons aucun argument."
	echo "Donnez le nom d'un toolchain en argument."
	exit
fi
