VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Begin VB.Form Form1
   Caption         =   "Animation"
   Begin MSComCtl2.Animation Anim1
      Height          =   720
      Left            =   120
      Top             =   120
      Width           =   720
      _ExtentX        =   1270
      _ExtentY        =   1270
      _Version        =   393216
      FullWidth       =   48
      FullHeight      =   48
   End
   Begin MSComCtl2.Animation Anim2
      Height          =   720
      Left            =   1200
      Top             =   120
      Width           =   720
      AutoPlay        =   -1  'True
      Center          =   -1  'True
      BackStyle       =   0
   End
End
Attribute VB_Name = "Form1"
Option Explicit

Private Sub Form_Load()
    Debug.Print Anim1.AutoPlay; Anim1.Center; Anim1.BackStyle; TypeName(Anim1)
    Debug.Print Anim2.AutoPlay; Anim2.Center; Anim2.BackStyle
    On Error Resume Next
    Anim1.Open App.Path & "\217-clip.avi"
    Debug.Print "open"; Err.Number
    Err.Clear
    Anim1.Play
    Anim1.Play 2, 0, 3
    Anim1.Stop
    Anim1.Play , 1
    Debug.Print "play"; Err.Number
    Err.Clear
    Anim1.Close
    Debug.Print "closed"; Err.Number
    Err.Clear
    Anim1.Play
    Debug.Print "playclosed"; Err.Number
    Err.Clear
    Anim1.Open "missing.avi"
    Debug.Print "missing"; Err.Number; Err.Description
    Err.Clear
    Anim1.Open App.Path & "\217-notavi.txt"
    Debug.Print "text"; Err.Number; Err.Description
    Err.Clear
    Anim2.Open App.Path & "\217-clip.avi"
    Debug.Print "auto"; Err.Number
    Err.Clear
    Anim1.Center = True
    Anim1.BackStyle = 0
    Debug.Print Anim1.Center; Anim1.BackStyle
End Sub

Private Sub Anim1_Click()
    Debug.Print "click1"
End Sub

Private Sub Anim1_DblClick()
    Debug.Print "dblclick1"
End Sub

Private Sub Anim2_Click()
    Debug.Print "click2"
End Sub
