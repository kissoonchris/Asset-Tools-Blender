[CmdletBinding()]
param(
    [string]$GameDir,
    [string]$ToolkitDir,
    [switch]$SkipGenerate,
    [switch]$Run
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$XboxRecompRevision = "4d337526dc4ab892483aad0246aee9d1c33d1b81"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$RecompDir = Join-Path $ProjectRoot "recomp"
$BuildDir = Join-Path $RecompDir "build"
$Driver = Join-Path $ProjectRoot "scripts\jsrf_xboxrecomp.py"

if ([string]::IsNullOrWhiteSpace($GameDir)) {
    $GameDir = Join-Path $ProjectRoot "game"
}
$GameDir = [System.IO.Path]::GetFullPath($GameDir)

if ([string]::IsNullOrWhiteSpace($ToolkitDir)) {
    $ToolkitDir = Join-Path $ProjectRoot ".tools\xboxrecomp"
}
$ToolkitDir = [System.IO.Path]::GetFullPath($ToolkitDir)

$XbePath = Join-Path $GameDir "default.xbe"
if (-not (Test-Path -LiteralPath $XbePath -PathType Leaf)) {
    throw "JSRF default.xbe was not found at '$XbePath'. Put the extracted retail game in '$GameDir' or pass -GameDir <folder>."
}

function Invoke-Checked {
    param(
        [Parameter(Mandatory=$true)][string]$FilePath,
        [Parameter(ValueFromRemainingArguments=$true)][string[]]$Arguments
    )
    & $FilePath @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "Command failed with exit code ${LASTEXITCODE}: $FilePath $($Arguments -join ' ')"
    }
}

function Resolve-PythonLauncher {
    $py = Get-Command py -ErrorAction SilentlyContinue
    if ($py) {
        return @($py.Source, "-3")
    }
    $python = Get-Command python -ErrorAction SilentlyContinue
    if ($python) {
        return @($python.Source)
    }
    throw "Python 3 was not found. Install Python 3.10 or newer and rerun this script."
}

$PythonLauncher = Resolve-PythonLauncher
$PythonExe = $PythonLauncher[0]
$PythonPrefix = @()
if ($PythonLauncher.Count -gt 1) {
    $PythonPrefix = $PythonLauncher[1..($PythonLauncher.Count - 1)]
}
$PythonCommandForDriver = (($PythonLauncher | ForEach-Object {
    if ($_ -match '\s') { '"' + $_ + '"' } else { $_ }
}) -join ' ')

if (-not (Test-Path -LiteralPath $ToolkitDir -PathType Container)) {
    $ToolkitParent = Split-Path -Parent $ToolkitDir
    New-Item -ItemType Directory -Force -Path $ToolkitParent | Out-Null
    Invoke-Checked git clone https://github.com/sp00nznet/xboxrecomp.git $ToolkitDir
}

if (-not (Test-Path -LiteralPath (Join-Path $ToolkitDir ".git") -PathType Container)) {
    throw "ToolkitDir '$ToolkitDir' exists but is not a Git checkout of xboxrecomp."
}

Invoke-Checked git -C $ToolkitDir fetch --depth 1 origin $XboxRecompRevision
Invoke-Checked git -C $ToolkitDir checkout --detach $XboxRecompRevision

# xboxrecomp's Python disassembly pipeline requires Capstone.
& $PythonExe @PythonPrefix -m pip install --disable-pip-version-check capstone
if ($LASTEXITCODE -ne 0) {
    throw "Failed to install/verify the Python capstone dependency."
}

# Validate the exact retail XBE before generating any native code.
& $PythonExe @PythonPrefix $Driver validate $XbePath
if ($LASTEXITCODE -ne 0) {
    throw "JSRF retail XBE validation failed."
}

if (-not $SkipGenerate) {
    $GenerateArgs = @(
        "generate",
        "--toolkit", $ToolkitDir,
        "--game-dir", $GameDir,
        "--project-root", $ProjectRoot,
        "--python", $PythonCommandForDriver
    )
    & $PythonExe @PythonPrefix $Driver @GenerateArgs
    if ($LASTEXITCODE -ne 0) {
        throw "xboxrecomp generation failed."
    }
}

New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null
Invoke-Checked cmake -S $RecompDir -B $BuildDir "-DXBOXRECOMP_DIR=$ToolkitDir"
Invoke-Checked cmake --build $BuildDir --config Release

$ExeCandidates = @(
    (Join-Path $BuildDir "Release\jsrf_recomp.exe"),
    (Join-Path $BuildDir "jsrf_recomp.exe")
)
$Exe = $null
foreach ($Candidate in $ExeCandidates) {
    if (Test-Path -LiteralPath $Candidate -PathType Leaf) {
        $Exe = $Candidate
        break
    }
}
if (-not $Exe) {
    throw "The build completed but jsrf_recomp.exe was not found under '$BuildDir'."
}

Write-Host "Built: $Exe"

if ($Run) {
    $env:JSRF_GAME_DIR = $GameDir
    if ([string]::IsNullOrWhiteSpace($env:RECOMP_WATCHDOG_SECS)) {
        $env:RECOMP_WATCHDOG_SECS = "20"
    }
    & $Exe
    exit $LASTEXITCODE
}
