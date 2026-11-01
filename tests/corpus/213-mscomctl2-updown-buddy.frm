VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Begin VB.Form Form1
   Caption         =   "UpDown buddy"
   Begin VB.TextBox Text1
      Height          =   285
      Left            =   120
      Top             =   120
      Width           =   1200
   End
   Begin MSComCtl2.UpDown UpDown1
      Height          =   285
      Left            =   1320
      Top             =   120
      Width           =   240
      _ExtentX        =   423
      _ExtentY        =   503
      _Version        =   393216
      Value           =   5
      BuddyControl    =   "Text1"
      BuddyDispid     =   196609
      BuddyIndex      =   0
      OrigLeft        =   1320
      OrigTop         =   120
      OrigRight       =   1560
      OrigBottom      =   405
      Max             =   9
      SyncBuddy       =   -1  'True
      BuddyProperty   =   65547
      Enabled         =   -1  'True
   End
   Begin VB.TextBox Text2
      Height          =   285
      Left            =   120
      Top             =   720
      Width           =   1200
   End
   Begin MSComCtl2.UpDown UpDown2
      Height          =   285
      Left            =   1320
      Top             =   720
      Width           =   240
      AutoBuddy       =   -1  'True
      Increment       =   5
      Max             =   50
      SyncBuddy       =   -1  'True
      BuddyProperty   =   "Text"
   End
End
Attribute VB_Name = "Form1"
Option Explicit

Private Sub Form_Load()
    Debug.Print "load"; Text1.Text; UpDown1.Value; "/"; Text2.Text; UpDown2.Value
    Text1.Text = "8"
    Debug.Print "typed"; UpDown1.Value
    Text1.Text = "99"
    Debug.Print "clamped"; UpDown1.Value
    Text1.Text = "abc"
    Debug.Print "text"; UpDown1.Value
    UpDown1.Value = 2
    Debug.Print "set"; Text1.Text
    UpDown2.Value = 20
    Debug.Print "auto"; Text2.Text
    UpDown2.SyncBuddy = False
    UpDown2.Value = 30
    Debug.Print "unsynced"; Text2.Text
End Sub

Private Sub UpDown1_Change()
    Debug.Print "change1"; UpDown1.Value; Text1.Text
End Sub

Private Sub UpDown2_Change()
    Debug.Print "change2"; UpDown2.Value; Text2.Text
End Sub
