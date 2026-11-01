VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Begin VB.Form frmClip
   Caption         =   "Animation"
   ClientHeight    =   1800
   ClientWidth     =   3000
   Begin MSComCtl2.Animation Anim1
      Height          =   1215
      Left            =   600
      Top             =   240
      Width           =   1815
      Center          =   -1  'True
   End
   Begin VB.Label lblState
      Caption         =   "Playing"
      Height          =   255
      Left            =   600
      Top             =   1500
      Width           =   1815
   End
End
Attribute VB_Name = "frmClip"
Option Explicit

Private Sub Form_Load()
    Anim1.Open App.Path & "\clip.avi"
    Anim1.Play
End Sub

Private Sub Anim1_Click()
    Anim1.Stop
    lblState.Caption = "Stopped"
End Sub
