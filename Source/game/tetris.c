#include "tetris.h"
#include <stdlib.h>
#include <stdio.h>
#include "../GLCD/GLCD.h"
#include "music/music.h"

TetrisGame_t game;

// Forme dei tetromini
const int8_t SHAPES[7][4][2] = {
    {{0,0}, {-1,0}, {1,0}, {2,0}},  // I 
    {{0,0}, {1,0}, {0,1}, {1,1}},   // O 
    {{0,0}, {-1,0}, {1,0}, {0,1}},  // T
    {{0,0}, {-1,0}, {1,0}, {-1,1}}, // L 
    {{0,0}, {-1,0}, {1,0}, {1,1}},  // J 
    {{0,0}, {1,0}, {0,1}, {-1,1}},  // S 
    {{0,0}, {-1,0}, {0,1}, {1,1}}   // Z 
};
/*
const uint16_t SHAPE_COLORS[7] = {Cyan, Yellow, Magenta, White, Blue, Green, Red};
*/
const uint16_t SHAPE_COLORS[7] = {
    0x067F, // I (Ciano Medio)
    0xFD20, // O (Arancione)
    0x913F, // T (Viola/Lilla)
    0xC618, // L (Grigio Argento)
    0x031F, // J (Blu Reale)
    0x05E0, // S (Verde Smeraldo)
    0xD800  // Z (Rosso Cremisi)
};

int Check_Collision(Tetromino_t t) {
    int i, nx, ny;
    for(i=0; i<4; i++) {
        nx = t.x + t.blocks[i][0];
        ny = t.y + t.blocks[i][1];
        
        if (nx < 0 || nx >= BOARD_COLS || ny >= BOARD_ROWS) return 1; 
        if (ny >= 0 && game.board[ny][nx] != COL_BACKGROUND) return 1; 
    }
    return 0;
}

void Generate_Piece(Tetromino_t *p) {
    int idx, i;
    idx = rand() % 7; // Comment this --- 28374 DEBUG FLAG
		//idx=0;	//-- for debug purpose - genera sempre la I --- 28374 DEBUG FLAG
    p->color = SHAPE_COLORS[idx];
    p->x = BOARD_COLS / 2 - 1;
    p->y = 0; 
    for(i=0; i<4; i++) {
        p->blocks[i][0] = SHAPES[idx][i][0];
        p->blocks[i][1] = SHAPES[idx][i][1];
    }
}

void Tetris_Draw_Next_Piece(void) {
    int i, k, x, y, px, py;
    
    // Pulisci area anteprima
    for(i=0; i<60; i++) 
        LCD_DrawLine(150+i, 220, 150+i, 270, Black);
        
    for(i=0; i<4; i++) {
        x = game.next.blocks[i][0];
        y = game.next.blocks[i][1];
        px = 170 + x*BLOCK_SIZE;
        py = 240 + y*BLOCK_SIZE;
        
        for(k=0; k<BLOCK_SIZE-1; k++) 
             LCD_DrawLine(px+k, py, px+k, py+BLOCK_SIZE-1, game.next.color);
    }
}

void Tetris_Init(void) {
    if(game.high_score > 1000000) game.high_score = 0; 
    game.state = STATE_INTRO;
    game.hard_drop_cooldown = 0; //
    Tetris_Draw_Intro();
}

void Tetris_Draw_UI(void) {
    LCD_DrawLine(BOARD_X - 1, BOARD_Y - 1, BOARD_X + (BOARD_COLS*BLOCK_SIZE), BOARD_Y - 1, White);
    LCD_DrawLine(BOARD_X - 1, BOARD_Y + (BOARD_ROWS*BLOCK_SIZE), BOARD_X + (BOARD_COLS*BLOCK_SIZE), BOARD_Y + (BOARD_ROWS*BLOCK_SIZE), White);
    LCD_DrawLine(BOARD_X + (BOARD_COLS*BLOCK_SIZE), BOARD_Y - 1, BOARD_X + (BOARD_COLS*BLOCK_SIZE), BOARD_Y + (BOARD_ROWS*BLOCK_SIZE), White);
	  LCD_DrawLine(BOARD_X - 1, BOARD_Y - 1, BOARD_X - 1, BOARD_Y + (BOARD_ROWS*BLOCK_SIZE), White);
    
    GUI_Text(130, 20, (uint8_t *)"CURRENT SCORE", White, Black);
    GUI_Text(130, 80, (uint8_t *)"BEST SCORE", White, Black);
    GUI_Text(130, 140, (uint8_t *)"LINES CLEARED", White, Black);
    GUI_Text(130, 200, (uint8_t *)"NEXT PIECE", White, Black);
}

void Tetris_Start(void) {
    
		Music_Start();
		int x, y;
    for(y=0; y<BOARD_ROWS; y++) 
        for(x=0; x<BOARD_COLS; x++) 
            game.board[y][x] = COL_BACKGROUND;

    game.score = 0;
    game.lines_cleared = 0;
    game.state = STATE_PLAYING;
    game.hard_drop_cooldown = 0;//
    
    Generate_Piece(&game.current);
    Generate_Piece(&game.next);
    game.prev_current = game.current; 
    
    LCD_Clear(COL_BACKGROUND);
    Tetris_Draw_UI();
    
    game.flag_update_display = 2;
}

void Tetris_GameOver(void) {
		Music_Stop();
	  Music_GameOver();
    game.state = STATE_GAMEOVER;
    if(game.score > game.high_score) game.high_score = game.score;
    
    game.flag_update_display = 3; 
}

void Trigger_Update(void) {
    game.flag_update_display = 1; 
}

// EXTRAPOINT2
void Generate_Malus(void) {
    int r, c;
    int blocks_placed = 0;
    int random_col;

    // Sposta tutte le righe su di 1
    for(r = 0; r < BOARD_ROWS - 1; r++) {
        for(c = 0; c < BOARD_COLS; c++) {
            game.board[r][c] = game.board[r+1][c];
        }
    }

    // Pulisce ultima riga
    for(c = 0; c < BOARD_COLS; c++) {
        game.board[BOARD_ROWS-1][c] = COL_BACKGROUND;
    }

    // Genera nuova riga 7 blocchi a caso
    while(blocks_placed < 7) {
        random_col = rand() % BOARD_COLS; // Colonna a caso tra 0 e 9
        
        // Se lo spazio è vuoto metti i blocco
        if(game.board[BOARD_ROWS-1][random_col] == COL_BACKGROUND) {
            game.board[BOARD_ROWS-1][random_col] = 0x7BEF;  // Grigio Ferro
            blocks_placed++;
        }
    }
}
// -------------------------------
// --- POWERUP FUNCTIONS ---
// -------------------------------
void Spawn_Powerup(void) {
    int attempts = 0;
    int r, c;
    int found = 0;
    
    // Ricerca blocco non vuoto da rimpiazzare
    while(attempts < 50 && !found) {
        r = rand() % BOARD_ROWS;
        c = rand() % BOARD_COLS;
        // Eslcudendo vuoti e blocchi powerups
        if (game.board[r][c] != COL_BACKGROUND && 
            game.board[r][c] != COL_POWERUP_HALF && 
            game.board[r][c] != COL_POWERUP_SLOW) {
            
            // randomizzazione tipologia powerup
            if (rand() % 2 == 0) {
                game.board[r][c] = COL_POWERUP_HALF;
            } else {
                game.board[r][c] = COL_POWERUP_SLOW;
            }
            found = 1;
        }
        attempts++;
    }
}

void Effect_ClearHalf(void) {
    int r, c;
    int first_occupied_row = BOARD_ROWS; 

    for(r = 0; r < BOARD_ROWS; r++) {
        for(c = 0; c < BOARD_COLS; c++) {
            if(game.board[r][c] != COL_BACKGROUND) {
                first_occupied_row = r;
                goto found_height; 
            }
        }
    }
found_height:;

    int total_occupied = BOARD_ROWS - first_occupied_row;
    
    if (total_occupied < 2) return; 

    int rows_to_clear = total_occupied / 2; 
    int start_row = BOARD_ROWS - rows_to_clear; 
    
    // CALCOLO PUNTEGGIO
    int score_bonus = 0;
    
    // I gruppi da 4 valgono 800
    int groups_of_4 = rows_to_clear / 4;
    score_bonus += (groups_of_4 * 800);
    
    // Le righe restanti valgono 100 punti l'una
    int remainder = rows_to_clear % 4;
    score_bonus += (remainder * 100);
    
    game.score += score_bonus;
    game.lines_cleared += rows_to_clear;
    
    // pulizia
    for(r = start_row - 1; r >= 0; r--) {
        for(c = 0; c < BOARD_COLS; c++) {
            game.board[r + rows_to_clear][c] = game.board[r][c];
            game.board[r][c] = COL_BACKGROUND; 
        }
    }
    for(r = 0; r < rows_to_clear; r++) {
         for(c = 0; c < BOARD_COLS; c++) game.board[r][c] = COL_BACKGROUND;
    }
}

void Effect_SlowDown(void) {
    // RIT 50ms: 1 sec - 20 tick --- 15 sec = 300 tick
    game.timer_powerup_slow = 300;
    game.speed_override = 1;
}

void Play_SFX_LineClear(void) {
    NOTE coin_sound = { (FREQUENCY)420, time_croma }; 
    
    playNote(coin_sound);
}

void Play_SFX_Lock(void) {
    NOTE clear_sound = { c5, time_biscroma }; 
    
    playNote(clear_sound);
}


void Lock_Piece(void) {
		Play_SFX_Lock();
    int i, nx, ny, y, x, kx, ky, full;
    int lines_in_this_move = 0;
    int powerup_activated = 0; // 0=None, 1=Half, 2=Slow
    
    int old_lines_count = game.lines_cleared;

    // Lock piece
    for(i=0; i<4; i++) {
        nx = game.current.x + game.current.blocks[i][0];
        ny = game.current.y + game.current.blocks[i][1];
        if(ny >= 0 && ny < BOARD_ROWS && nx >= 0 && nx < BOARD_COLS) {
            game.board[ny][nx] = game.current.color;
        }
    }
    game.score += 10;

    // controllo linee piene e powerup
    for(y=0; y<BOARD_ROWS; y++) {
        full = 1;
        powerup_activated = 0; // Reset for each row
        
        for(x=0; x<BOARD_COLS; x++) {
            if(game.board[y][x] == COL_BACKGROUND) { 
                full = 0; 
                break; 
            }
            // se linea piena contiene un powerup
            if(game.board[y][x] == COL_POWERUP_HALF) powerup_activated = 1;
            else if(game.board[y][x] == COL_POWERUP_SLOW) powerup_activated = 2;
        }
        
        if(full) {
            // --- POWERUP ---
            if (powerup_activated == 1) {
                Effect_ClearHalf();
                lines_in_this_move = 0;
                break; 
            }
            else if (powerup_activated == 2) {
                Effect_SlowDown();
            }
            
            // --- STANDARD ---
            lines_in_this_move++;
            for(ky=y; ky>0; ky--) 
                for(kx=0; kx<BOARD_COLS; kx++) 
                    game.board[ky][kx] = game.board[ky-1][kx];
            for(kx=0; kx<BOARD_COLS; kx++) game.board[0][kx] = COL_BACKGROUND;
            y--; 
        }
    }
    
    // Halfclear gestisce lo score in autonomia
		// calcolo punteggio per quelle in eccesso
    if(lines_in_this_move > 0) {
				Play_SFX_LineClear();
        game.score += (100 * lines_in_this_move); 
        game.lines_cleared += lines_in_this_move;
    }

    // --- SPAWN LOGIC ---
    
    // Powerup Spawn (ogni 5 linee - ExtraPoint2)
    if ( (old_lines_count / 5) < (game.lines_cleared / 5) ) {
         Spawn_Powerup();
    }

    // Malus Spawn (ogni 10 linee - ExtraPoint2)
    if ( (old_lines_count / 10) < (game.lines_cleared / 10) ) {
         Generate_Malus();
    }

    // next tetromino
    game.current = game.next;
    Generate_Piece(&game.next);
    game.prev_current = game.current; 

    if(Check_Collision(game.current)) {
        Tetris_GameOver();
    } else {
        game.flag_update_display = 2; 
    }
}


/*	
		FUNZIONI ACTION
*/
void Action_GravityTick(void) {
    if(game.state != STATE_PLAYING) return;
    
    if(game.hard_drop_cooldown > 0) game.hard_drop_cooldown--;

    game.prev_current = game.current; 
    game.current.y++;
    
    if(Check_Collision(game.current)) {
        game.current.y--; 
        Lock_Piece();
    } else {
        Trigger_Update();
    }
}

void Action_MoveLeft(void) {
    if(game.state != STATE_PLAYING) return;
    game.prev_current = game.current;
    game.current.x--;
    if(Check_Collision(game.current)) game.current.x++;
    else Trigger_Update();
}

void Action_MoveRight(void) {
    if(game.state != STATE_PLAYING) return;
    game.prev_current = game.current;
    game.current.x++;
    if(Check_Collision(game.current)) game.current.x--;
    else Trigger_Update();
}

void Action_Rotate(void) {
    int i;
    int8_t old_x, old_y;
    Tetromino_t backup;

    if(game.state != STATE_PLAYING) return;
    game.prev_current = game.current;
    
    backup = game.current;
    for(i=0; i<4; i++) {
        old_x = game.current.blocks[i][0];
        old_y = game.current.blocks[i][1];
        game.current.blocks[i][0] = -old_y;
        game.current.blocks[i][1] = old_x;
    }
    
    if(Check_Collision(game.current)) game.current = backup;
    else Trigger_Update();
}

void Action_HardDrop(void) {
    if(game.state != STATE_PLAYING) return;
    if(game.hard_drop_cooldown > 0) return; 

    game.prev_current = game.current; 
    
    while(!Check_Collision(game.current)) {
        game.current.y++;
    }
    game.current.y--; 
    Lock_Piece();
    game.hard_drop_cooldown = 0; //
}

void Action_SoftDrop(void) {
	//
}

void Tetris_Draw_Board(void) {
    uint8_t mode;
    int x, y, i, k, px, py;
    uint16_t color;
    char buff[20];

    mode = game.flag_update_display;
    if (mode == 0) return;
    game.flag_update_display = 0; 

    // MODE 3: GAME OVER SCREEN
    if (mode == 3) {
        LCD_Clear(Black);
        GUI_Text(80, 100, (uint8_t *)"GAME OVER", Red, Black);
        
        sprintf(buff, "SCORE: %d", game.score);
        GUI_Text(80, 140, (uint8_t *)buff, White, Black);
        GUI_Text(40, 180, (uint8_t *)"PRESS KEY1 TO RESTART", Yellow, Black);
        return;
    }
    
    if (game.state == STATE_GAMEOVER) return;

    // MODE 2: AGGIORNAMENTO TOTALE
    if(mode == 2) {
        for(y=0; y<BOARD_ROWS; y++) {
            for(x=0; x<BOARD_COLS; x++) {
                color = game.board[y][x];
                px = BOARD_X + x*BLOCK_SIZE;
                py = BOARD_Y + y*BLOCK_SIZE;
                
                for(i=0; i<BLOCK_SIZE-1; i++) { 
                    LCD_DrawLine(px+i, py, px+i, py+BLOCK_SIZE-1, color);
                }
            }
        }
				/*
				// --- INIZIO CODICE GRIGLIA ---
        uint16_t grid_color = 0x2104; // Un grigio molto scuro e sottile (Hex RGB 565)
        
        for(y=0; y<BOARD_ROWS; y++) {
            for(x=0; x<BOARD_COLS; x++) {
                // Disegna la griglia SOLO se la cella è vuota (Background)
                // Così non roviniamo i pezzi colorati
                if(game.board[y][x] == COL_BACKGROUND) {
                    px = BOARD_X + x*BLOCK_SIZE;
                    py = BOARD_Y + y*BLOCK_SIZE;
                    
                    // Disegna bordo destro della cella
                    LCD_DrawLine(px + BLOCK_SIZE - 1, py, px + BLOCK_SIZE - 1, py + BLOCK_SIZE - 1, grid_color);
                    // Disegna bordo inferiore della cella
                    LCD_DrawLine(px, py + BLOCK_SIZE - 1, px + BLOCK_SIZE - 1, py + BLOCK_SIZE - 1, grid_color);
                }
            }
        }
        // --- FINE CODICE GRIGLIA ---
        */
        GUI_Text(130, 40, (uint8_t *)"          ", White, Black);
        sprintf(buff, "%d", game.score);
        GUI_Text(130, 40, (uint8_t *)buff, White, Black);
        
        GUI_Text(130, 100, (uint8_t *)"          ", White, Black);
        sprintf(buff, "%d", game.high_score);
        GUI_Text(130, 100, (uint8_t *)buff, White, Black);
        
        GUI_Text(130, 160, (uint8_t *)"          ", White, Black);
        sprintf(buff, "%d", game.lines_cleared);
        GUI_Text(130, 160, (uint8_t *)buff, White, Black);
        
        Tetris_Draw_Next_Piece();
    }
	/*
    // MODE 1: AGGIORNAMENTO PARZIALE (Solo blocchi del tetromino)
    else if(mode == 1) {
        for(i=0; i<4; i++) {
            x = game.prev_current.x + game.prev_current.blocks[i][0];
            y = game.prev_current.y + game.prev_current.blocks[i][1];
            if(y >= 0 && y < BOARD_ROWS) {
                 px = BOARD_X + x*BLOCK_SIZE;
                 py = BOARD_Y + y*BLOCK_SIZE;
                 for(k=0; k<BLOCK_SIZE-1; k++) {
                     LCD_DrawLine(px+k, py, px+k, py+BLOCK_SIZE-1, COL_BACKGROUND);
                 }
            }
        }
    }
*/
		// MODE 1: AGGIORNAMENTO PARZIALE (Con contorno di 1) -> piccolo lag musica
		else if(mode == 1) {
    int dx, dy, target_x, target_y;
    uint16_t color_to_draw;

    for(i=0; i<4; i++) {
        // Coordinate del blocco precedente
        x = game.prev_current.x + game.prev_current.blocks[i][0];
        y = game.prev_current.y + game.prev_current.blocks[i][1];

        // Itera sui vicini (dx: -1, 0, 1 | dy: -1, 0, 1) per coprire il contorno
        for(dy = -1; dy <= 1; dy++) {
            for(dx = -1; dx <= 1; dx++) {
                
                target_x = x + dx;
                target_y = y + dy;

                // 1. Controllo limiti della board
                if(target_x >= 0 && target_x < BOARD_COLS && target_y >= 0 && target_y < BOARD_ROWS) {
                    
                    // Calcolo coordinate pixel
                    px = BOARD_X + target_x * BLOCK_SIZE;
                    py = BOARD_Y + target_y * BLOCK_SIZE;
                    
                    // 2. Leggi il colore che DOVREBBE esserci in quella cella
                    // Se è vuota sarà COL_BACKGROUND, se c'è un pezzo sarà il suo colore
                    color_to_draw = game.board[target_y][target_x];

                    // 3. Ridisegna il blocco intero
                    for(k=0; k<BLOCK_SIZE-1; k++) {
                        LCD_DrawLine(px+k, py, px+k, py+BLOCK_SIZE-1, color_to_draw);
                    }
                }
            }
        }
    }
}

		/*
		// MODE 1: AGGIORNAMENTO PARZIALE ESTESO (con contorno)
    else if(mode == 1) {
        int dx, dy, target_x, target_y;
        uint16_t grid_color = 0x2104; // Colore griglia
        
        for(i=0; i<4; i++) {
            // Coordinate centrali del blocco PRECEDENTE
            x = game.prev_current.x + game.prev_current.blocks[i][0];
            y = game.prev_current.y + game.prev_current.blocks[i][1];

            // Iteriamo su un'area 3x3 attorno al blocco (contorno di 1)
            for(dy = -1; dy <= 1; dy++) {
                for(dx = -1; dx <= 1; dx++) {
                    
                    target_x = x + dx;
                    target_y = y + dy;

                    // 1. Controllo che siamo dentro la board
                    if(target_x >= 0 && target_x < BOARD_COLS && target_y >= 0 && target_y < BOARD_ROWS) {
                        
                        px = BOARD_X + target_x * BLOCK_SIZE;
                        py = BOARD_Y + target_y * BLOCK_SIZE;
                        
                        // 2. Leggiamo cosa c'è REALMENTE in quella cella nella memoria del gioco
                        uint16_t cell_content = game.board[target_y][target_x];

                        // CASO A: La cella è vuota (Sfondo)
                        if(cell_content == COL_BACKGROUND) {
                            // Cancelliamo (Nero)
                            for(k=0; k<BLOCK_SIZE-1; k++) {
                                LCD_DrawLine(px+k, py, px+k, py+BLOCK_SIZE-1, COL_BACKGROUND);
                            }
                            // Ridisegniamo la griglia (perché l'abbiamo appena cancellata col nero)
                            LCD_DrawLine(px + BLOCK_SIZE - 1, py, px + BLOCK_SIZE - 1, py + BLOCK_SIZE - 1, grid_color);
                            LCD_DrawLine(px, py + BLOCK_SIZE - 1, px + BLOCK_SIZE - 1, py + BLOCK_SIZE - 1, grid_color);
                        }
                        // CASO B: La cella contiene un pezzo piazzato (Torre)
                        else {
                            // Ridisegniamo il pezzo piazzato (altrimenti il passaggio del pezzo lo cancellerebbe!)
                            for(k=0; k<BLOCK_SIZE-1; k++) {
                                LCD_DrawLine(px+k, py, px+k, py+BLOCK_SIZE-1, cell_content);
                            }
                        }
                    }
                }
            }
        }
    }
		*/

    // DISEGNA NUOVO PEZZO
    if(game.state == STATE_PLAYING) {
        for(i=0; i<4; i++) {
            x = game.current.x + game.current.blocks[i][0];
            y = game.current.y + game.current.blocks[i][1];
            if(y >= 0 && y < BOARD_ROWS) {
                 px = BOARD_X + x*BLOCK_SIZE;
                 py = BOARD_Y + y*BLOCK_SIZE;
                 for(k=0; k<BLOCK_SIZE-1; k++) {
                     LCD_DrawLine(px+k, py, px+k, py+BLOCK_SIZE-1, game.current.color);
                 }
            }
        }
    }
}


/*	
		FUNZIONI PER DISEGNARE SCHERMATA INTRO
*/

void Draw_Title_Block(int x, int y, uint16_t color) {
    int i;
    
    for(i=0; i<BLOCK_SIZE; i++) {
        LCD_DrawLine(x+i, y, x+i, y+BLOCK_SIZE, color);
    }
    
    // Bordo su
    LCD_DrawLine(x, y, x + BLOCK_SIZE, y, Black);
    // Bordo giu
    LCD_DrawLine(x, y + BLOCK_SIZE, x + BLOCK_SIZE, y + BLOCK_SIZE, Black);
    // Bordo sinistro
    LCD_DrawLine(x, y, x, y + BLOCK_SIZE, Black);
    // Bordo Destro
    LCD_DrawLine(x + BLOCK_SIZE, y, x + BLOCK_SIZE, y + BLOCK_SIZE, Black);
}

void Tetris_Draw_Title(void) {
    int start_x = 5; // Margine sinistro
    int start_y = 50; // Altezza dall alto
    int bs = BLOCK_SIZE; // 10 pixel
    int x, y;

    // LETTERA T (Red)
    x = start_x; y = start_y;
    Draw_Title_Block(x, y, Red);     Draw_Title_Block(x+bs, y, Red); Draw_Title_Block(x+2*bs, y, Red);
    Draw_Title_Block(x+bs, y+bs, Red); Draw_Title_Block(x+bs, y+2*bs, Red);
    Draw_Title_Block(x+bs, y+3*bs, Red); Draw_Title_Block(x+bs, y+4*bs, Red);

    // LETTERA E (White)
    x += 4*bs;
    Draw_Title_Block(x, y, White); Draw_Title_Block(x+bs, y, White); Draw_Title_Block(x+2*bs, y, White);
    Draw_Title_Block(x, y+bs, White); 
    Draw_Title_Block(x, y+2*bs, White); Draw_Title_Block(x+bs, y+2*bs, White); Draw_Title_Block(x+2*bs, y+2*bs, White);
    Draw_Title_Block(x, y+3*bs, White);
    Draw_Title_Block(x, y+4*bs, White); Draw_Title_Block(x+bs, y+4*bs, White); Draw_Title_Block(x+2*bs, y+4*bs, White);

    // LETTERA T (Yellow)
    x += 4*bs;
    Draw_Title_Block(x, y, Yellow);     Draw_Title_Block(x+bs, y, Yellow); Draw_Title_Block(x+2*bs, y, Yellow);
    Draw_Title_Block(x+bs, y+bs, Yellow); Draw_Title_Block(x+bs, y+2*bs, Yellow);
    Draw_Title_Block(x+bs, y+3*bs, Yellow); Draw_Title_Block(x+bs, y+4*bs, Yellow);

    // LETTERA R (Green)
    x += 4*bs;
    Draw_Title_Block(x, y, Green); Draw_Title_Block(x+bs, y, Green);
    Draw_Title_Block(x, y+bs, Green);  Draw_Title_Block(x+2*bs, y+bs, Green);
    Draw_Title_Block(x, y+2*bs, Green); Draw_Title_Block(x+bs, y+2*bs, Green);
    Draw_Title_Block(x, y+3*bs, Green); Draw_Title_Block(x+2*bs, y+3*bs, Green);
    Draw_Title_Block(x, y+4*bs, Green); Draw_Title_Block(x+2*bs, y+4*bs, Green);

    // LETTERA I (Cyan)
    x += 4*bs;
    Draw_Title_Block(x, y, Cyan); Draw_Title_Block(x+bs, y, Cyan); Draw_Title_Block(x+2*bs, y, Cyan);
    Draw_Title_Block(x+bs, y+bs, Cyan); 
    Draw_Title_Block(x+bs, y+2*bs, Cyan);
    Draw_Title_Block(x+bs, y+3*bs, Cyan);
    Draw_Title_Block(x, y+4*bs, Cyan); Draw_Title_Block(x+bs, y+4*bs, Cyan); Draw_Title_Block(x+2*bs, y+4*bs, Cyan);

    // LETTERA S (Magenta)
    x += 4*bs;
    Draw_Title_Block(x+bs, y, Magenta); Draw_Title_Block(x+2*bs, y, Magenta);
    Draw_Title_Block(x, y+bs, Magenta); 
    Draw_Title_Block(x+bs, y+2*bs, Magenta);
    Draw_Title_Block(x+2*bs, y+3*bs, Magenta); 
    Draw_Title_Block(x, y+4*bs, Magenta); Draw_Title_Block(x+bs, y+4*bs, Magenta);
}

void Tetris_Draw_Intro(void) {
    LCD_Clear(Black);
    //GUI_Text(90, 100, (uint8_t *)"TETRIS", Red, Black);
    //GUI_Text(50, 150, (uint8_t *)"PRESS KEY1 TO PLAY", White, Black);
		Tetris_Draw_Title();
	  GUI_Text(45, 150, (uint8_t *)"PRESS KEY1 TO PLAY", White, Red);
		GUI_Text(90, 240, (uint8_t *)"S320305", White, Black);
		GUI_Text(45, 270, (uint8_t *)"Domenico di Stasio", White, Black);
}