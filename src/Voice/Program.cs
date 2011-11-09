// The following is a voice interpreter for the Kinect
// drawing on the Kinect API and inspiration from KinecTunes

using System;
using System.IO;
using System.Diagnostics;
using System.Collections.Generic;
using System.Linq;
using iTunesLib;
using Microsoft.Research.Kinect.Audio;
using Microsoft.Speech.AudioFormat;
using Microsoft.Speech.Recognition;

namespace Voice {
    class Program {
        // The following static global is the keycode for Kinect speech recognition
        private const string SpeechRecognitionID = "SR_MS_en-US_Kinect_10.0";

        /**
         * The following is a generic method to exit the program when an error occurs
         * 
         *   params: exit_message
         * 
         */
        public static void ExitProgram(string exit_message) {
            Console.WriteLine(exit_message);
            Console.WriteLine("Press any key to exit...");
            Console.ReadLine();
            Environment.Exit(1);
        }

        /**
         * The following method is called when speech is recognized and performs iTunes
         * controlling
         * 
         *   params: sender, e
         *   
         */
        static void SpeechWasRecognized(object sender, SpeechRecognizedEventArgs e) {
            // Only trust results over 0.95
            if (e.Result.Confidence >= 0.90) {
                Console.WriteLine("Recognized command '{0}'", e.Result.Text);
                if (e.Result.Text == "itunes play") {
                    Process.Start("scripts\\Play.vbs");
                } else if (e.Result.Text == "itunes pause") {
                    Process.Start("scripts\\Pause.vbs");
                } else if (e.Result.Text == "itunes stop") {
                    Process.Start("scripts\\Stop.vbs");
                } else if (e.Result.Text == "itunes mute") {
                    Process.Start("scripts\\Mute.vbs");
                } else if (e.Result.Text == "itunes volume up") {
                    Process.Start("scripts\\VolumeUp.vbs");
                } else if (e.Result.Text == "itunes volume down") {
                    Process.Start("scripts\\VolumeDown.vbs");
                } else if (e.Result.Text == "itunes next") {
                    Process.Start("scripts\\Next.vbs");
                } else if (e.Result.Text == "itunes previous") {
                    Process.Start("scripts\\Previous.vbs");
                } else if (e.Result.Text.Contains("itunes play album")) {
                    string album = String.Join(" ", e.Result.Text.Split(new char[] { ' ' }).Skip(3));
                    Process.Start("scripts\\PlayAlbum.vbs", string.Format("/album:\"{0}\"", album));
                } else if (e.Result.Text.Contains("itunes play artist")) {
                    string artist = String.Join(" ", e.Result.Text.Split(new char[] { ' ' }).Skip(3));
                    Process.Start("scripts\\PlayArtist.vbs", string.Format("/artist:\"{0}\"", artist));
                } else if (e.Result.Text.Contains("itunes play playlist")) {
                    string playlist = String.Join(" ", e.Result.Text.Split(new char[] { ' ' }).Skip(3));
                    Process.Start("scripts\\PlayPlaylist.vbs", string.Format("/playlist:\"{0}\"", playlist));
                } else if (e.Result.Text.Contains("itunes play song")) {
                    string song = String.Join(" ", e.Result.Text.Split(new char[] { ' ' }).Skip(3));
                    Process.Start("scripts\\PlaySong.vbs", string.Format("/song:\"{0}\"", song));
                } else if (e.Result.Text.Contains("itunes play")) {
                    string token = String.Join(" ", e.Result.Text.Split(new char[] { ' ' }).Skip(2));
                    Process.Start("scripts\\PlayPlaylist.vbs", string.Format("/playlist:\"{0}\"", token));
                    Process.Start("scripts\\PlayAlbum.vbs", string.Format("/album:\"{0}\"", token));
                    Process.Start("scripts\\PlayArtist.vbs", string.Format("/artist:\"{0}\"", token));
                    Process.Start("scripts\\PlaySong.vbs", string.Format("/song:\"{0}\"", token));
                }

            // Otherwise we assume no matching command found
            } else {
                Console.WriteLine("No Matching Command Found For Audio", e.Result.Text);
            }
        }

        /**
         * Application main method
         * 
         */
        public static void Main(string[] args) {
            // We get a source obj and initialize context specific variables
            using (KinectAudioSource source = new KinectAudioSource()) {
                source.FeatureMode = true;
                source.AutomaticGainControl = false;
                source.SystemMode = SystemMode.OptibeamArrayOnly;
                RecognizerInfo recognizer_info = SpeechRecognitionEngine.InstalledRecognizers().Where(r => r.Id == SpeechRecognitionID).FirstOrDefault();

                // Make sure we got the hook
                if (recognizer_info == null)
                    ExitProgram("There's no speech recognizer on your system.  Please install one from the README.");
                
                // Get a hook into iTunes using the COM library
                iTunesApp itunes_application = new iTunesApp();
                if (itunes_application == null)
                    ExitProgram("There was a problem getting access to iTunes.");

                using (SpeechRecognitionEngine speech_recognizer = new SpeechRecognitionEngine(recognizer_info.Id)) {
                    // First, we create a grammar with basic iTunes instructions
                    Choices basic_itunes_options = new Choices();
                    basic_itunes_options.Add("itunes play");
                    basic_itunes_options.Add("itunes pause");
                    basic_itunes_options.Add("itunes stop");
                    basic_itunes_options.Add("itunes next");
                    basic_itunes_options.Add("itunes previous");
                    basic_itunes_options.Add("itunes mute");
                    basic_itunes_options.Add("itunes volume up");
                    basic_itunes_options.Add("itunes volume down");

                    GrammarBuilder basic_itunes_grammar = new GrammarBuilder();
                    basic_itunes_grammar.Append(basic_itunes_options);

                    // Next, we make an iTunes library-specific set of grammars for granular control
                    //  The following is inspired by but not directly lifted from KinecTunes.  Credit
                    //  is due for inspiration though
                    Choices dynamic_itunes_options = new Choices();
                    IITLibraryPlaylist itunes_library = itunes_application.LibraryPlaylist;

                    // The library is one-based so we go through each track and pull out relevant data into the grammar
                    //   We maintain lists to avoid duplicate grammars, which can cause errors with the Kinect
                    List<string> artists = new List<string>();
                    List<string> songs = new List<string>();
                    List<string> albums = new List<string>();
                    for (int i = 1; i <= itunes_library.Tracks.Count; i++) {
                        IITTrack track = itunes_library.Tracks[i];
                        if (track != null && track.KindAsString.Contains("audio")) {
                            if (track.Name != null && !artists.Contains(track.Name)) {
                                dynamic_itunes_options.Add(string.Format("itunes play {0}", track.Name));
                                dynamic_itunes_options.Add(string.Format("itunes play song {0}", track.Name));
                                songs.Add(track.Name);
                            }
                            if (track.Artist != null && !artists.Contains(track.Artist)) {
                                dynamic_itunes_options.Add(string.Format("itunes play {0}", track.Artist)); 
                                dynamic_itunes_options.Add(string.Format("itunes play artist {0}", track.Artist));
                                artists.Add(track.Artist);
                            }
                            if (track.Album != null && !albums.Contains(track.Album)) {
                                dynamic_itunes_options.Add(string.Format("itunes play {0}", track.Album)); 
                                dynamic_itunes_options.Add(string.Format("itunes play album {0}", track.Album));
                                albums.Add(track.Album);
                            }
                        }
                    }

                    // Treat the playlists specially
                    List<string> playlists = new List<string>();
                    for (int i = 1; i <= itunes_application.LibrarySource.Playlists.Count; i++) {
                        var playlist = itunes_application.LibrarySource.Playlists[i];
                        if (playlist.Name != null && !playlists.Contains(playlist.Name)) {
                            playlists.Add(playlist.Name);
                            dynamic_itunes_options.Add(string.Format("itunes play {0}", playlist.Name)); 
                            dynamic_itunes_options.Add(string.Format("itunes play playlist {0}", playlist.Name));
                        }
                    }

                    GrammarBuilder dynamic_itunes_grammar = new GrammarBuilder();
                    dynamic_itunes_grammar.Append(dynamic_itunes_options);

                    // Load all the grammars into a grammar object, then our speech recognition engine
                    Grammar itunes_grammar_one = new Grammar(basic_itunes_grammar);
                    Grammar itunes_grammar_two = new Grammar(dynamic_itunes_grammar);

                    // Notice that we don't care when the speech is hypothesized or rejected, only accepted
                    speech_recognizer.LoadGrammar(itunes_grammar_one);
                    speech_recognizer.LoadGrammar(itunes_grammar_two);
                    speech_recognizer.SpeechRecognized += SpeechWasRecognized;

                    using (Stream s = source.Start()) {
                        speech_recognizer.SetInputToAudioStream(s, new SpeechAudioFormatInfo(EncodingFormat.Pcm,
                                                                                             16000, 16, 1, 32000,
                                                                                             2, null));
                        Console.Write("Kinect has loaded iTunes Library.  Initializing speech recognition...");
                        // Why is signal handling so difficult in C#?  Whatever, let's just use any keystrokes for interrupt
                        speech_recognizer.RecognizeAsync(RecognizeMode.Multiple);
                        Console.WriteLine("OK.\nPress any key to exit...");
                        Console.ReadLine();
                        speech_recognizer.RecognizeAsyncStop();
                    }
                }
            }
        }
    }
}
