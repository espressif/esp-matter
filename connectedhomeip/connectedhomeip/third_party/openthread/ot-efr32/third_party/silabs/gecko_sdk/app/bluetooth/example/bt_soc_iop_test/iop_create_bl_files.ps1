################################################################################
# Bluetooth - SoC Interoperability Test GBL generator
#
# This script generates the GBL files needed for SoC Interoperability Test.
# These files are used for the OTA-DFU tests.
#
# Prerequisites
# - Windows PowerShell.
# - PATH_SCMD and PATH_GCCARM environment variables to be set. See readme.md
# for more information.
# - The Bluetooth - SoC Interoperability Test example generated either with
# the "Link SDK and copy project sources" or the "Copy contents" option.
# - The project is built with success.
#
# Usage
# - Run this script.
# - Copy the generated gbl files onto the storage of the phone.
# - In the EFR Connect app, open the Interoperability Test demo tile.
# - Start the test. When prompted to choose a gbl file for OTA-DFU, select the
# ota-dfu_ack.gbl file. When prompted again, select ota-dfu_non_ack.gbl.
#
# For a more detailed guide see the readme.md file of the example.
################################################################################

$App1 = "ota-dfu_non_ack"
$App2 = "ota-dfu_ack"
$DeviceName1 = "IOP_Test_1"
$DeviceName2 = "IOP_Test_2"

$PathGbl = Join-Path $PSScriptRoot 'output_gbl'
$OtaApploName = "apploader"
$OtaAppliName = "application"
$UartdfuFullName = "full"

################################################################################
# Functions
################################################################################
# Checks the presence of a string in a binary.
function Search-Str-In-Bin {
    Param
    (
        [Parameter(Mandatory=$true, Position=0)]
        [string] $Str,
        [Parameter(Mandatory=$true, Position=1)]
        [string] $File
    )

    Write-Output "Searching for $Str in $File"
    $result = Select-String -Path $File -Pattern $Str -CaseSensitive
    if ($null -eq $result) {
        Write-Error "Error: $Str cannot be found in $File"
        Write-Output "Press any key to continue..."
        [void][Console]::ReadKey()
        Exit(1)
    } else {
        Write-Output "Found: $result"
    }
}

################################################################################
# Entry point
################################################################################
# Locate the out file
$PathOut = (Get-ChildItem -Path $PSScriptRoot -Include ('*.axf', '*.out') -Recurse | ForEach-Object { $_.FullName })
if ($null -eq $PathOut) {
    Write-Output 'Error: neither .axf nor .out file was found.'
    Write-Output 'Was the project compiled and linked successfully?'
    Write-Output 'Press any key to continue...'
    [void][Console]::ReadKey()
    Exit(1)
}

# Locate the create_bl_files.bat script
$CreateBlFiles = Join-Path $PSScriptRoot 'create_bl_files.bat'
if (-not (Test-Path -Path $CreateBlFiles)) {
    Write-Output "Error: $CreateBlFiles was not found."
    Write-Output "Was the project generated with the copy option?"
    Write-Output 'Press any key to continue...'
    [void][Console]::ReadKey()
    Exit(1)
}

Write-Output "**********************************************************************"
Write-Output "Generating gbl file for the default application."
Write-Output "**********************************************************************"
Write-Output ""

Start-Process -FilePath $CreateBlFiles -Wait
Move-Item -Path (Join-Path $PathGbl "$OtaAppliName.gbl") -Destination (Join-Path $PathGbl "$App1.gbl") -Force

# Check device name
Search-Str-In-Bin -Str $DeviceName1 -File (Join-Path $PathGbl "$App1.gbl")

Write-Output "**********************************************************************"
Write-Output "Generating gbl file for the updated application."
Write-Output "**********************************************************************"
Write-Output ""

# Make a copy of the out file
Copy-Item -Path $PathOut -Destination ($PathOut + "_backup")

# Change the device name by manipulating the out file
(Get-Content $PathOut -Raw -ReadCount 0).replace($DeviceName1, $DeviceName2) | Set-Content $PathOut -NoNewline

Start-Process -FilePath $CreateBlFiles -Wait
Move-Item -Path (Join-Path $PathGbl "$OtaAppliName.gbl") -Destination (Join-Path $PathGbl "$App2.gbl") -Force

# Clean up
# Restore the original out file
Move-Item -Path ($PathOut + "_backup") -Destination $PathOut -Force
# Only keep the necessary gbl files
Remove-Item (Join-Path $PathGbl "$OtaApploName*")
Remove-Item (Join-Path $PathGbl "$OtaAppliName*")
Remove-Item (Join-Path $PathGbl "$UartdfuFullName*")

# Check device name
Search-Str-In-Bin -Str $DeviceName2 -File (Join-Path $PathGbl "$App2.gbl")
Exit(0)
