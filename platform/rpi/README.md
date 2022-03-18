## BCM2835 - Raspberry Pi Zero

### usbboot
USB Boot will flash the firmware and kernel via usb to RAM and will
immediately boot the kernel. Repowering the board will lose the
flashed firmware/kernel, so you need to use SDCard boot for this.

#### Run configuration for CLion

Add a "before launch" on the executable you want to upload
  - External Tool
    - Name: `usbboot`
    - Executable: `$CMakeCurrentGenerationDir$/platform/bcm2835/usbboot/usbboot`
    - Arguments: `-d $ProjectFileDir$/platform/bcm2835/boot -i $CMakeCurrentProductFile$.bin`
    - Working Directory: `$ProjectFileDir$`

### How to upload
While the usb boot code is running, you will not be able to upload code. You need to
reset the board by connecting the two `RUN` pins usually found next to the GPIO.

Once reset you should be able to run the executable which will in turn execute the
usbboot tool. After it's done the actual binary will attempt to execute. This should
fail as it's an ARM executable (this is completely expected). The kernel should now
be running on the Raspberry Pi!
