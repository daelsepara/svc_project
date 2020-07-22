@echo off
cls
arm-agb-elf-gcc -marm -o svc_v2.elf main_version2.cpp
rem arm-agb-elf-gcc -marm -mthumb-interwork -Wall -o svc_v2.elf main_version2.cpp
rem libgbfs.c
arm-agb-elf-objcopy -O binary svc_v2.elf svc_v2.gba
gbafix svc_v2.gba -tSVC_V2 -cSVCP -mLD -r2
del svc_v2.elf
:end
