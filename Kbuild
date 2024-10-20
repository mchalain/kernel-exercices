obj-m += mydriver1.o

dtb-y += mydevice1.dtbo

%.dtbo: %.dts
	dtc -O dtb -o $@ -@ $<

