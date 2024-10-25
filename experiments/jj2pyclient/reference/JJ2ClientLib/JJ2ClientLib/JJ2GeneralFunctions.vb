Imports JJ2ClientLib.JJ2
Public Class JJ2GeneralFunctions
    Public Shared Function GetGameModeShortString(gameMode As Byte, customMode As Byte) As String
        If customMode = 0 Then
            Select Case gameMode
                Case 0
                    Return "SP"
                Case 1
                    Return "Coop"
                Case 2
                    Return "Battle"
                Case 3
                    Return "Race"
                Case 4
                    Return "Treasure"
                Case 5
                    Return "CTF"
                Case Else
                    Return ""
            End Select
        Else
            Select Case customMode
                Case 1
                    Return "RT"
                Case 2
                    Return "LRS"
                Case 3
                    Return "XLRS"
                Case 4
                    Return "PEST"
                Case 11
                    Return "TB"
                Case 12
                    Return "JB"
                Case 13
                    Return "DCTF"
                Case 14
                    Return "FR"
                Case 15
                    Return "TLRS"
                Case 16
                    Return "Dom"
                Case 17
                    Return "HH"
                Case Else
                    Return ""
            End Select
        End If
    End Function


End Class
