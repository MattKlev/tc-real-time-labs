$session = New-TcSession -NetId '192.168.1.102.1.1' -Port 851
$symbol = $session | get-TcSymbol -Path 'MAIN.Data'
$symbol | Read-TcValue |  Out-Null