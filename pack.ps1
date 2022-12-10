$gitTag = (git tag --points-at)
$archiveName = "aviutl-draw"
if (![string]::IsNullOrEmpty($gitTag))
{
    $archiveName = "${archiveName}_${gitTag}"
}

New-Item publish -ItemType Directory -Force

7z a "publish\${archiveName}.zip" `
    .\README.md `
    .\LICENSE `
    .\CREDITS.md `
    ".\Release\KaroterraDraw.dll"
