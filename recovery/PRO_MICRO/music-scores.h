/* Musical 'scores' for tunes played by rocket */

const int ASZ_BEGINNING[] =
  {MUSIC_TEMPO, 70,
   MUSIC_TIME_SIG_LOWER, 4,
   
   NOTE_C4, -2, NOTE_G4, -2, NOTE_C5, -3,
   NOTE_REST, 8,
   NOTE_DS5, 8, NOTE_D5, -4,
   NOTE_REST, -2,
   MUSIC_STOP
};

const int ARMED_SIGNAL[] =
  {MUSIC_TEMPO, 160,
    MUSIC_TIME_SIG_LOWER, 4,

    NOTE_A5, -1, NOTE_A5, -1, NOTE_A5, -1, NOTE_A5, -1,
    NOTE_REST, -8,
    MUSIC_REPEAT
  };

const int THRUST_SIGNAL[] =
  {MUSIC_TEMPO, 320,
    MUSIC_TIME_SIG_LOWER, 4,

    NOTE_A5, -1, NOTE_A5, -1, NOTE_A5, -1,
    NOTE_REST, -1,
    MUSIC_REPEAT
  };

const int DESCENT_SIGNAL[] =
  {MUSIC_TEMPO, 200,
    MUSIC_TIME_SIG_LOWER, 4,

    NOTE_A5, -1, NOTE_A5, -1,
    NOTE_REST, -2,
    MUSIC_REPEAT
  };

const int RECOVERY_SIGNAL[] =
  {MUSIC_TEMPO, 140,
    MUSIC_TIME_SIG_LOWER, 4,

    NOTE_A5, -1, NOTE_A5, -1, NOTE_A5, -1,
    NOTE_REST, -1,
    MUSIC_REPEAT
  };
  
