obj-m += mydriver1.o

dtb-y += mydevice1.dtbo
dtb-y += mydevice2.1.dtbo
dtb-y += mydevice2.2.dtbo
dtb-y += mydevice3.dtbo
#dtb-y += mydevice4.1.dtbo
#dtb-y += mydevice4.2.dtbo

%.dtbo: %.dts
	dtc -O dtb -o $@ -@ $<

