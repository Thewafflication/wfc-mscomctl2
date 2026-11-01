VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Begin VB.Form Form1
   Caption         =   "FlatScrollBar"
   Begin MSComCtl2.FlatScrollBar Flat1
      Height          =   255
      LargeChange     =   10
      Left            =   120
      Max             =   100
      Min             =   10
      Orientation     =   1
      Top             =   120
      Value           =   20
      Width           =   2000
   End
   Begin MSComCtl2.FlatScrollBar Flat2
      Height          =   1000
      Left            =   2500
      Top             =   120
      Width           =   255
   End
End
Attribute VB_Name = "Form1"
Option Explicit

Private Sub Form_Load()
    Print "start"; Flat1.Value; Flat1.Min; Flat1.Max; Flat2.Max; Flat1.LargeChange; Flat1.SmallChange; Flat2.Orientation; Flat1.Orientation
    On Error Resume Next
    Flat1.Value = 500
    Print Err.Number; Err.Description
    Err.Clear
    Flat1.Value = 50
    Print TypeName(Flat1); Flat1
End Sub

Private Sub Flat1_Change()
    Print "change"; Flat1.Value
End Sub

Private Sub Flat1_Scroll()
    Print "scroll"; Flat1.Value
End Sub

Private Sub Flat2_Change()
    Print "change2"; Flat2.Value
End Sub
