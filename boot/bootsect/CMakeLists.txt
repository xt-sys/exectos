# XT Boot Sector
PROJECT(BOOTSECT)

add_definitions("-DARCH_ESP_SOURCE=\\\"${ARCH}/cpu.S\\\"")

# Compile boot sectors
compile_bootsector(mbrboot ${BOOTSECT_SOURCE_DIR}/mbrboot.S 0x7C00 Start)
compile_bootsector(espboot ${BOOTSECT_SOURCE_DIR}/espboot.S 0x7C00 Start)
