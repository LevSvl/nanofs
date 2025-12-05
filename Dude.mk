# Programmer
DUDE = avrdude
DUDECONF ?= avrdude.conf

DUDEOPTS += -v -V -patmega328p
DUDEOPTS += -carduino -b57600 -D -P${COM}
