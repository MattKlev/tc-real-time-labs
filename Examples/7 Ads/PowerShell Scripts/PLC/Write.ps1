$session = New-TcSession -NetId '192.168.1.102.1.1' -Port 851

# Get symbol — Size property tells us the array length in bytes
$symbol = $session | Get-TcSymbol -Path 'MAIN.Data'

# Read the full byte array
[byte[]]$bytes = $symbol | Read-TcValue

# Increment each byte by 1, wrapping at 255 -> 0
[byte[]]$incremented = $bytes | ForEach-Object { [byte](($_ + 1) % 256) }

# Write back
$symbol | Write-TcValue -Value $incremented -Force