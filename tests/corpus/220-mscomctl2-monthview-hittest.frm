VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Begin VB.Form Form1
   Caption         =   "HitTest"
   Begin MSComCtl2.MonthView Month1
      Height          =   2370
      Left            =   120
      Top             =   120
      Width           =   2700
      _ExtentX        =   4763
      _ExtentY        =   4180
      _Version        =   393216
      CurrentDate     =   46539
   End
End
Attribute VB_Name = "Form1"
Option Explicit

Private Sub Form_Load()
    Dim area As Long
    Dim d As Date
    Dim w As Single
    Dim h As Single
    d = DateSerial(2000, 1, 1)
    area = Month1.HitTest(100, 100, d)
    Debug.Print "hit"; area; Format$(d, "yyyy-mm-dd")
    Month1.ComputeControlSize 1, 1, w, h
    Debug.Print "size"; w; h
End Sub
