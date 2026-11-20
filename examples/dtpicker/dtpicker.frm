VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Begin VB.Form frmPicker
   Caption         =   "DTPicker"
   ClientHeight    =   5000
   ClientWidth     =   6600
   Begin MSComCtl2.DTPicker dtpLong
      Height          =   345
      Left            =   2400
      Top             =   1560
      Width           =   3600
      CalendarBackColor =   &H452B25&
      CalendarForeColor =   &HF5EBE8&
      CalendarTitleBackColor =   &HC6D63D&
      CalendarTitleForeColor =   &H1F1411&
      CalendarTrailingForeColor =   &HBEA098&
      Format          =   126615552
      CurrentDate     =   46326.5
   End
   Begin MSComCtl2.DTPicker dtpShort
      Height          =   345
      Left            =   2400
      Top             =   2160
      Width           =   3600
      CheckBox        =   -1  'True
      CalendarBackColor =   &H452B25&
      CalendarForeColor =   &HF5EBE8&
      CalendarTitleBackColor =   &HC6D63D&
      CalendarTitleForeColor =   &H1F1411&
      CalendarTrailingForeColor =   &HBEA098&
      Format          =   126615553
      CurrentDate     =   46326.5
   End
   Begin MSComCtl2.DTPicker dtpClock
      Height          =   345
      Left            =   2400
      Top             =   2760
      Width           =   3600
      CalendarBackColor =   &H452B25&
      CalendarForeColor =   &HF5EBE8&
      CalendarTitleBackColor =   &HC6D63D&
      CalendarTitleForeColor =   &H1F1411&
      CalendarTrailingForeColor =   &HBEA098&
      Format          =   126615554
      UpDown          =   -1  'True
      CurrentDate     =   46326.5
   End
   Begin MSComCtl2.DTPicker dtpPattern
      Height          =   345
      Left            =   2400
      Top             =   3360
      Width           =   3600
      CalendarBackColor =   &H452B25&
      CalendarForeColor =   &HF5EBE8&
      CalendarTitleBackColor =   &HC6D63D&
      CalendarTitleForeColor =   &H1F1411&
      CalendarTrailingForeColor =   &HBEA098&
      CustomFormat    =   "yyyy-MM-dd HH:mm"
      Format          =   126615555
      CurrentDate     =   46326.5
   End
   Begin VB.Label lblTitle
      Caption         =   "DTPicker"
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
      Width           =   5880
   End
   Begin VB.Label lblSubtitle
      Caption         =   "Four ways to show a date and a time"
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
      Width           =   5880
   End
   Begin VB.Label lblLongName
      Caption         =   "Long date"
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
      Left            =   660
      Top             =   1590
      Width           =   1600
   End
   Begin VB.Label lblShortName
      Caption         =   "Short date"
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
      Left            =   660
      Top             =   2190
      Width           =   1600
   End
   Begin VB.Label lblClockName
      Caption         =   "Time"
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
      Left            =   660
      Top             =   2790
      Width           =   1600
   End
   Begin VB.Label lblPatternName
      Caption         =   "Custom"
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
      Left            =   660
      Top             =   3390
      Width           =   1600
   End
   Begin VB.Label lblPickedName
      Caption         =   "Last change"
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
      Left            =   660
      Top             =   4020
      Width           =   1600
   End
   Begin VB.Label lblPicked
      Caption         =   "Pick a date"
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
      Left            =   2400
      Top             =   3990
      Width           =   3600
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
Attribute VB_Name = "frmPicker"
Option Explicit

Private Sub Form_Load()
    ThemeApply Me
    ThemeCard shpCard, shpCardS1, shpCardS2, 360, 1260, 5880, 3400, 15
    ThemeCardLabels Me, shpCard
End Sub

Private Sub dtpLong_Change()
    Dim v As Variant
    v = dtpLong.Value
    lblPicked.Caption = Format$(v, "d mmmm yyyy")
End Sub

Private Sub dtpShort_Change()
    Dim v As Variant
    v = dtpShort.Value
    If IsNull(v) Then
        lblPicked.Caption = "Cleared"
    Else
        lblPicked.Caption = Format$(v, "yyyy-mm-dd")
    End If
End Sub

Private Sub dtpClock_Change()
    Dim v As Variant
    v = dtpClock.Value
    lblPicked.Caption = Format$(v, "hh:nn:ss")
End Sub

Private Sub dtpPattern_Change()
    Dim v As Variant
    v = dtpPattern.Value
    lblPicked.Caption = Format$(v, "yyyy-mm-dd hh:nn")
End Sub
