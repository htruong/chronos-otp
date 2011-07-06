#!/bin/bash

function build_freq {

	echo "Building firmware for frequency ${1}..."
	sed "s/TEMPLATE_FREQUENCY/${1}/g" config.h.template > config.h
	make clean  &> /dev/null
	make &> build-report.txt
	if (($?))
	then
		echo "Firmware build for frequency ${1} failed!"
	else
		cp build/eZChronos.txt prebuilt/chronos-otp-${1}.txt
		cp build/eZChronos.elf prebuilt/chronos-otp-${1}.elf
	fi
	make clean &> /dev/null
#	rm config.h

}

mv config.h config.h.dev

build_freq 433

build_freq 902

build_freq 868

mv config.h.dev config.h

