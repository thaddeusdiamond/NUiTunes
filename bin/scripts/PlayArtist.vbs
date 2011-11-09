rem This script will cause iTunes play the first song in the specified artist.

rem Examples:
rem	    PlayArtist "5 stars"
rem	    PlayArtist /artist:"5 stars"
rem Be sure to enclose the artist name in quotes if it includes spaces.

rem Copyright 2004 Maximized Software, Inc.  All rights reserved.
rem More info is available at:  http://www.maximized.com/freeware/ScriptsForiTunes/
rem Version 2004-12-09-1


rem =================================================
function GetNamedArg(ArgName)


with Wscript.Arguments.Named
	if .Exists(ArgName) then
		GetNamedArg = .Item(ArgName)
	else
		GetNamedArg = ""
	end if
end with


end function
rem =================================================




rem =================================================
rem Main script:


' Variables
Dim iTunes 'As iTunes
Dim ArgArtist 'As String
Dim ArgArtistLcase 'As String
Dim Artist 'As IITArtist
Dim TempArtist 'As IITArtist
Dim SyntaxErr 'As Boolean
Dim Msg 'As String


' Init
ArgArtist = GetNamedArg("artist")
Set Artist = Nothing

' Special support: if they provide exactly one unnamed argument, then
' assume that it's the artist name (assuming /artist hasn't been specified)
If (ArgArtist = "") And (Wscript.Arguments.Unnamed.Count = 1) Then
	' Use it!
	ArgArtist = Wscript.Arguments.Unnamed.Item(0)
End If


' Bounds check
SyntaxErr = False
If ArgArtist = "" Then SyntaxErr = True

If SyntaxErr = True Then
    Msg = "Please specify the artist to play." + vbCrLf + vbCrLf
	Msg = Msg + "Examples:" + vbCrLf
	Msg = Msg + vbTab + "PlayArtist ""5 stars""" + vbCrLf
	Msg = Msg + vbTab + "PlayArtist /artist:""5 stars"""
    Wscript.echo Msg
    Wscript.Quit
End If


' Connect to iTunes app
Set iTunes = Wscript.CreateObject("iTunes.Application")


' Find the specified artist
Set Playlist = iTunes.LibraryPlaylist
Set Tracks_Artist = Playlist.Search(ArgArtist, 2)

' Did we find one?
If Tracks_Artist Is Nothing Then
    'Wscript.echo "The artist """ + ArgArtist + """ could not be found."
    Wscript.Quit
Else
	' Start playing the artist
	Randomize
	Set Track = Tracks_Artist.Item(Int(Rnd * Tracks_Artist.Count) + 1)
	Track.Play
End If


' Done; clean up
Set Track = Nothing
Set iTunes = Nothing


Rem End of script.
