VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Begin VB.Form Form1
   Caption         =   "Callback fields"
   Begin MSComCtl2.DTPicker DTPicker1
      Height          =   330
      Left            =   120
      Top             =   120
      Width           =   3600
      _ExtentX        =   6350
      _ExtentY        =   582
      _Version        =   393216
      CustomFormat    =   "'Week 'XX"
      Format          =   3
      CurrentDate     =   46326.5
   End
End
Attribute VB_Name = "Form1"
Option Explicit

Private Sub DTPicker1_Format(ByVal CallbackField As String, FormattedString As String)
    FormattedString = Format$(DatePart("ww", DTPicker1.Value), "00")
    Debug.Print "format"; CallbackField; FormattedString
End Sub

Private Sub DTPicker1_FormatSize(ByVal CallbackField As String, Size As Integer)
    Size = 24
    Debug.Print "size"; CallbackField; Size
End Sub

Private Sub DTPicker1_CallbackKeyDown(ByVal KeyCode As Integer, ByVal Shift As Integer, ByVal CallbackField As String, CallbackDate As Date)
    Debug.Print "key"; KeyCode; Shift; CallbackField
    If KeyCode = 38 Then CallbackDate = CallbackDate + 7
    If KeyCode = 40 Then CallbackDate = CallbackDate - 7
End Sub

Private Sub DTPicker1_Change()
    Debug.Print "change"; Format$(DTPicker1.Value, "yyyy-mm-dd")
End Sub
