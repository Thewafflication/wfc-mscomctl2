VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Begin VB.Form Form1
   Caption         =   "MonthView"
   Begin MSComCtl2.MonthView Month1
      Height          =   2370
      Left            =   120
      Top             =   120
      Width           =   2700
      _ExtentX        =   4763
      _ExtentY        =   4180
      _Version        =   393216
      ForeColor       =   -2147483630
      BackColor       =   -2147483633
      Appearance      =   1
      StartOfWeek     =   82903042
      CurrentDate     =   46326
   End
   Begin MSComCtl2.MonthView Month2
      Height          =   2370
      Left            =   3000
      Top             =   120
      Width           =   2700
      MultiSelect     =   -1  'True
      MaxSelCount     =   3
      StartOfWeek     =   2
      CurrentDate     =   46326
   End
End
Attribute VB_Name = "Form1"
Option Explicit

Private Sub Form_Load()
    Debug.Print Format$(Month1.Value, "yyyy-mm-dd"); Month1.Day; Month1.Month; Month1.Year; Month1.DayOfWeek; TypeName(Month1)
    Debug.Print Format$(Month1.SelStart, "yyyy-mm-dd"); Format$(Month1.SelEnd, "yyyy-mm-dd"); Month1.MaxSelCount; Month1.MultiSelect
    Month1.Value = DateSerial(2026, 2, 10)
    Debug.Print Format$(Month1.Value, "yyyy-mm-dd"); Month1.DayOfWeek; Month1.Week
    Month1.Month = 3
    Debug.Print Format$(Month1.Value, "yyyy-mm-dd")
    Month1.Day = 31
    Month1.Month = 4
    Debug.Print "clamped"; Format$(Month1.Value, "yyyy-mm-dd")
    Month1.Year = 2027
    Debug.Print Format$(Month1.Value, "yyyy-mm-dd")
    On Error Resume Next
    Month1.Day = 31
    Debug.Print "badday"; Err.Number; Month1.Day
    Err.Clear
    Month1.MinDate = DateSerial(2027, 1, 1)
    Month1.MaxDate = DateSerial(2027, 12, 31)
    Month1.Value = DateSerial(2028, 1, 1)
    Debug.Print "range"; Err.Number; Format$(Month1.Value, "yyyy-mm-dd")
    Err.Clear
    Month1.MinDate = DateSerial(2027, 6, 1)
    Debug.Print "moved"; Format$(Month1.Value, "yyyy-mm-dd")
    Month1.MaxDate = DateSerial(2027, 5, 1)
    Debug.Print "minmax"; Err.Number
    Err.Clear
    Debug.Print "visible"; Format$(Month1.VisibleDays(1), "yyyy-mm-dd"); Format$(Month1.VisibleDays(42), "yyyy-mm-dd")
    Month1.VisibleDays(43)
    Debug.Print "index"; Err.Number
    Err.Clear
    Month1.DayBold(DateSerial(2027, 6, 15)) = True
    Debug.Print "bold"; Month1.DayBold(DateSerial(2027, 6, 15)); Month1.DayBold(DateSerial(2027, 6, 16))
    Month2.SelStart = DateSerial(2026, 10, 10)
    Month2.SelEnd = DateSerial(2026, 10, 12)
    Debug.Print "multi"; Format$(Month2.SelStart, "yyyy-mm-dd"); Format$(Month2.SelEnd, "yyyy-mm-dd")
    Month2.SelEnd = DateSerial(2026, 10, 20)
    Debug.Print "toomany"; Err.Number
    Err.Clear
End Sub

Private Sub Month1_SelChange(StartDate As Date, EndDate As Date, Cancel As Boolean)
    Debug.Print "sel1"; Format$(StartDate, "yyyy-mm-dd"); Format$(EndDate, "yyyy-mm-dd")
    If StartDate = DateSerial(2027, 7, 4) Then Cancel = True
End Sub

Private Sub Month1_DateClick(ByVal DateClicked As Date)
    Debug.Print "click1"; Format$(DateClicked, "yyyy-mm-dd"); Format$(Month1.Value, "yyyy-mm-dd")
End Sub

Private Sub Month1_DateDblClick(ByVal DateDblClicked As Date)
    Debug.Print "dbl1"; Format$(DateDblClicked, "yyyy-mm-dd")
End Sub

Private Sub Month2_SelChange(StartDate As Date, EndDate As Date, Cancel As Boolean)
    Debug.Print "sel2"; Format$(StartDate, "yyyy-mm-dd"); Format$(EndDate, "yyyy-mm-dd")
End Sub
