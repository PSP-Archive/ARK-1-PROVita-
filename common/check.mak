# No GAME set...
ifndef GAME
$(error Please define which game you are about to exploit, eg. GAME=NPEH00020)
endif

# No FIRMWARE set...
ifndef FIRMWARE
$(error Define Vita firmware version, eg. FIRMWARE=161)
endif

# No PSID set...
ifndef RELEASE
ifndef PSID
$(error Define PSID in hexstring, eg. PSID=0102030405060708090A0B0C0D0E0F10)
endif
endif
