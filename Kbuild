obj-m += mydriver1.o

dtb-y += mydevice1.dtbo
dtb-y += mydevice2.dtbo

%.dtbo: %.dts
	dtc -O dtb -o $@ -@ $<

