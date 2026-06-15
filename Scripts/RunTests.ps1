# Evolution Arena - build + headless automation test harness.
#
# Builds the editor target, runs the automation suite headless (including the
# battle-determinism release gate), and exits non-zero if anything fails - so it
# is usable both locally and from CI. See CLAUDE.md "Tech stack & commands".
#
# Usage:
#   ./Scripts/RunTests.ps1                       # build + run all EvolutionArena tests
#   ./Scripts/RunTests.ps1 -SkipBuild            # tests only (reuse current binaries)
#   ./Scripts/RunTests.ps1 -Engine "C:\UE_5.6"   # point at a different engine install
#   ./Scripts/RunTests.ps1 -TestFilter EvolutionArena.Battle   # run a subset
#
# NOTE: the engine path defaults to D:\UE_5.6 because the launcher registry entry for
# "5.6" is stale on this machine (points at C:\UE_5.6). Override with -Engine as needed.
# Keep this file ASCII-only: Windows PowerShell 5.1 reads .ps1 as ANSI, so non-ASCII
# punctuation (em dashes, curly quotes) corrupts string literals.

[CmdletBinding()]
param(
	[string]$Engine = "D:\UE_5.6",
	[ValidateSet("Development", "DebugGame", "Shipping", "Test")]
	[string]$Config = "Development",
	[string]$TestFilter = "EvolutionArena",
	[switch]$SkipBuild
)

$ErrorActionPreference = "Stop"

$ProjectRoot = Split-Path -Parent $PSScriptRoot
$Project     = Join-Path $ProjectRoot "EvolutionArena.uproject"
$BuildBat    = Join-Path $Engine "Engine\Build\BatchFiles\Build.bat"
$EditorCmd   = Join-Path $Engine "Engine\Binaries\Win64\UnrealEditor-Cmd.exe"

# ---- Preconditions -------------------------------------------------------------
if (-not (Test-Path $Project))   { Write-Error "Project not found: $Project"; exit 2 }
if (-not (Test-Path $BuildBat))  { Write-Error "Build.bat not found at $BuildBat. Pass -Engine with the UE root."; exit 2 }
if (-not (Test-Path $EditorCmd)) { Write-Error "UnrealEditor-Cmd not found at $EditorCmd. Pass -Engine with the UE root."; exit 2 }

# ---- 1) Build ------------------------------------------------------------------
if (-not $SkipBuild) {
	Write-Host "== Building EvolutionArenaEditor ($Config, Win64) =="
	& $BuildBat EvolutionArenaEditor Win64 $Config -project="$Project" -waitmutex -NoHotReload
	if ($LASTEXITCODE -ne 0) { Write-Error "Build failed (exit $LASTEXITCODE)"; exit 1 }
}

# ---- 2) Run automation tests headless ------------------------------------------
$LogDir    = Join-Path $ProjectRoot "Saved\Logs"
$ReportDir = Join-Path $ProjectRoot "Saved\TestReports"
$Log       = Join-Path $LogDir "CIAutomation.log"
New-Item -ItemType Directory -Force -Path $LogDir, $ReportDir | Out-Null

Write-Host "== Running automation tests: '$TestFilter' =="
& $EditorCmd "$Project" `
	-ExecCmds="Automation RunTests $TestFilter; Quit" `
	-unattended -nopause -nullrhi -nosplash -nosound `
	-abslog="$Log" -ReportExportPath="$ReportDir" | Out-Null

# ---- 3) Parse results ----------------------------------------------------------
if (-not (Test-Path $Log)) { Write-Error "No automation log produced at $Log"; exit 1 }

$passed = (Select-String -Path $Log -Pattern 'Result=\{Success\}').Count
$failed = (Select-String -Path $Log -Pattern 'Result=\{Fail').Count
$ran    = $passed + $failed

Write-Host ""
Write-Host "== Test summary: $passed passed, $failed failed (of $ran) =="

if ($ran -eq 0) { Write-Error "No tests ran. Check the filter '$TestFilter'."; exit 1 }
if ($failed -gt 0) {
	Write-Host "Failures:"
	Select-String -Path $Log -Pattern 'Result=\{Fail' | ForEach-Object { Write-Host "  $($_.Line)" }
	exit 1
}

Write-Host "All tests passed. Report: $ReportDir"
exit 0
