#include <Arduboy2.h>

Arduboy2 arduboy;

// screen size: 128x64
#define TILE_SIZE 8

// game states
enum state {
  START = 1,
  GAME = 2,
  LOSE = 3,
  RESET = 4
};

enum state game = START;

// vars
int direction = 3; // movement direction

int x = 3; // snake coordinates
int y = 4;

int foodx = -2;
int foody = -2;

int length = 1; // length of the snake

// snake movement history array
byte snaketrail[(128/TILE_SIZE) * (64/TILE_SIZE)][2];

int delayframes = 5; // amount of frames that pass before the snake moves

// pushes back all elements and adds new elements to index 0
// used for saving tail positions
void snake_pushback(int x, int y) {
  for (int i = sizeof(snaketrail) - 1; i >= 0; i--) {
    snaketrail[i][0] = snaketrail[i - 1][0];
    snaketrail[i][1] = snaketrail[i - 1][1];
  }
  
  if (x > 128/TILE_SIZE - 1) { x = 0; }
  else if ( x < 0) { x = 128/TILE_SIZE - 1; }
  
  if (y > 64/TILE_SIZE - 1) { y = 0; }
  else if ( y < 0) { y = 64/TILE_SIZE - 1; }
  
  snaketrail[0][0] = x;
  snaketrail[0][1] = y;
}


// spawns food randomly, not placing it on the snake
void spawnfood() {
  foodx = -2;
  foody = -2;
  
  while (foodx == -2 && foody == -2){
    foodx = random(1, 128/TILE_SIZE - 1);
    foody = random(1, 64/TILE_SIZE - 1);
    
    for (int i = 0; i < length; i++) {
      if (snaketrail[i][0] == foodx && snaketrail[i][1] == foody){
        foodx = -2;
        foody = -2;
      }
    }
  }
}


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
  
  switch (game){
    case START:
      arduboy.setCursor(128/3.5, 64/4);
      arduboy.print("ARDU-SNAKE");
      
      arduboy.setCursor(128/3, 64/1.4);
      arduboy.print("PRESS A");
      
      if (arduboy.pressed(A_BUTTON)){
        game = RESET;
      }
      
      break;
    
    
    case GAME:
      // delays movement by a set amount of frames while still allowing input
      if (delayframes > 0) {
        if (arduboy.pressed(DOWN_BUTTON) && direction != 2) { direction = 1; }
        if (arduboy.pressed(UP_BUTTON) && direction != 1) { direction = 2; }
        if (arduboy.pressed(RIGHT_BUTTON) && direction != 4) { direction = 3; }
        if (arduboy.pressed(LEFT_BUTTON) && direction != 3) { direction = 4; }
        
        delayframes -= 1;
        
      // moves the snake depending on the direction it faces
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
        
        delayframes = 10;
        
        snake_pushback(x, y);
      }
      
      // screen looping
      if (x > 128/TILE_SIZE - 1) { x = 0; }
      else if ( x < 0) { x = 128/TILE_SIZE - 1; }
      
      if (y > 64/TILE_SIZE - 1) { y = 0; }
      else if ( y < 0) { y = 64/TILE_SIZE - 1; }
      
      
      // draws the entire snake and checks for collisions
      for (int i = 0; i < length; i++) {
        if (snaketrail[i + 1][0] == x && snaketrail[i + 1][1] == y){
          game = LOSE;
        }
        
        arduboy.fillRect(snaketrail[i][0] * TILE_SIZE, snaketrail[i][1] * TILE_SIZE, TILE_SIZE, TILE_SIZE);
      }
      
      // checks if the player is touching the food
      if (x == foodx && y == foody){
        length += 1;
        spawnfood();
      }
      
      // draws food square
      arduboy.drawRect(foodx * TILE_SIZE, foody * TILE_SIZE, TILE_SIZE, TILE_SIZE);
      
      break;
    
    
    case LOSE:
      arduboy.setCursor(128/3.5, 64/4);
      arduboy.print("GAME OVER");
      
      arduboy.setCursor(128/3.35, 64/2);
      arduboy.print("Score: ");
      arduboy.print(length - 1);
      
      arduboy.setCursor(128/3, 64/1.25);
      arduboy.print("PRESS A");
      
      if (arduboy.pressed(A_BUTTON)){
        game = RESET;
      }
      break;
      
    
    // resets snake size and position before beginning the game
    case RESET:
      // immediately moves to force the visuals to update
      delayframes = 0;
      
      x = 3;
      y = 4;
      length = 1;
      direction = 3;
      spawnfood();
      
      game = GAME;
      break;
  }
  
  arduboy.display();
}
