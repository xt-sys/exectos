# Architecture specific QEMU settings
if(ARCH STREQUAL i686)
	set(QEMU_COMMAND "qemu-system-i386")
elseif(ARCH STREQUAL amd64)
	set(QEMU_COMMAND "qemu-system-x86_64")
endif()

# This target creates a disk image
add_custom_target(diskimg
                  DEPENDS install
                  COMMAND diskimg -c ${EXECTOS_BINARY_DIR}/output/binaries -f 16 -o ${EXECTOS_BINARY_DIR}/output/disk.img -s ${PROJECT_DISK_IMAGE_SIZE}
                  -m ${EXECTOS_BINARY_DIR}/boot/bootsect/mbrboot.bin
                  VERBATIM)

find_program(BOCHS_EMULATOR bochs)
if(BOCHS_EMULATOR)
    # This target starts up a BOCHS+BIOS virtual machine
    add_custom_target(bochsvm
                      DEPENDS diskimg
                      COMMAND bochs -f ../sdk/firmware/bochsrc_${ARCH}.cfg -q -unlock
                      VERBATIM USES_TERMINAL)
endif()

find_program(QEMU_EMULATOR ${QEMU_COMMAND})
if(QEMU_EMULATOR)
    # This target starts up a QEMU+OVMF virtual machine using KVM accelerator
    add_custom_target(testefikvm
                      DEPENDS install
                      COMMAND ${QEMU_COMMAND} -name "ExectOS-${ARCH}-EFI-KVM" -machine type=q35,kernel_irqchip=on,accel="kvm:whpx",mem-merge=off,vmport=off -enable-kvm -cpu host,-hypervisor,+topoext
                                              -smp 2,sockets=1,cores=1,threads=2 -m 4G -overcommit mem-lock=off -rtc clock=host,base=localtime,driftfix=none
                                              -drive file=${EXECTOS_SOURCE_DIR}/sdk/firmware/ovmf_code_${ARCH}.fd,if=pflash,format=raw,unit=0,readonly=on
                                              -drive file=${EXECTOS_SOURCE_DIR}/sdk/firmware/ovmf_vars_${ARCH}.fd,if=pflash,format=raw,unit=1
                                              -hda fat:rw:${EXECTOS_BINARY_DIR}/output/binaries
                                              -boot menu=on -d int -M smm=off -no-reboot -no-shutdown -serial stdio
                      VERBATIM USES_TERMINAL)

    # This target starts up a QEMU+OVMF virtual machine using TCG accelerator
    add_custom_target(testefitcg
                      DEPENDS install
                      COMMAND ${QEMU_COMMAND} -name "ExectOS-${ARCH}-EFI-TCG" -machine type=q35,accel=tcg -cpu max,-hypervisor
                                              -smp 2,sockets=1,cores=1,threads=2 -m 4G -overcommit mem-lock=off -rtc clock=host,base=localtime,driftfix=none
                                              -drive file=${EXECTOS_SOURCE_DIR}/sdk/firmware/ovmf_code_${ARCH}.fd,if=pflash,format=raw,unit=0,readonly=on
                                              -drive file=${EXECTOS_SOURCE_DIR}/sdk/firmware/ovmf_vars_${ARCH}.fd,if=pflash,format=raw,unit=1
                                              -hda fat:rw:${EXECTOS_BINARY_DIR}/output/binaries
                                              -boot menu=on -d int -no-reboot -no-shutdown -serial stdio
                      VERBATIM USES_TERMINAL)

    # This target starts up a QEMU+SEABIOS virtual machine using KVM accelerator
    add_custom_target(testkvm
                      DEPENDS diskimg
                      COMMAND ${QEMU_COMMAND} -name "ExectOS-${ARCH}-BIOS-KVM" -machine type=q35,kernel_irqchip=on,accel="kvm:whpx",mem-merge=off,vmport=off -enable-kvm -cpu host,-hypervisor,+topoext
                                              -smp 2,sockets=1,cores=1,threads=2 -m 4G -overcommit mem-lock=off -rtc clock=host,base=localtime,driftfix=none
                                              -hda ${EXECTOS_BINARY_DIR}/output/disk.img
                                              -boot menu=on -d int -no-reboot -no-shutdown -serial stdio
                      VERBATIM USES_TERMINAL)

    # This target starts up a QEMU+SEABIOS virtual machine using TCG accelerator
    add_custom_target(testtcg
                      DEPENDS diskimg
                      COMMAND ${QEMU_COMMAND} -name "ExectOS-${ARCH}-BIOS-TCG" -machine type=q35,accel=tcg -cpu max,-hypervisor
                                              -smp 2,sockets=1,cores=1,threads=2 -m 4G -overcommit mem-lock=off -rtc clock=host,base=localtime,driftfix=none
                                              -hda ${EXECTOS_BINARY_DIR}/output/disk.img
                                              -boot menu=on -d int -no-reboot -no-shutdown -serial stdio
                      VERBATIM USES_TERMINAL)
endif()
