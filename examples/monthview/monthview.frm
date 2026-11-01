VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Begin VB.Form frmCalendar
   Caption         =   "MonthView"
   ClientHeight    =   3400
   ClientWidth     =   3900
   Begin MSComCtl2.MonthView mvDate
      Height          =   2370
      Left            =   240
      Top             =   240
      Width           =   3420
      CurrentDate     =   46326
      ShowWeekNumbers =   -1  'True
   End
   Begin VB.Label lblDate
      Caption         =   "Selected:"
      Height          =   255
      Left            =   240
      Top             =   2880
      Width           =   3400
   End
End
Attribute VB_Name = "frmCalendar"
Option Explicit

Private Sub Form_Load()
    mvDate.DayBold(DateSerial(2026, 10, 15)) = True
    ShowDate
End Sub

Private Sub ShowDate()
    lblDate.Caption = "Selected: " & Format$(mvDate.Value, "dddd, d mmmm yyyy")
End Sub

Private Sub mvDate_DateClick(ByVal DateClicked As Date)
    ShowDate
End Sub
