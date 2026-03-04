#ifndef TETRIS_H
#define TETRIS_H

#include <stdint.h>
#include "LPC17xx.h"
#include "GLCD.h"

#define BOARD_ROWS 20
#define BOARD_COLS 10
#define BLOCK_SIZE 10    // Pixels per block
#define BOARD_X    10    // X start position of board on screen
#define BOARD_Y    10    // Y start position of board on screen

#define COL_BACKGROUND Black
#define COL_GRID       White
#define COL_TEXT       White

#define COL_POWERUP_HALF  0xFFE0 // Giallo Elettrico
#define COL_POWERUP_SLOW  0xF81F // Magenta Neon

typedef enum {
    STATE_INTRO,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAMEOVER
} GameState_t;

// Stuttura tetromino
typedef struct {
    int8_t blocks[4][2];
    uint16_t color;
    int x, y; 
} Tetromino_t;

typedef struct {
    GameState_t state;
    uint16_t board[BOARD_ROWS][BOARD_COLS];
    
    Tetromino_t current;
    Tetromino_t prev_current; // posizione da frame precedente
    Tetromino_t next;
    
    uint32_t score;
    uint32_t high_score;
    uint32_t lines_cleared;
    
    // 0 = non aggiorna
    // 1 = aggiorna solo pezzo
    // 2 = aggiormanento completo
    volatile uint8_t flag_update_display; 
    
    volatile int hard_drop_cooldown; // per evitare caduta multipla

    // powerus
    int timer_powerup_slow;
    int speed_override;

} TetrisGame_t;

extern TetrisGame_t game;

void Tetris_Init(void);
void Tetris_Start(void);
void Tetris_Pause(void);
void Tetris_GameOver(void);

void Action_MoveLeft(void);
void Action_MoveRight(void);
void Action_Rotate(void);
void Action_SoftDrop(void);
void Action_HardDrop(void);
void Action_GravityTick(void);

void Tetris_Draw_Intro(void);
void Tetris_Draw_Board(void);
void Tetris_Draw_UI(void);
void Tetris_Draw_GameOver(void);

#endif