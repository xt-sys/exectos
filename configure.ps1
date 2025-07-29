# PROJECT:     ExectOS
# LICENSE:     See the COPYING.md in the top level directory
# FILE:        configure.ps1
# DESCRIPTION: Project configuration script for preparing the build environment
# DEVELOPERS:  Aiken Harris <harraiken91@gmail.com>


# Check XTchain
if (-not $env:XTCVER) {
    Write-Host "XTChain not detected or corrupted!"
    exit 1
}

# Set target architecture
if ($env:TARGET) {
    $ARCH = $env:TARGET
} else {
    $ARCH = "amd64"
}

# Set target build type
if (-not $env:BUILD_TYPE) {
    $env:BUILD_TYPE = "DEBUG"
}

# Set variables
$EXECTOS_SOURCE_DIR = (Get-Location).Path
$EXECTOS_BINARY_DIR = "build-$($ARCH)-$($env:BUILD_TYPE.ToLower())"

# Create directories if needed
if ($EXECTOS_SOURCE_DIR -eq (Get-Location).Path) {
    Write-Host "Creating directories in $EXECTOS_BINARY_DIR"
    New-Item -ItemType Directory -Path $EXECTOS_BINARY_DIR -Force | Out-Null
    Set-Location -Path $EXECTOS_BINARY_DIR
}

# Delete old cache
Remove-Item -Path "CMakeCache.txt" -ErrorAction SilentlyContinue
Remove-Item -Path "host-tools/CMakeCache.txt" -ErrorAction SilentlyContinue

# Configure project using CMake
& cmake -G Ninja -DARCH:STRING=$($ARCH) -DBUILD_TYPE:STRING=$($env:BUILD_TYPE) $EXECTOS_SOURCE_DIR

# Check if configuration succeeded
if ($LASTEXITCODE -ne 0) {
    Write-Host "Configure script failed."
    exit 1
} else {
    "$($ARCH)" | Out-File -Encoding ASCII -NoNewline build.arch
    Write-Host "Configure script completed. Enter '$EXECTOS_BINARY_DIR' directory and execute 'xbuild' to build ExectOS."
}
