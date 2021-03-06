#include <Arduboy2.h>

Arduboy2 arduboy;

// screen size: 128x64
#define TILE_SIZE 8

#define WIDTH (128/TILE_SIZE)
#define HEIGHT (64/TILE_SIZE)

#define SNAKE_HEAD snaketrail[0]

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
struct vector2d snaketrail[(WIDTH) * (HEIGHT)];

struct vector2d food; // position struct for food

byte length = 1; // length of the snake

byte delayframes = 16; // amount of frames that pass before the snake moves


// pushes back all elements and adds new elements to index 0
// used for saving tail positions
// index 0 is the snake's head
void snake_pushback(byte x, byte y) {
  for (byte i = sizeof(snaketrail) - 1; i > 0; i--) {
    snaketrail[i].x = snaketrail[i - 1].x;
    snaketrail[i].y = snaketrail[i - 1].y;
  }
  
  SNAKE_HEAD.x = x;
  SNAKE_HEAD.y = y;
}


// spawns food randomly, not placing it on the snake
void spawnfood() {
  food.x = WIDTH + 1;
  food.y = HEIGHT + 1;
  
  while (food.x == WIDTH + 1 && food.y == HEIGHT + 1){
    food.x = random(0, WIDTH);
    food.y = random(0, HEIGHT);
    
    for (byte i = 0; i < length; i++) {
      if (snaketrail[i].x == food.x && snaketrail[i].y == food.y){
        food.x = WIDTH + 1;
        food.y = HEIGHT + 1;
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
void changedirection(){
  // true when corresponding d-pad button is pressed
  bool down = arduboy.pressed(DOWN_BUTTON);
  bool up = arduboy.pressed(UP_BUTTON);
  bool right = arduboy.pressed(RIGHT_BUTTON);
  bool left = arduboy.pressed(LEFT_BUTTON);
  
  // direction doesn't change if more than one button or no buttons are pressed
  if (down + up + right + left != 1) { return; }
  
  
  // if the button pressed isn't the current direction or the opposite of
  // the current direction, turn that way and skip delayframes to move immediately
  if ( up && direction != UP && direction != DOWN) { 
    delayframes = 0; direction = UP; } 
    
  else if (down && direction != DOWN && direction != UP) { 
    delayframes = 0; direction = DOWN; }
    
  else if (right && direction != LEFT && direction != RIGHT) { 
    delayframes = 0; direction = RIGHT; }
    
  else if (left && direction != RIGHT && direction != LEFT) { 
    delayframes = 0; direction = LEFT; 
  }
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
        
        delayframes -= 1;
        changedirection();
        
      // moves the snake depending on the direction it faces
      } else {
        
        snake_pushback(SNAKE_HEAD.x, SNAKE_HEAD.y);
        
        // screen looping and snake movement
        switch (direction) {
          case DOWN:
            if (SNAKE_HEAD.y >= HEIGHT - 1) { SNAKE_HEAD.y = 0; break; }
            SNAKE_HEAD.y += 1;
            break;
            
          case UP:
            if (SNAKE_HEAD.y == 0) { SNAKE_HEAD.y = HEIGHT; }
            SNAKE_HEAD.y -= 1;
            break;
            
          case RIGHT:
            if (SNAKE_HEAD.x >= WIDTH - 1) { SNAKE_HEAD.x = 0; break; }
            SNAKE_HEAD.x += 1;
            break;
            
          case LEFT:
            if (SNAKE_HEAD.x == 0) { SNAKE_HEAD.x = WIDTH; }
            SNAKE_HEAD.x -= 1;
            break;
        }
        
        
        delayframes = 16;
      }
      
      
      // draws the entire snake and checks for collisions
      for (byte i = 0; i < length; i++) {
        if (snaketrail[i].x == SNAKE_HEAD.x && snaketrail[i].y == SNAKE_HEAD.y && i != 0){
          game = LOSE;
        }
        
        // used for drawing individual segments of the snake based on tile size
        arduboy.fillRect(snaketrail[i].x * TILE_SIZE, snaketrail[i].y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
      }
      
      // checks if the player is touching the food
      if (SNAKE_HEAD.x == food.x && SNAKE_HEAD.y == food.y){
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
      
      SNAKE_HEAD.x = 3;
      SNAKE_HEAD.y = 4;
      
      length = 1;
      
      direction = RIGHT;
      
      spawnfood();
      
      game = GAME;
      break;
  }
  
  arduboy.display();
}
