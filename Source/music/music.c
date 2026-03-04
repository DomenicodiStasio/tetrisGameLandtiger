#include "LPC17xx.h"
#include "music.h"
#include "../timer/timer.h"

static int current_song_type = 0; 
static int current_note = 0;



// --- SPARTITI ---

// CANZONE TETRIS
NOTE tetris_theme[] = {
		
    {e4, time_semiminima}, {b3, time_croma}, {c4, time_croma}, {d4, time_semiminima}, 
    {c4, time_croma}, {b3, time_croma}, {a3, time_semiminima}, {a3, time_croma}, 
    {c4, time_croma}, {e4, time_semiminima}, {d4, time_croma}, {c4, time_croma},
    {b3, time_semiminima}, {b3, time_croma}, {c4, time_croma}, {d4, time_semiminima},
    {e4, time_semiminima}, {c4, time_semiminima}, {a3, time_semiminima}, {a3, time_semiminima},
    {pause, time_croma}, 
		
    {d4, time_semiminima}, {d4, time_croma}, {f4, time_croma}, {a4, time_semiminima}, 
    {g4, time_croma}, {f4, time_croma}, {e4, time_semiminima}, {e4, time_croma}, {c4, time_croma}, 
    {e4, time_semiminima}, {d4, time_croma}, {c4, time_croma},
    {b3, time_semiminima}, {b3, time_croma}, {c4, time_croma}, {d4, time_semiminima}, 
    {e4, time_semiminima}, {c4, time_semiminima}, {a3, time_semiminima}, {a3, time_semiminima},
    {pause, time_semiminima}
};
const int tetris_length = sizeof(tetris_theme) / sizeof(NOTE);

// CANZONE GAME OVER
NOTE gameover_theme[] = {
    {a3, time_semiminima}, {g3, time_semiminima}, {f3, time_semiminima},
    {e3, time_semiminima}, {d3, time_semiminima}, {c3, time_semiminima},
    {b2, time_minima},     {pause, time_croma},   {b2, time_semibreve}
};
const int gameover_length = sizeof(gameover_theme) / sizeof(NOTE);


// --- FUNZIONI MUSICA ---

void Music_Start(void) {
		current_song_type = 0;
    current_note = 0;
    playNote(tetris_theme[current_note]);
}

void Music_GameOver(void) {
    current_song_type = 1;
    current_note = 0;
    playNote(gameover_theme[current_note]);
}

void Music_Stop(void) {
    disable_timer(0);
    reset_timer(0);
    disable_timer(1);
    reset_timer(1);
}

void Music_PlayNext(void) {
    current_note++;
    
    if (current_song_type == 0) {
        if(current_note >= tetris_length) {
            current_note = 0;
        }
        playNote(tetris_theme[current_note]);
    } 
    else {
        if(current_note >= gameover_length) {
            Music_Stop();
        } else {
            playNote(gameover_theme[current_note]);
        }
    }
}

void playNote(NOTE note)
{
		if(note.freq != pause)
		{
				reset_timer(0);
				init_timer(0, note.freq);
				enable_timer(0);
		}
		reset_timer(1);
		init_timer(1, note.duration);
		enable_timer(1);
}

BOOL isNotePlaying(void)
{
		return ((LPC_TIM0->TCR != 0) || (LPC_TIM1->TCR != 0));
}