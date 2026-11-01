VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Begin VB.Form Form1
   Caption         =   "Calendar colours"
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
      MonthBackColor  =   13434879
      TitleBackColor  =   8388608
      TitleForeColor  =   16777215
      TrailingForeColor=  8421504
      CurrentDate     =   46539
   End
   Begin MSComCtl2.DTPicker DTPicker1
      Height          =   330
      Left            =   3000
      Top             =   120
      Width           =   2400
      _ExtentX        =   4233
      _ExtentY        =   582
      _Version        =   393216
      CalendarBackColor=  13434879
      CalendarTitleBackColor= 8388608
      CalendarTitleForeColor= 16777215
      CurrentDate     =   46539
   End
End
Attribute VB_Name = "Form1"
Option Explicit

Private Sub Form_Load()
    Debug.Print Month1.MonthBackColor; Month1.TitleBackColor; Month1.TitleForeColor; Month1.TrailingForeColor
    Debug.Print DTPicker1.CalendarBackColor; DTPicker1.CalendarTitleBackColor
    Month1.TitleBackColor = &H800000
    DTPicker1.CalendarTrailingForeColor = &H808080
    Debug.Print Month1.TitleBackColor; DTPicker1.CalendarTrailingForeColor
End Sub
