VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Begin VB.Form Form1
   Caption         =   "DTPicker"
   Begin MSComCtl2.DTPicker DTPicker1
      Height          =   330
      Left            =   120
      Top             =   120
      Width           =   2400
      _ExtentX        =   4233
      _ExtentY        =   582
      _Version        =   393216
      Format          =   126615553
      CurrentDate     =   46326.5
   End
   Begin MSComCtl2.DTPicker DTPicker2
      Height          =   330
      Left            =   120
      Top             =   600
      Width           =   2400
      CustomFormat    =   "yyyy-MM-dd HH:mm"
      CheckBox        =   -1  'True
      Format          =   126615555
      CurrentDate     =   46000.25
   End
End
Attribute VB_Name = "Form1"
Option Explicit

Private Function Stamp(ByVal v As Variant) As String
    If IsNull(v) Then
        Stamp = "null"
    Else
        Stamp = Format$(v, "yyyy-mm-dd hh:nn:ss")
    End If
End Function

Private Function Now1() As String
    Dim v As Variant
    v = DTPicker1.Value
    Now1 = Stamp(v)
End Function

Private Function Now2() As String
    Dim v As Variant
    v = DTPicker2.Value
    Now2 = Stamp(v)
End Function

Private Sub Form_Load()
    Debug.Print Now1; DTPicker1.Day; DTPicker1.Month; DTPicker1.Year; DTPicker1.Hour; DTPicker1.Minute; DTPicker1.Second; DTPicker1.DayOfWeek; DTPicker1.Format; TypeName(DTPicker1)
    Debug.Print Now2; DTPicker2.Format; DTPicker2.CheckBox; DTPicker2.CustomFormat; DTPicker2.UpDown
    DTPicker1.Value = DateSerial(2027, 3, 4) + TimeSerial(5, 6, 7)
    DTPicker1.Hour = 23
    On Error Resume Next
    DTPicker1.Hour = 24
    Debug.Print "hour"; Err.Number; DTPicker1.Hour
    Err.Clear
    DTPicker1.Day = 31
    Debug.Print Now1
    DTPicker1.Month = 2
    Debug.Print Now1
    DTPicker1.Day = 30
    Debug.Print "badday"; Err.Number
    Err.Clear
    DTPicker1.MinDate = DateSerial(2027, 1, 1)
    DTPicker1.MaxDate = DateSerial(2027, 12, 31)
    DTPicker1.Value = DateSerial(2028, 1, 1)
    Debug.Print "range"; Err.Number; Now1
    Err.Clear
    DTPicker1.Value = Null
    Debug.Print "null"; Err.Number
    Err.Clear
    DTPicker2.Value = Null
    Debug.Print "cleared"; Err.Number; IsNull(DTPicker2.Value); IsNull(DTPicker2.Day)
    Err.Clear
    DTPicker2.Hour = 3
    Debug.Print "nullpart"; Err.Number
    Err.Clear
End Sub

Private Sub DTPicker1_Change()
    Debug.Print "change1"; Now1
End Sub

Private Sub DTPicker1_DropDown()
    Debug.Print "dropdown1"
End Sub

Private Sub DTPicker1_CloseUp()
    Debug.Print "closeup1"
End Sub

Private Sub DTPicker2_Change()
    Debug.Print "change2"; Now2
End Sub
