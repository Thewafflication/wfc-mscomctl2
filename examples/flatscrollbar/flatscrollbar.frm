VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Begin VB.Form frmFlat
   Caption         =   "FlatScrollBar"
   ClientHeight    =   4320
   ClientWidth     =   6000
   Begin MSComCtl2.FlatScrollBar fsbHorizontal
      Height          =   285
      LargeChange     =   10
      Left            =   660
      Max             =   100
      Orientation     =   1
      Top             =   2370
      Value           =   35
      Width           =   3900
   End
   Begin MSComCtl2.FlatScrollBar fsbVertical
      Height          =   2100
      LargeChange     =   10
      Left            =   5070
      Max             =   100
      Orientation     =   0
      Top             =   1620
      Value           =   60
      Width           =   285
   End
   Begin VB.Label lblTitle
      Caption         =   "FlatScrollBar"
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
      Width           =   5280
   End
   Begin VB.Label lblSubtitle
      Caption         =   "Drag the thumb, or click an arrow or the track"
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
      Width           =   5280
   End
   Begin VB.Label lblHorizontalName
      Caption         =   "Horizontal"
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
      Top             =   1560
      Width           =   2200
   End
   Begin VB.Label lblHorizontalHint
      Caption         =   "0 to 100, page of 10"
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
      Top             =   1890
      Width           =   2400
   End
   Begin VB.Label lblHorizontal
      Alignment       =   1
      Caption         =   "35"
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
      Left            =   3300
      Top             =   1590
      Width           =   1260
   End
   Begin VB.Label lblVerticalName
      Caption         =   "Vertical"
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
      Top             =   2940
      Width           =   2200
   End
   Begin VB.Label lblVerticalHint
      Caption         =   "0 to 100"
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
      Top             =   3270
      Width           =   2400
   End
   Begin VB.Label lblVertical
      Alignment       =   1
      Caption         =   "60"
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
      Left            =   3300
      Top             =   2970
      Width           =   1260
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
Attribute VB_Name = "frmFlat"
Option Explicit

Private Sub Form_Load()
    ThemeApply Me
    ThemeCard shpCard, shpCardS1, shpCardS2, 360, 1260, 5280, 2700, 15
    ThemeCardLabels Me, shpCard
End Sub

Private Sub fsbHorizontal_Change()
    lblHorizontal.Caption = fsbHorizontal.Value
End Sub

Private Sub fsbVertical_Change()
    lblVertical.Caption = fsbVertical.Value
End Sub
