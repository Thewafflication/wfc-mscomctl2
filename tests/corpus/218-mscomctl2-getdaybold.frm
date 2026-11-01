VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Begin VB.Form Form1
   Caption         =   "GetDayBold"
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

Private Sub Month1_GetDayBold(ByVal StartDate As Date, ByVal Count As Integer, State() As Boolean)
    Dim i As Integer
    Debug.Print "ask"; Format$(StartDate, "yyyy-mm-dd"); Count; LBound(State); UBound(State)
    For i = 0 To Count - 1
        If Weekday(StartDate + i) = 1 Then State(i) = True
    Next i
End Sub

Private Sub Form_Load()
    Month1.DayBold(DateSerial(2027, 6, 6)) = False
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Debug.Print "bold"; Month1.DayBold(DateSerial(2027, 6, 6)); Month1.DayBold(DateSerial(2027, 6, 7)); Month1.DayBold(DateSerial(2027, 6, 13)); Month1.DayBold(DateSerial(2027, 6, 27)); Month1.DayBold(DateSerial(2027, 7, 4))
End Sub
