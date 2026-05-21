$session = New-TcSession -NetId '192.168.1.102.1.1' -Port 351

# Get symbol — Size property tells us the array length in bytes
$symbol = $session | Get-TcSymbol -Path 'Module1.Outputs.Data' # at the edge of enough time to process
#$symbol = $session | Get-TcSymbol -Path 'Module1.Data' #will cause a cycle exceed

# Read the full byte array
[byte[]]$bytes = $symbol | Read-TcValue

# Increment each byte by 1, wrapping at 255 -> 0
[byte[]]$incremented = $bytes | ForEach-Object { [byte](($_ + 1) % 256) }

# Write back
$symbol | Write-TcValue -Value $incremented -Force