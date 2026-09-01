[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string] $Executable,

    [Parameter()]
    [ValidateRange(2, 8192)]
    [int] $FileSizeMiB = 5,

    [Parameter()]
    [ValidateRange(1, 4096)]
    [Alias('WindowSizeMiB')]
    [int] $RamBudgetMiB = 2,

    [Parameter()]
    [ValidateRange(5, 3600)]
    [int] $TimeoutSeconds = 20,

    [Parameter()]
    [ValidateRange(0, 1048576)]
    [int] $MaximumPeakWorkingSetMiB = 0,

    [Parameter()]
    [switch] $KeepArtifacts
)

$ErrorActionPreference = 'Stop'

function Invoke-TestProcess {
    param(
        [Parameter(Mandatory = $true)]
        [string] $FilePath,

        [Parameter(Mandatory = $true)]
        [string[]] $ArgumentList,

        [Parameter(Mandatory = $true)]
        [string] $WorkingDirectory,

        [Parameter(Mandatory = $true)]
        [int] $TimeoutSeconds
    )

    $startInfo = [System.Diagnostics.ProcessStartInfo]::new()
    $startInfo.FileName = $FilePath
    $startInfo.WorkingDirectory = $WorkingDirectory
    $startInfo.UseShellExecute = $false
    $startInfo.CreateNoWindow = $true
    $startInfo.RedirectStandardOutput = $true
    $startInfo.RedirectStandardError = $true
    foreach ($argument in $ArgumentList) {
        [void] $startInfo.ArgumentList.Add($argument)
    }

    $process = [System.Diagnostics.Process]::new()
    $process.StartInfo = $startInfo
    if (-not $process.Start()) {
        throw "Could not start '$FilePath'."
    }

    $stdoutTask = $process.StandardOutput.ReadToEndAsync()
    $stderrTask = $process.StandardError.ReadToEndAsync()
    $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
    [int64] $peakWorkingSet64 = 0
    while (-not $process.WaitForExit(50)) {
        $process.Refresh()
        if ($process.WorkingSet64 -gt $peakWorkingSet64) {
            $peakWorkingSet64 = $process.WorkingSet64
        }
        if ($stopwatch.Elapsed.TotalSeconds -ge $TimeoutSeconds) {
            break
        }
    }
    if (-not $process.HasExited) {
        $process.Kill($true)
        $process.WaitForExit()
        $stdout = $stdoutTask.GetAwaiter().GetResult()
        $stderr = $stderrTask.GetAwaiter().GetResult()
        throw "Process timed out after $TimeoutSeconds seconds: $FilePath $($ArgumentList -join ' ')`nSTDOUT:`n$stdout`nSTDERR:`n$stderr"
    }

    try {
        $process.Refresh()
        if ($process.PeakWorkingSet64 -gt $peakWorkingSet64) {
            $peakWorkingSet64 = $process.PeakWorkingSet64
        }
    } catch {
        # A very short-lived process can release its performance counters
        # before PowerShell samples them. Leave the value at zero in that case.
    }

    $stdout = $stdoutTask.GetAwaiter().GetResult()
    $stderr = $stderrTask.GetAwaiter().GetResult()
    [pscustomobject]@{
        ExitCode = $process.ExitCode
        Stdout = $stdout
        Stderr = $stderr
        PeakWorkingSet64 = $peakWorkingSet64
    }
}

function New-DeterministicFile {
    param(
        [Parameter(Mandatory = $true)]
        [string] $Path,

        [Parameter(Mandatory = $true)]
        [int64] $Length
    )

    $buffer = [byte[]]::new(1MB)
    $stream = [System.IO.File]::Open($Path, [System.IO.FileMode]::CreateNew, [System.IO.FileAccess]::Write, [System.IO.FileShare]::None)
    try {
        [int64] $remaining = $Length
        [int64] $blockNumber = 0
        while ($remaining -gt 0) {
            [System.Array]::Fill($buffer, [byte] ($blockNumber % 251))
            [System.BitConverter]::GetBytes($blockNumber).CopyTo($buffer, 0)
            $count = [int] [Math]::Min($remaining, $buffer.Length)
            $stream.Write($buffer, 0, $count)
            $remaining -= $count
            $blockNumber++
        }
    } finally {
        $stream.Dispose()
    }
}

$executableFullPath = [System.IO.Path]::GetFullPath($Executable)
if (-not (Test-Path -LiteralPath $executableFullPath -PathType Leaf)) {
    throw "Executable does not exist: $executableFullPath"
}
if ($RamBudgetMiB -ge $FileSizeMiB) {
    throw 'RamBudgetMiB must be smaller than FileSizeMiB to exercise oversized-file streaming.'
}

$testRoot = Join-Path ([System.IO.Path]::GetTempPath()) ('zpaqfranz-w-ramdisk-' + [guid]::NewGuid().ToString('N'))
$sourceDirectory = Join-Path $testRoot 'source'
$restoreDirectory = Join-Path $testRoot 'restore'
$archivePath = Join-Path $testRoot 'streaming.zpaq'
$sourcePath = Join-Path $sourceDirectory 'payload.bin'
$smallSourcePath = Join-Path $sourceDirectory 'small.bin'

try {
    [void] (New-Item -ItemType Directory -Path $sourceDirectory, $restoreDirectory -Force)
    New-DeterministicFile -Path $sourcePath -Length ([int64] $FileSizeMiB * 1MB)
    New-DeterministicFile -Path $smallSourcePath -Length 512KB
    $sourceHash = (Get-FileHash -LiteralPath $sourcePath -Algorithm SHA256).Hash
    $smallSourceHash = (Get-FileHash -LiteralPath $smallSourcePath -Algorithm SHA256).Hash

    $addResult = Invoke-TestProcess -FilePath $executableFullPath -WorkingDirectory $sourceDirectory -TimeoutSeconds 120 -ArgumentList @(
        'a', $archivePath, 'payload.bin', 'small.bin', '-m0', '-nojit', '-noeta', '-nocolor'
    )
    if ($addResult.ExitCode -ne 0) {
        throw "Archive creation failed with exit code $($addResult.ExitCode).`nSTDOUT:`n$($addResult.Stdout)`nSTDERR:`n$($addResult.Stderr)"
    }

    $extractResult = Invoke-TestProcess -FilePath $executableFullPath -WorkingDirectory $testRoot -TimeoutSeconds $TimeoutSeconds -ArgumentList @(
        'w', $archivePath,
        '-to', $restoreDirectory,
        '-ramdisk',
        '-maxsize', "${RamBudgetMiB}MB",
        '-checksum',
        '-verify',
        '-nojit',
        '-noeta',
        '-nocolor'
    )
    if ($extractResult.ExitCode -ne 0) {
        throw "Chunked extraction failed with exit code $($extractResult.ExitCode).`nSTDOUT:`n$($extractResult.Stdout)`nSTDERR:`n$($extractResult.Stderr)"
    }

    [int64] $fileBytes = [int64] $FileSizeMiB * 1MB
    [int64] $windowBytes = [int64] $RamBudgetMiB * 1MB
    [int] $windowCount = [int] [Math]::Ceiling($fileBytes / [double] $windowBytes)
    for ($windowIndex = 0; $windowIndex -lt $windowCount; $windowIndex++) {
        [int64] $windowOffset = [int64] $windowIndex * $windowBytes
        [int64] $currentWindowBytes = [Math]::Min($windowBytes, $fileBytes - $windowOffset)
        $expectedWindowLog = "RAM window $($windowIndex + 1)/$windowCount offset=$windowOffset size=$currentWindowBytes"
        if ($extractResult.Stdout -notmatch [regex]::Escape($expectedWindowLog)) {
            throw "Missing window extraction evidence '$expectedWindowLog'.`nSTDOUT:`n$($extractResult.Stdout)"
        }
    }

    $restoredFiles = @(Get-ChildItem -LiteralPath $restoreDirectory -Recurse -File | Where-Object Name -eq 'payload.bin')
    if ($restoredFiles.Count -ne 1) {
        throw "Expected exactly one restored payload.bin, found $($restoredFiles.Count)."
    }
    if ($restoredFiles[0].Length -ne ([int64] $FileSizeMiB * 1MB)) {
        throw "Restored length mismatch. Expected $([int64] $FileSizeMiB * 1MB), got $($restoredFiles[0].Length)."
    }

    $restoredHash = (Get-FileHash -LiteralPath $restoredFiles[0].FullName -Algorithm SHA256).Hash
    if ($restoredHash -ne $sourceHash) {
        throw "Restored SHA-256 mismatch. Expected $sourceHash, got $restoredHash."
    }

    $restoredSmallFiles = @(Get-ChildItem -LiteralPath $restoreDirectory -Recurse -File | Where-Object Name -eq 'small.bin')
    if ($restoredSmallFiles.Count -ne 1) {
        throw "Expected exactly one restored small.bin, found $($restoredSmallFiles.Count)."
    }
    $restoredSmallHash = (Get-FileHash -LiteralPath $restoredSmallFiles[0].FullName -Algorithm SHA256).Hash
    if ($restoredSmallHash -ne $smallSourceHash) {
        throw "Small-file SHA-256 mismatch. Expected $smallSourceHash, got $restoredSmallHash."
    }

    if ($MaximumPeakWorkingSetMiB -gt 0) {
        $maximumBytes = [int64] $MaximumPeakWorkingSetMiB * 1MB
        if ($extractResult.PeakWorkingSet64 -le 0) {
            throw 'Peak working set measurement was unavailable.'
        }
        if ($extractResult.PeakWorkingSet64 -gt $maximumBytes) {
            throw "Peak working set $($extractResult.PeakWorkingSet64) exceeded limit $maximumBytes."
        }
    }

    Write-Host "PASS: w -ramdisk restored a $FileSizeMiB MiB windowed file and a normal RAM-batch file with a $RamBudgetMiB MiB budget."
    Write-Host "SHA-256: $sourceHash"
    Write-Host "Peak working set: $([Math]::Round($extractResult.PeakWorkingSet64 / 1MB, 2)) MiB"
} finally {
    if ($KeepArtifacts) {
        Write-Host "Artifacts kept at $testRoot"
    } elseif (Test-Path -LiteralPath $testRoot) {
        Remove-Item -LiteralPath $testRoot -Recurse -Force
    }
}
