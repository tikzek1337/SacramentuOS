# SacramentuOS Windows 11 runner
# Default mode uses QEMU -kernel and bypasses GRUB/ISO.
# This fixes the common freeze at: "Booting from DVD/CD...".
#
# Usage from PowerShell inside the project folder:
#   powershell -ExecutionPolicy Bypass -File .\scripts\run_qemu_windows.ps1
# ISO mode, only after making build\SacramentuOS.iso:
#   powershell -ExecutionPolicy Bypass -File .\scripts\run_qemu_windows.ps1 -Mode iso

param(
    [ValidateSet("kernel", "iso")]
    [string]$Mode = "kernel",
    [string]$QemuPath = ""
)

$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)

function Resolve-Qemu {
    param([string]$ManualPath)

    if ($ManualPath -and (Test-Path $ManualPath)) {
        return $ManualPath
    }

    $cmd = Get-Command qemu-system-i386.exe -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }

    $common = @(
        "C:\Program Files\qemu\qemu-system-i386.exe",
        "C:\Program Files (x86)\qemu\qemu-system-i386.exe",
        "$env:ProgramFiles\qemu\qemu-system-i386.exe"
    )

    foreach ($path in $common) {
        if (Test-Path $path) { return $path }
    }

    throw "qemu-system-i386.exe not found. Install QEMU for Windows or pass -QemuPath 'C:\path\qemu-system-i386.exe'."
}

$Qemu = Resolve-Qemu $QemuPath

if ($Mode -eq "kernel") {
    $Kernel = Join-Path $ProjectRoot "prebuilt\sacramentuos.kernel"
    if (!(Test-Path $Kernel)) {
        throw "Kernel not found: $Kernel"
    }
    Write-Host "Starting SacramentuOS directly from kernel: $Kernel"
    & $Qemu -machine pc -m 256M -kernel $Kernel -no-reboot -no-shutdown
    exit $LASTEXITCODE
}

$Iso = Join-Path $ProjectRoot "build\SacramentuOS.iso"
if (!(Test-Path $Iso)) {
    throw "ISO not found: $Iso. Build it first in WSL with: make iso-prebuilt"
}

Write-Host "Starting SacramentuOS from ISO: $Iso"
& $Qemu -machine pc -m 256M -boot d -cdrom $Iso -no-reboot -no-shutdown
exit $LASTEXITCODE
