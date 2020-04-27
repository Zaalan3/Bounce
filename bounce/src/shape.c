#include <math.h>
#include <graphx.h> 
#include <stdint.h>

#include "shape.h" 
#include "glob.h"

/* 
line: LineSeg to fill 
p0,p1: endpoints of line 
c: point to orient normal towards
*/ 
void generateLineSeg(uint8_t i,Vector2 p0,Vector2 p1,Vector2 c) { 
	LineSeg l;
	int dx = p1.x-p0.x;
	int dy = p1.y-p0.y;
	int length = sqrt(dy*dy + dx*dx);
	
	// find length and unit vector
	l.p0 = p0;
	l.p1 = p1;

	l.v.x = itofx(dx)/length;
	l.v.y = itofx(dy)/length;	
	l.length = length; 
	
	// orient vector about c
	c.x -= p0.x; 
	c.y -= p0.y;
	
	if(crossProduct(c,l.v) >= 0) { 
		l.n.x = l.v.y; 
		l.n.y = 0 - l.v.x; 
	} else { 
		l.n.x = 0 - l.v.y; 
		l.n.y = l.v.x;
	} 
	l.color = 0xD0; 
	line[i] = l;
} 
	
void renderBalls(void) {
	uint8_t b; 
	for(b=0;b<numBalls;b++) { 
		gfx_SetColor(ball[b].color); 
		gfx_FillCircle(fxtoi(ball[b].p.x),fxtoi(ball[b].p.y),ball[b].radius); 
	} 
} 

void renderLineSegs(void) { 
	uint8_t l; 
	for(l=0;l<numLines;l++) {
		gfx_SetColor(line[l].color); 
		gfx_Line(line[l].p0.x,line[l].p0.y,line[l].p1.x,line[l].p1.y); 
	} 
}