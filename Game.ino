#include <Arduboy2.h>

Arduboy2 arduboy;

// screen size: 128x64
#define TILE_SIZE 4

// game states
enum state {
  START,
  GAME,
  LOSE
};


// vars
int direction = 3;

int x = 3;
int y = 4;

int delayframes = 8;

// this function runs once
void setup() {
  arduboy.begin();
  
  arduboy.initRandomSeed();

  arduboy.setFrameRate(30);
}


void loop() {
  // pause render until it's time for the next frame
  if (!(arduboy.nextFrame()))
    return;

  arduboy.clear();
  
  arduboy.setCursor(0, 0);
  

  // delays movement by a set amount of frames while still allowing input
  if (delayframes > 0) {
    if (arduboy.pressed(DOWN_BUTTON) && direction != 2) { direction = 1; }
    if (arduboy.pressed(UP_BUTTON) && direction != 1) { direction = 2; }
    if (arduboy.pressed(RIGHT_BUTTON) && direction != 3) { direction = 3; }
    if (arduboy.pressed(LEFT_BUTTON) && direction != 4) { direction = 4; }
    
    delayframes -= 1;
    
  } else {
    switch (direction) {
      case 1: // down
        y += 1;
        break;
        
      case 2: // up
        y -= 1;
        break;
        
      case 3: // right
        x += 1;
        break;
        
      case 4: // left
        x -= 1;
        break;
    }
    
    delayframes = 15;
    
  }
  
  
  if (x > 128/TILE_SIZE) { x = 1; }
  else if ( x < 1) { x = 128/TILE_SIZE; }
  
  if (y > 64/TILE_SIZE) { y = 1; }
  else if ( y < 1) { y = 64/TILE_SIZE; }
  
  
  arduboy.fillRect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
  
  
  arduboy.display();
}
