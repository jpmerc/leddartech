leddartech
==========

In order to be able to connect to the leddar device with the actual Ros package, follow these simple steps :

	1) sudo useradd -G plugdev USERNAME
	2) sudo cp 10-leddartech-rules /etc/udev/rules.d/
	3) sudo udevadm trigger
