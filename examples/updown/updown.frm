VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Begin VB.Form frmSpin
   Caption         =   "UpDown"
   ClientHeight    =   4020
   ClientWidth     =   6000
   Begin MSComCtl2.UpDown udQuantity
      Height          =   570
      Left            =   4560
      Max             =   20
      Min             =   1
      Top             =   1590
      Value           =   5
      Width           =   345
   End
   Begin MSComCtl2.UpDown udStep
      Height          =   345
      Left            =   4260
      Max             =   100
      Orientation     =   1
      Top             =   2700
      Value           =   40
      Width           =   645
      Wrap            =   -1  'True
   End
   Begin VB.Label lblTitle
      Caption         =   "UpDown"
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
      Caption         =   "Spin a value with the arrows"
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
   Begin VB.Label lblQuantityName
      Caption         =   "Quantity"
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
   Begin VB.Label lblQuantityHint
      Caption         =   "1 to 20, vertical"
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
      Top             =   1920
      Width           =   2400
   End
   Begin VB.Label lblQuantity
      Alignment       =   1
      Caption         =   "5"
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
      Left            =   3000
      Top             =   1620
      Width           =   1260
   End
   Begin VB.Label lblStepName
      Caption         =   "Step"
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
      Top             =   2670
      Width           =   2200
   End
   Begin VB.Label lblStepHint
      Caption         =   "0 to 100, horizontal, wraps"
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
      Top             =   3030
      Width           =   2900
   End
   Begin VB.Label lblStep
      Alignment       =   1
      Caption         =   "40"
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
      Left            =   3000
      Top             =   2730
      Width           =   1140
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
Attribute VB_Name = "frmSpin"
Option Explicit

Private Sub Form_Load()
    ThemeApply Me
    ThemeCard shpCard, shpCardS1, shpCardS2, 360, 1260, 5280, 2400, 15
    ThemeCardLabels Me, shpCard
End Sub

Private Sub udQuantity_Change()
    lblQuantity.Caption = udQuantity.Value
End Sub

Private Sub udStep_Change()
    lblStep.Caption = udStep.Value
End Sub
