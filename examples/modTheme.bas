Attribute VB_Name = "modTheme"
Option Explicit
'
' The look of the examples, the same as the WFC games: a dark navy
' background, raised panels with a thin light edge and a two-step shadow,
' light text, a teal accent and an amber highlight. Colors are VB values
' (&HBBGGRR). The original is tests/vb6games/ui/modTheme.bas in WFC.
'
Public Const ThBack As Long = &H1F1411     ' #11141F window background
Public Const ThPanel As Long = &H33201B    ' #1B2033 raised surface
Public Const ThLine As Long = &H5C3C35     ' #353C5C borders
Public Const ThText As Long = &HF5EBE8     ' #E8EBF5 text
Public Const ThMuted As Long = &HBEA098    ' #98A0BE secondary text
Public Const ThAccent As Long = &HC6D63D   ' #3DD6C6 teal
Public Const ThAccent2 As Long = &H54B4FF  ' #FFB454 amber
Public Const ThShadow1 As Long = &H0F0A08  ' #080A0F near shadow
Public Const ThShadow2 As Long = &H17100D  ' #0D1017 far shadow
Public Const ThEdge As Long = &H6B4A40     ' #404A6B light edge of a panel
Public Const ThFontName As String = "Segoe UI"

' Colors the form and makes its labels opaque on it. The size, weight and color
' of a label come from its Font and ForeColor in the form file (a font set
' from code is not applied to a label that is already shown): the heading is
' teal, secondary text muted, a readout amber, other text light.
Public Sub ThemeApply(frm As Form)
    Dim c As Control
    frm.BackColor = ThBack
    On Error Resume Next
    For Each c In frm.Controls
        If TypeName(c) = "Label" Then
            c.BackStyle = 1
            c.BackColor = ThBack
        End If
    Next c
End Sub

' A raised panel with a thin border and a two-step shadow. The three shapes
' go to the back of the form (the shadows behind the panel); u is the size
' of one pixel in the form's units (15 for twips).
Public Sub ThemeCard(card As Object, shadowA As Object, shadowB As Object, ByVal L As Single, ByVal T As Single, ByVal W As Single, ByVal H As Single, ByVal u As Single)
    shadowB.Left = L + 7 * u
    shadowB.Top = T + 9 * u
    shadowB.Width = W
    shadowB.Height = H
    shadowB.FillStyle = 0
    shadowB.FillColor = ThShadow2
    shadowB.BorderStyle = 0
    shadowA.Left = L + 3 * u
    shadowA.Top = T + 4 * u
    shadowA.Width = W
    shadowA.Height = H
    shadowA.FillStyle = 0
    shadowA.FillColor = ThShadow1
    shadowA.BorderStyle = 0
    card.Left = L
    card.Top = T
    card.Width = W
    card.Height = H
    card.FillStyle = 0
    card.FillColor = ThPanel
    card.BorderColor = ThEdge
    card.BorderWidth = 1
End Sub

' Gives the labels that lie inside the card the color of the card (a label is
' an opaque window, so it must match what is behind it).
Public Sub ThemeCardLabels(frm As Form, card As Object)
    Dim c As Control
    On Error Resume Next
    For Each c In frm.Controls
        If TypeName(c) = "Label" Then
            If c.Left >= card.Left And c.Top >= card.Top And c.Left + c.Width <= card.Left + card.Width And c.Top + c.Height <= card.Top + card.Height Then
                c.BackColor = ThPanel
            End If
        End If
    Next c
End Sub
