#include <music.h>
#include <pitches.h>

int asz_music[] = {MUSIC_TEMPO, 70,
                   MUSIC_TIME_SIG_LOWER, 4,

                   NOTE_C4, -2, NOTE_G4, -2, NOTE_C5, -3,
                   NOTE_REST, 8,
                   NOTE_DS5, 8, NOTE_D5, -4,
                   NOTE_REST, -2,

                   NOTE_C4, -2, NOTE_G4, -2, NOTE_C5, -3,
                   NOTE_REST, 8,
                   NOTE_DS5, 8, NOTE_E5, -4,
                   NOTE_REST, -2,
                   
                   NOTE_C4, -2, NOTE_G4, -2, NOTE_C5, -3,
                   NOTE_REST, 8,
                   NOTE_E5, 8, NOTE_F5, -4,

                   NOTE_A5, 8, NOTE_B5, 8,
                   NOTE_C6, -2, NOTE_D6, -1,
                   NOTE_E5, 8, NOTE_F5, 8, 
                   NOTE_G5, 4, NOTE_E5, 4, NOTE_C5, 4, NOTE_G4, 4,
                   
                   NOTE_E5, 8, NOTE_F5, 8,
                   NOTE_G5, 4, NOTE_E5, 4, NOTE_C5, 4, NOTE_G4, 4,

                   NOTE_E5, 8, NOTE_F5, 8,
                   NOTE_G5, -3, NOTE_A5, -3, NOTE_B5, -3, NOTE_C6, -3,
                   MUSIC_STOP
};


Music mus(2);

void setup() {
  // put your setup code here, to run once:
  mus.music_array = asz_music;
  //// Uncomment for debugging
  /* Serial.begin(9600); */
  /* mus.print_debuging_info = true; */
  mus.is_playing = true;
}

void loop() {
  // put your main code here, to run repeatedly:
  mus.update();
}
