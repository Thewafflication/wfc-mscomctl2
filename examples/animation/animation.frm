VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Begin VB.Form frmClip
   Caption         =   "Animation"
   ClientHeight    =   4560
   ClientWidth     =   6000
   Begin MSComCtl2.Animation Anim1
      Height          =   1500
      Left            =   2250
      Top             =   1500
      Width           =   1500
      BackStyle       =   1
      Center          =   -1  'True
   End
   Begin VB.Label lblTitle
      Caption         =   "Animation"
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
      Caption         =   "A clip plays in the control; click it to stop"
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
   Begin VB.Label lblState
      Alignment       =   2
      Caption         =   "Playing"
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
      Left            =   660
      Top             =   3150
      Width           =   4680
   End
   Begin VB.Label lblHint
      Alignment       =   2
      Caption         =   "Click the clip to stop or start it"
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
      Top             =   3600
      Width           =   4680
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
Attribute VB_Name = "frmClip"
Option Explicit

Private Sub Form_Load()
    ThemeApply Me
    ThemeCard shpCard, shpCardS1, shpCardS2, 360, 1260, 5280, 2940, 15
    ThemeCardLabels Me, shpCard
    Anim1.Open App.Path & "\clip.avi"
    Anim1.Play
End Sub

Private Sub Anim1_Click()
    If lblState.Caption = "Playing" Then
        Anim1.Stop
        lblState.Caption = "Stopped"
    Else
        Anim1.Play
        lblState.Caption = "Playing"
    End If
End Sub
