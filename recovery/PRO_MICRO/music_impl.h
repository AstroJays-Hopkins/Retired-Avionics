#include "Arduino.h"
//#include "music.h"
#include "pitches.h"

Music::Music(int pin) {
  _pin = pin;
  pinMode(_pin, OUTPUT);
}

int Music::_next_music_array_value() {
  return music_array[current_music_position + 1];
}

void Music::_play_note(int note_freq, int length) {
  // Plays the note of the specified frequency and length.
  // Note length is actually inverse length --- 4 represents a quarter note
  // this does not play notes with lengths of zero;
  // it also does not skip to the next note if the current note's length is zero.
  if (length != 0) {
    if (length > 0) {
      _current_note_length = (unsigned long)((_beat_length * beat_note) / length);
    } else if (length < 0) {
      _current_note_length = (unsigned long)(_beat_length * (- length));
    }
    // Play nothing if the note is a rest; otherwise, play note.
    if (note_freq == NOTE_REST) {
      noTone(_pin);
    } else {
      tone(_pin, (unsigned int)note_freq);
    }
    _current_note_start_time = millis();
  }
}

void Music::_interpret_pending_instructions_and_play_next_note () {
  // Function interprets any instructions at and after the _current_position
  // in the_music_array up to the next note, which is then played.
  if (print_debugging_info) {
    debug_print();
  }
  int control_number = music_array[current_music_position];
  switch (control_number) {
  case MUSIC_TEMPO:
    // Set the length of one beat to next value in array
    _beat_length = 1000 / (float)(_next_music_array_value() / 60);
    current_music_position += 2;
    // interpret next instruction
    _interpret_pending_instructions_and_play_next_note();
    break;
  case MUSIC_TIME_SIG_LOWER:
    // set the next value in _music_array to note that corresponds to the beat
    // (lower number in a musical composition's time signature)
    beat_note = _next_music_array_value();
    current_music_position += 2;
    _interpret_pending_instructions_and_play_next_note();
    break;
  case MUSIC_SLUR:
    // Plays notes without gaps in between them
    is_slurred = true;
    current_music_position++;
    _interpret_pending_instructions_and_play_next_note();
    break;
  case MUSIC_NOSLUR:
    // Plays notes with gaps in between them
    is_slurred = false;
    current_music_position++;
    _interpret_pending_instructions_and_play_next_note();
    break;
  case MUSIC_STOP:
    // Stop the music!
    is_playing = false;
    noTone(_pin);
    break;
  case MUSIC_REPEAT:
    // Jump back to beginning!
    current_music_position = 0;
    break;
  default:
    // If control_number is not recognized, assume that it is a note and play it
    _play_note(control_number, _next_music_array_value());
    current_music_position += 2;
    break;
  }
}


void Music::update () {
  // If music is supposed to be playing and it's time to stop or start
  // playing a note ...
  if(is_playing && ((millis() - _current_note_start_time) >= _current_note_length))
    {
      // If we just were in the brief amount of time in between notes,
      // play the next note.
      if (_is_in_between_notes) {
        _is_in_between_notes = false;
        _interpret_pending_instructions_and_play_next_note();
      } else {
        // If we were not previously waiting in between notes,
        // then we must still be playing a note and must stop playing it.
        if (is_slurred) {
          // If we are slurring, proceed immediately to the next note;
          // do not put a gap in between notes.
          _interpret_pending_instructions_and_play_next_note();
        } else {
          // If we are not slurring, silence the buzzer
          // and set up a delay before the next note.
          noTone(_pin);
          _current_note_start_time = millis();
          _current_note_length = _note_gap;
          _is_in_between_notes = true;
        }
      }        
    }
  if (!is_playing) {
    // If not playing, silence the buzzer.
    noTone(_pin);
  }
}


void Music::debug_print () {
  // Assumes that the serial interface has already been initialized.
  // Dumps a fair bit of information to serial line.
  Serial.write("\n Current Position: ");
  Serial.print(current_music_position);
  Serial.write("\n Control Number: ");
  Serial.print(music_array[current_music_position]);
  Serial.write("\n Next value: ");
  Serial.print(_next_music_array_value());

  Serial.write("\n Beat length: ");
  Serial.print(_beat_length);
  Serial.write("\n Note length: ");
  Serial.print(_current_note_length);
  Serial.write("\n Current start time: ");
  Serial.print(_current_note_start_time);
  Serial.print("\n");
  Serial.write("\n Current time: ");
  Serial.print(millis());
  Serial.print("\n");
}
