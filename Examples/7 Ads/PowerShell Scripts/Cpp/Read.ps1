$session = New-TcSession -NetId '192.168.1.102.1.1' -Port 351

$symbol = $session | Get-TcSymbol -Path 'Module1.Outputs.Data' # at the edge of enough time to process
#$symbol = $session | Get-TcSymbol -Path 'Module1.Data' #will cause a cycle exceed

$symbol | Read-TcValue |  Out-Null