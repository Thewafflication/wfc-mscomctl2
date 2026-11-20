VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Begin VB.Form frmCalendar
   Caption         =   "MonthView"
   ClientHeight    =   4590
   ClientWidth     =   7200
   Begin MSComCtl2.MonthView mvDate
      Height          =   2370
      Left            =   660
      Top             =   1620
      Width           =   3420
      ForeColor       =   &HF5EBE8&
      BackColor       =   &H33201B&
      MonthBackColor  =   &H452B25&
      TitleBackColor  =   &HC6D63D&
      TitleForeColor  =   &H1F1411&
      TrailingForeColor =   &HBEA098&
      CurrentDate     =   46326
      ShowWeekNumbers =   -1  'True
   End
   Begin VB.Label lblTitle
      Caption         =   "MonthView"
      BeginProperty Font 
         Name            =   "Segoe UI"
         Size            =   18
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &HC6D63D&
      Height          =   480
      Left            =   360
      Top             =   210
      Width           =   6480
   End
   Begin VB.Label lblSubtitle
      Caption         =   "Pick a date; the weeks are numbered and one day is bold"
      BeginProperty Font 
         Name            =   "Segoe UI"
         Size            =   10
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &HBEA098&
      Height          =   300
      Left            =   360
      Top             =   780
      Width           =   6480
   End
   Begin VB.Label lblSelectedName
      Caption         =   "Selected"
      BeginProperty Font 
         Name            =   "Segoe UI"
         Size            =   11
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &HF5EBE8&
      Height          =   330
      Left            =   4380
      Top             =   1620
      Width           =   2400
   End
   Begin VB.Label lblDate
      Caption         =   "31 Oct 2026"
      BeginProperty Font 
         Name            =   "Segoe UI"
         Size            =   14
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H54B4FF&
      Height          =   390
      Left            =   4380
      Top             =   1980
      Width           =   2400
   End
   Begin VB.Label lblWeekday
      Caption         =   "Saturday"
      BeginProperty Font 
         Name            =   "Segoe UI"
         Size            =   10
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &HBEA098&
      Height          =   300
      Left            =   4380
      Top             =   2430
      Width           =   2400
   End
   Begin VB.Label lblWeek
      Caption         =   "Week 44"
      BeginProperty Font 
         Name            =   "Segoe UI"
         Size            =   10
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &HBEA098&
      Height          =   300
      Left            =   4380
      Top             =   2730
      Width           =   2400
   End
   Begin VB.Shape shpCard
      Height          =   300
      Left            =   0
      Top             =   0
      Width           =   300
   End
   Begin VB.Shape shpCardS1
      Height          =   300
      Left            =   0
      Top             =   0
      Width           =   300
   End
   Begin VB.Shape shpCardS2
      Height          =   300
      Left            =   0
      Top             =   0
      Width           =   300
   End
End
Attribute VB_Name = "frmCalendar"
Option Explicit

Private Sub Form_Load()
    ThemeApply Me
    ThemeCard shpCard, shpCardS1, shpCardS2, 360, 1260, 6480, 2970, 15
    ThemeCardLabels Me, shpCard
    mvDate.DayBold(DateSerial(2026, 10, 15)) = True
    ShowDate
End Sub

Private Sub ShowDate()
    lblDate.Caption = Format$(mvDate.Value, "d mmm yyyy")
    lblWeekday.Caption = Format$(mvDate.Value, "dddd")
    lblWeek.Caption = "Week " & mvDate.Week
End Sub

Private Sub mvDate_DateClick(ByVal DateClicked As Date)
    ShowDate
End Sub
