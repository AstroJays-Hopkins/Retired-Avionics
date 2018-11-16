/* Music generating library --- can play music while running other code 
   provided the update() function is called periodically */

class Music {
 public:
  Music(int pin);
  /* void start();   // start playing music */
  /* void stop();    // stop the music! */
  void update();  // must be called periodically for the music to be played correctly; manages actual control of buzzer
  void debug_print();

  bool is_playing = false;
  bool is_slurred = false;
  bool print_debugging_info = false;

  int *music_array;
  int current_music_position = 0;
  int beat_note;

 private:
  int _pin;
  unsigned long _current_note_start_time;
  unsigned long _current_note_length;
  unsigned long _beat_length;
  int _note_gap = 50;  // gap in between notes, in milliseconds.

  bool _is_in_between_notes = false;

  void _play_note(int note_freq, int length);
  void _interpret_pending_instructions_and_play_next_note();
  int _next_music_array_value();
  
};

enum music_control {
  MUSIC_TEMPO,
  MUSIC_TIME_SIG_LOWER,
  MUSIC_SLUR,
  MUSIC_NOSLUR,
  MUSIC_STOP,
  MUSIC_REPEAT
  // ^^ should only be used at the end of a piece; jumps to beginning
  //    nothing in the music_array after MUSIC_REPEAT will be played
};

// Some macros to make writing music easier:

// Creates a note with frequency N and with a length of 1.5 * L
// (same effect of putting a dot next to a note in sheet music)
// Must be used only for notes shorter than a whole note.
#define MUSIC_DOT(N,L) MUSIC_SLUR, N, L, N, (L*2), MUSIC_NOSLUR
