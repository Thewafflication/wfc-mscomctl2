VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Begin VB.Form frmFlat
   Caption         =   "FlatScrollBar"
   ClientHeight    =   1800
   ClientWidth     =   3900
   Begin MSComCtl2.FlatScrollBar fsbHorizontal
      Height          =   255
      LargeChange     =   10
      Left            =   240
      Max             =   100
      Orientation     =   1
      Top             =   240
      Value           =   35
      Width           =   2400
   End
   Begin MSComCtl2.FlatScrollBar fsbVertical
      Height          =   1260
      Left            =   3240
      Max             =   100
      Top             =   240
      Value           =   60
      Width           =   255
   End
   Begin VB.Label lblValues
      Caption         =   "Horizontal 35, vertical 60"
      Height          =   255
      Left            =   240
      Top             =   840
      Width           =   2600
   End
End
Attribute VB_Name = "frmFlat"
Option Explicit

Private Sub ShowValues()
    lblValues.Caption = "Horizontal " & fsbHorizontal.Value & ", vertical " & fsbVertical.Value
End Sub

Private Sub fsbHorizontal_Change()
    ShowValues
End Sub

Private Sub fsbVertical_Change()
    ShowValues
End Sub
