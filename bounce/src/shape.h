#ifndef SHAPE_H 
#define SHAPE_H 

#include <stdint.h>
#include <stdbool.h>

typedef struct { 
	int x,y; 
} Vector2; 

#define fxtoi(x) (x>>8)
#define itofx(x) (x<<8)

#define dotProduct(a,b) (a.x*b.x + a.y*b.y)
#define crossProduct(a,b) (a.x*b.y - a.y*b.x)

/*
p0,p1 : integer position of endpoints of line segment
v : fixed point unit vector in direction from p0 to p1 
n : fixed point normal vector
length : length of line segment 
color: rendering color 
*/ 
typedef struct { 
	Vector2 p0,p1; 
	Vector2 v,n; 
	int length;
	uint8_t color;
} LineSeg; 


/*
p,v,a : fixed point position,velocity,and acceleration vectors 
mass: mass of Ball 
radius: radius of Ball 
color: rendering color 
*/
typedef struct { 
	Vector2 p,v,a; 
	uint8_t mass; 
	uint8_t radius;
	uint8_t color;
	unsigned int sleeping;
	bool collided;
} Ball; 

/* 
l: LineSeg to fill 
p0,p1: endpoints of line 
c: point to orient normal towards
*/ 
void generateLineSeg(uint8_t i,Vector2 p0,Vector2 p1,Vector2 c); 

void renderBalls(void);
void renderLineSegs(void);

#endif