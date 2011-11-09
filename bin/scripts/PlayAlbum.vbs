rem This script will cause iTunes play the first song in the specified album.

rem Examples:
rem	    PlayAlbum "5 stars"
rem	    PlayAlbum /album:"5 stars"
rem Be sure to enclose the album name in quotes if it includes spaces.

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
Dim ArgAlbum 'As String
Dim ArgAlbumLcase 'As String
Dim Album 'As IITAlbum
Dim TempAlbum 'As IITAlbum
Dim SyntaxErr 'As Boolean
Dim Msg 'As String


' Init
ArgAlbum = GetNamedArg("album")
Set Album = Nothing

' Special support: if they provide exactly one unnamed argument, then
' assume that it's the album name (assuming /album hasn't been specified)
If (ArgAlbum = "") And (Wscript.Arguments.Unnamed.Count = 1) Then
	' Use it!
	ArgAlbum = Wscript.Arguments.Unnamed.Item(0)
End If


' Bounds check
SyntaxErr = False
If ArgAlbum = "" Then SyntaxErr = True

If SyntaxErr = True Then
    Msg = "Please specify the album to play." + vbCrLf + vbCrLf
	Msg = Msg + "Examples:" + vbCrLf
	Msg = Msg + vbTab + "PlayAlbum ""5 stars""" + vbCrLf
	Msg = Msg + vbTab + "PlayAlbum /album:""5 stars"""
    Wscript.echo Msg
    Wscript.Quit
End If


' Connect to iTunes app
Set iTunes = Wscript.CreateObject("iTunes.Application")


' Find the specified album
Set Playlist = iTunes.LibraryPlaylist
Set Tracks_Album = Playlist.Search(ArgAlbum, 3)

' Did we find one?
If Tracks_Album Is Nothing Then
    'Wscript.echo "The album """ + ArgAlbum + """ could not be found."
    Wscript.Quit
Else
	' Start playing the album
	Randomize
	Set Track = Tracks_Album.Item(Int(Rnd * Tracks_Album.Count) + 1)
	Track.Play
End If


' Done; clean up
Set Track = Nothing
Set iTunes = Nothing


Rem End of script.
