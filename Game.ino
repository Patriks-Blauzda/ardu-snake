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

// snake directional states
enum directionstate {
  DOWN = 1,
  UP = 2,
  RIGHT = 3,
  LEFT = 4
};

enum state game = START;
enum directionstate direction = RIGHT;


// vars

struct vector2d { byte x; byte y; }; // struct for object position

// snake movement history array
struct vector2d snaketrail[(128/TILE_SIZE) * (64/TILE_SIZE)];

struct vector2d food; // position struct for food

int length = 1; // length of the snake

int delayframes = 15; // amount of frames that pass before the snake moves


// pushes back all elements and adds new elements to index 0
// used for saving tail positions
// index 0 is the snake's head
void snake_pushback(int x, int y) {
  for (int i = sizeof(snaketrail) - 1; i > 0; i--) {
    snaketrail[i].x = snaketrail[i - 1].x;
    snaketrail[i].y = snaketrail[i - 1].y;
  }
  
  snaketrail[0].x = x;
  snaketrail[0].y = y;
}


// spawns food randomly, not placing it on the snake
void spawnfood() {
  food.x = 0;
  food.y = 0;
  
  while (food.x == 0 && food.y == 0){
    food.x = random(1, 128/TILE_SIZE - 1);
    food.y = random(1, 64/TILE_SIZE - 1);
    
    for (int i = 0; i < length; i++) {
      if (snaketrail[i].x == food.x && snaketrail[i].y == food.y){
        food.x = 0;
        food.y = 0;
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


// checks if a movement button is the only one pressed
bool onepressed(int button){
  bool buttons[4] = {
    arduboy.pressed(DOWN_BUTTON), arduboy.pressed(UP_BUTTON),
    arduboy.pressed(RIGHT_BUTTON), arduboy.pressed(LEFT_BUTTON)
  };
  
  // checks if specified button from the array is the only one pressed
  // excludes checking the opposite directional button
  if (buttons[button] && !buttons[2] && !buttons[3]) {return true;}
  else if (buttons[button] && !buttons[2] && !buttons[3]) {return true;}
  else if (buttons[button] && !buttons[0] && !buttons[1]) {return true;}
  else if (buttons[button] && !buttons[0] && !buttons[1]) {return true;}
  else {return false;}
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
      // if turning the snake, delay is cancelled
      // moving opposite the direction the snake faces is made impossible
      if (delayframes > 0) {
        if (onepressed(0) && direction != UP && direction != DOWN) { direction = DOWN; delayframes = 0;}
        if (onepressed(1) && direction != DOWN && direction != UP) { direction = UP; delayframes = 0;}
        if (onepressed(2) && direction != LEFT && direction != RIGHT) { direction = RIGHT; delayframes = 0;}
        if (onepressed(3) && direction != RIGHT && direction != LEFT) { direction = LEFT; delayframes = 0;}
        
        delayframes -= 1;
        
      // moves the snake depending on the direction it faces
      } else {
        
        snake_pushback(snaketrail[0].x, snaketrail[0].y);
        
        // screen looping and snake movement
        switch (direction) {
          case DOWN:
            if (snaketrail[0].y >= 64/TILE_SIZE - 1) { snaketrail[0].y = 0; break; }
            snaketrail[0].y += 1;
            break;
            
          case UP:
            if (snaketrail[0].y == 0) { snaketrail[0].y = 64/TILE_SIZE; }
            snaketrail[0].y -= 1;
            break;
            
          case RIGHT:
            if (snaketrail[0].x >= 128/TILE_SIZE - 1) { snaketrail[0].x = 0; break; }
            snaketrail[0].x += 1;
            break;
            
          case LEFT:
            if (snaketrail[0].x == 0) { snaketrail[0].x = 128/TILE_SIZE; }
            snaketrail[0].x -= 1;
            break;
        }
        
        
        delayframes = 15;
      }
      
      
      // draws the entire snake and checks for collisions
      for (int i = 0; i < length; i++) {
        if (snaketrail[i + 1].x == snaketrail[0].x && snaketrail[i + 1].y == snaketrail[0].y){
          game = LOSE;
        }
        
        // used for drawing individual segments of the snake based on tile size
        arduboy.fillRect(snaketrail[i].x * TILE_SIZE, snaketrail[i].y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
      }
      
      // checks if the player is touching the food
      if (snaketrail[0].x == food.x && snaketrail[0].y == food.y){
        length += 1;
        spawnfood();
      }
      
      // draws food square
      arduboy.drawRect(food.x * TILE_SIZE, food.y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
      
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
      
      snaketrail[0].x = 3;
      snaketrail[0].y = 4;
      
      length = 1;
      
      direction = RIGHT;
      
      spawnfood();
      
      game = GAME;
      break;
  }
  
  arduboy.display();
}
