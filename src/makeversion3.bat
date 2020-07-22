@echo off
arm-agb-elf-gcc -marm -mthumb-interwork -Wall -o svc_v2.elf main_version2.cpp libgbfs.c
gbfs duhast.gbfs duhast.pcm
arm-agb-elf-objcopy -O binary svc_v2.elf svc_v2.bin
gbafix svc_v2.gba -tSVC_V2 -cSVCP -mLD -r1
padbin 256 svc_v2.bin
copy /b svc_v2.bin+duhast.gbfs svc_v2.gba
del svc_v2.elf
del svc_v2.bin
:end
