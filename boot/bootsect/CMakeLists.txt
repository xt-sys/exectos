# XT Boot Sector
PROJECT(BOOTSECT)

# Compile boot sectors
compile_bootsector(mbrboot ${BOOTSECT_SOURCE_DIR}/${ARCH}/mbrboot.S 0x7C00 Start)
