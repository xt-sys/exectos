# Architecture specific QEMU settings
if(ARCH STREQUAL i686)
	set(QEMU_COMMAND "qemu-system-i386")
elseif(ARCH STREQUAL amd64)
	set(QEMU_COMMAND "qemu-system-x86_64")
endif()

# This target creates a disk image
add_custom_target(diskimg
                  DEPENDS install
                  COMMAND sh -c "dd if=/dev/zero of=${EXECTOS_BINARY_DIR}/output/disk.img bs=512 count=${PROJECT_DISK_IMAGE_BLOCKS} 2>/dev/null 1>/dev/null"
                  COMMAND parted ${EXECTOS_BINARY_DIR}/output/disk.img -s -a minimal mklabel gpt
                  COMMAND parted ${EXECTOS_BINARY_DIR}/output/disk.img -s -a minimal mkpart EFI FAT32 2048s ${PROJECT_PART_IMAGE_BLOCKS}s
                  COMMAND parted ${EXECTOS_BINARY_DIR}/output/disk.img -s -a minimal toggle 1 boot
                  COMMAND sh -c "dd if=/dev/zero of=${EXECTOS_BINARY_DIR}/output/part.img bs=512 count=${PROJECT_PART_IMAGE_BLOCKS} 2>/dev/null 1>/dev/null"
                  COMMAND mformat -i ${EXECTOS_BINARY_DIR}/output/part.img -h32 -t32 -n64 -L32
                  COMMAND sh -c "mcopy -s -i ${EXECTOS_BINARY_DIR}/output/part.img ${EXECTOS_BINARY_DIR}/output/binaries/* ::"
                  COMMAND sh -c "dd if=${EXECTOS_BINARY_DIR}/output/part.img of=${EXECTOS_BINARY_DIR}/output/disk.img bs=512 count=${PROJECT_PART_IMAGE_BLOCKS} seek=2048 conv=notrunc 2>/dev/null 1>/dev/null"
                  COMMAND rm ${EXECTOS_BINARY_DIR}/output/part.img
                  VERBATIM)

# This target starts up a BOCHS+OVMF virtual machine
add_custom_target(bochsvm
                  DEPENDS diskimg
                  COMMAND bochs -f ../sdk/firmware/bochsrc_${ARCH}.cfg -q -unlock
                  VERBATIM USES_TERMINAL)

# This target starts up a QEMU+OVMF virtual machine using KVM accelerator
add_custom_target(testkvm
                  DEPENDS diskimg
                  COMMAND ${QEMU_COMMAND} -name "ExectOS-${ARCH}-KVM" -machine type=q35,kernel_irqchip=on,accel=kvm,mem-merge=off,vmport=off -enable-kvm -cpu host,-hypervisor,+topoext
                                          -smp 2,sockets=1,cores=1,threads=2 -m 4G -overcommit mem-lock=off -rtc clock=host,base=localtime,driftfix=none
                                          -drive file=${EXECTOS_SOURCE_DIR}/sdk/firmware/ovmf_code_${ARCH}.fd,if=pflash,format=raw,unit=0,readonly=on
                                          -drive file=${EXECTOS_SOURCE_DIR}/sdk/firmware/ovmf_vars_${ARCH}.fd,if=pflash,format=raw,unit=1
                                          -hda ${EXECTOS_BINARY_DIR}/output/disk.img
                                          -boot menu=on -d int -M smm=off -no-reboot -no-shutdown -serial stdio
                  VERBATIM USES_TERMINAL)

# This target starts up a QEMU+OVMF virtual machine using TCG accelerator
add_custom_target(testtcg
                  DEPENDS diskimg
                  COMMAND ${QEMU_COMMAND} -name "ExectOS-${ARCH}-TCG" -machine type=q35,accel=tcg -cpu max,-hypervisor
                                          -smp 2,sockets=1,cores=1,threads=2 -m 4G -overcommit mem-lock=off -rtc clock=host,base=localtime,driftfix=none
                                          -drive file=${EXECTOS_SOURCE_DIR}/sdk/firmware/ovmf_code_${ARCH}.fd,if=pflash,format=raw,unit=0,readonly=on
                                          -drive file=${EXECTOS_SOURCE_DIR}/sdk/firmware/ovmf_vars_${ARCH}.fd,if=pflash,format=raw,unit=1
                                          -hda ${EXECTOS_BINARY_DIR}/output/disk.img
                                          -boot menu=on -d int -M smm=off -no-reboot -no-shutdown -serial stdio
                  VERBATIM USES_TERMINAL)
