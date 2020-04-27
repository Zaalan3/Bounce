#ifndef COLLIDE_H 
#define COLLIDE_H 

#include <stdbool.h>
#include <stdint.h>
#include "shape.h" 

void initCollisionStack(void); 
void solveCollisions(void);

void collide_BallLine(uint8_t b,uint8_t l); 
void collide_BallBall(uint8_t ball1,uint8_t ball2); 

#endif