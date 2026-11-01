VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Begin VB.Form frmSpin
   Caption         =   "UpDown"
   ClientHeight    =   1500
   ClientWidth     =   3600
   Begin MSComCtl2.UpDown udQuantity
      Height          =   420
      Left            =   1560
      Max             =   20
      Min             =   1
      Top             =   360
      Value           =   5
      Width           =   270
   End
   Begin MSComCtl2.UpDown udStep
      Height          =   270
      Left            =   1560
      Max             =   100
      Orientation     =   1
      Top             =   960
      Value           =   40
      Width           =   420
      Wrap            =   -1  'True
   End
   Begin VB.Label lblQuantity
      Caption         =   "Quantity: 5"
      Height          =   255
      Left            =   240
      Top             =   480
      Width           =   1200
   End
   Begin VB.Label lblStep
      Caption         =   "Step: 40"
      Height          =   255
      Left            =   240
      Top             =   1020
      Width           =   1200
   End
End
Attribute VB_Name = "frmSpin"
Option Explicit

Private Sub udQuantity_Change()
    lblQuantity.Caption = "Quantity: " & udQuantity.Value
End Sub

Private Sub udStep_Change()
    lblStep.Caption = "Step: " & udStep.Value
End Sub
