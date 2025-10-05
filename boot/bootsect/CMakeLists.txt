# XT Boot Sector
PROJECT(BOOTSECT)

# Compile boot sectors
compile_bootsector(mbrboot ${BOOTSECT_SOURCE_DIR}/mbrboot.S 0x7C00 Start)
compile_bootsector(espboot ${BOOTSECT_SOURCE_DIR}/espboot.S 0x7C00 Start)
