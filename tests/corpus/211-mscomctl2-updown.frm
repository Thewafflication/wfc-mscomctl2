VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Begin VB.Form Form1
   Caption         =   "UpDown"
   Begin MSComCtl2.UpDown UpDown1
      Height          =   360
      Left            =   120
      Max             =   5
      Min             =   1
      Top             =   120
      Value           =   2
      Width           =   240
   End
   Begin MSComCtl2.UpDown UpDown2
      Height          =   360
      Increment       =   2
      Left            =   600
      Max             =   2
      Top             =   120
      Value           =   2
      Width           =   240
      Wrap            =   -1  'True
   End
End
Attribute VB_Name = "Form1"
Option Explicit

Private Sub Form_Load()
    Debug.Print UpDown1.Min; UpDown1.Max; UpDown1.Value; UpDown1.Increment; UpDown1.Wrap; TypeName(UpDown1)
    Debug.Print UpDown2.Min; UpDown2.Max; UpDown2.Value; UpDown2.Increment; UpDown2.Wrap
    UpDown1.Value = 3
    Debug.Print "value"; UpDown1.Value; UpDown1
    UpDown1.Max = 3
    UpDown1.Min = 2
    Debug.Print UpDown1.Min; UpDown1.Max; UpDown1.Value
    UpDown1.Max = 5
    UpDown1.Min = 1
    On Error Resume Next
    UpDown1.Value = 9
    Debug.Print "high"; Err.Number; UpDown1.Value
    Err.Clear
    UpDown1.Min = 7
    Debug.Print "min"; Err.Number; UpDown1.Min
    Err.Clear
    UpDown1.Value = 3
End Sub

Private Sub UpDown1_Change()
    Debug.Print "change1"; UpDown1.Value
End Sub

Private Sub UpDown1_UpClick()
    Debug.Print "up1"; UpDown1.Value
End Sub

Private Sub UpDown1_DownClick()
    Debug.Print "down1"; UpDown1.Value
End Sub

Private Sub UpDown2_Change()
    Debug.Print "change2"; UpDown2.Value
End Sub

Private Sub UpDown2_UpClick()
    Debug.Print "up2"; UpDown2.Value
End Sub

Private Sub UpDown2_DownClick()
    Debug.Print "down2"; UpDown2.Value
End Sub
