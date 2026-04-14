$targets = @("../test/test", "src", "inc")
$clangFormat = "C:/Users/mundo/.vscode/extensions/ms-vscode.cpptools-1.31.4-win32-x64/LLVM/bin/clang-format.exe"

$files = foreach ($target in $targets) {
    Get-ChildItem -Path $target -Recurse -File -Include *.c, *.h
}

$files += Get-Item app.h, app.c, main.c

$needsFormatting = foreach ($file in ($files | Sort-Object FullName -Unique)) {
    & $clangFormat --dry-run --Werror --style=file $file.FullName 2>$null | Out-Null
    if ($LASTEXITCODE -ne 0) {
        $file.FullName
    }
}

if ($needsFormatting) {
    Write-Host "These files require formatting:" -ForegroundColor Red
    $needsFormatting | ForEach-Object { Write-Host $_ -ForegroundColor Red }
    exit 1
}

Write-Host "All files are formatted!" -ForegroundColor Green
