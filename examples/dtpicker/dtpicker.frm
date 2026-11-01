VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Begin VB.Form frmPicker
   Caption         =   "DTPicker"
   ClientHeight    =   2400
   ClientWidth     =   4200
   Begin MSComCtl2.DTPicker dtpLong
      Height          =   330
      Left            =   240
      Top             =   240
      Width           =   3600
      Format          =   126615552
      CurrentDate     =   46326.5
   End
   Begin MSComCtl2.DTPicker dtpShort
      Height          =   330
      Left            =   240
      Top             =   720
      Width           =   1700
      CheckBox        =   -1  'True
      Format          =   126615553
      CurrentDate     =   46326.5
   End
   Begin MSComCtl2.DTPicker dtpTime
      Height          =   330
      Left            =   2100
      Top             =   720
      Width           =   1700
      Format          =   126615554
      UpDown          =   -1  'True
      CurrentDate     =   46326.5
   End
   Begin MSComCtl2.DTPicker dtpCustom
      Height          =   330
      Left            =   240
      Top             =   1200
      Width           =   3600
      CustomFormat    =   "yyyy-MM-dd HH:mm"
      Format          =   126615555
      CurrentDate     =   46326.5
   End
   Begin VB.Label lblPicked
      Caption         =   "Pick a date"
      Height          =   255
      Left            =   240
      Top             =   1740
      Width           =   3600
   End
End
Attribute VB_Name = "frmPicker"
Option Explicit

Private Sub dtpLong_Change()
    lblPicked.Caption = "Long date changed to " & Format$(dtpLong.Value, "yyyy-mm-dd")
End Sub
