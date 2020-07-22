@echo off
arm-agb-elf-gcc -marm -mthumb-interwork -Wall -o svc_project.elf main.cpp libgbfs.c
objcopy -O binary svc_project.elf svc_project.bin
gbafix svc_project.bin -tSVC_PROJECT -cSVCP -mLD -r1
padbin 256 svc_project.bin
gbfs music.gbfs bomb.pcm duhast.pcm dragula.pcm hmetal.pcm
copy /b svc_project.bin+music.gbfs svc_project.gba
del svc_project.elf
del svc_project.bin
:end
