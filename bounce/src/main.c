/*
 *--------------------------------------
 * Program Name: Bounce
 * Author: Zaalan3
 * License: N/A
 * Description: Simple physics implementation
 *--------------------------------------
*/

#include <stdint.h> 
#include <stdbool.h> 
#include <stdlib.h> 
#include <tice.h> 
#include <graphx.h>
#include <keypadc.h>

#include "shape.h" 
#include "collision.h" 
#include "glob.h"

#define G 64
#define SLEEP_THRESHOLD 60

void initialize(void);
void step(void); 
void render(void); 

#define startTimer() timer_1_Counter = 0; \
					timer_Control = TIMER1_ENABLE|TIMER1_CPU|TIMER1_UP;
#define stopTimer() timer_Control = TIMER1_DISABLE; 
#define getTimer() timer_1_Counter


int main(void)
{
	bool cont = true; 
	
	initialize(); 
	
	while(cont) {
		kb_Scan(); 
		
		startTimer(); 
		step(); 
		stopTimer(); 
		render();
		
		cont = !kb_IsDown(kb_KeyClear);
	} 
	
	gfx_End(); 
    return 0;
}

void initialize() { 
	Vector2 p0 = {0,40}; 
	Vector2 p1 = {230,70}; 
	Vector2 p2 = {220,130}; 
	Vector2 p3 = {150,170}; 
	Vector2 p4 = {150,239}; 
	Vector2 p5 = {280,239}; 
	Vector2 p6 = {270,165}; 
	Vector2 p7 = {290,131}; 
	Vector2 p8 = {319,80}; 
	
	Vector2 c1 = {180,10};
	Vector2 c2 = {160,180};
	
	
	gfx_Begin(); 
	gfx_SetTextFGColor(0xFE);
	gfx_ZeroScreen(); 
	gfx_SetDrawBuffer(); 
	
	
	ball[0].p.x = itofx(70); 
	ball[0].p.y = itofx(20); 
	ball[0].v.x = 0;
	ball[0].v.y = 0;
	ball[0].a.x = 0;
	ball[0].a.y = 0;
	ball[0].mass = 10;
	ball[0].radius = 10;
	ball[0].color = 0x3f;
	ball[0].sleeping = 0;
	
	ball[1].p.x = itofx(225); 
	ball[1].p.y = itofx(20); 
	ball[1].v.x = 0;
	ball[1].v.y = 0;
	ball[1].a.x = 0;
	ball[1].a.y = 0;
	ball[1].mass = 11;
	ball[1].radius = 11;
	ball[1].color = 0xDE;
	ball[1].sleeping = 0;

	ball[2].p.x = itofx(140); 
	ball[2].p.y = itofx(20); 
	ball[2].v.x = 0;
	ball[2].v.y = 0;
	ball[2].a.x = 0;
	ball[2].a.y = 0;
	ball[2].mass = 12;
	ball[2].radius = 12;
	ball[2].color = 0xED;
	ball[2].sleeping = 0;
	
	ball[3].p.x = itofx(160); 
	ball[3].p.y = itofx(30); 
	ball[3].v.x = 2;
	ball[3].v.y = 0;
	ball[3].a.x = 0;
	ball[3].a.y = 0;
	ball[3].mass = 5;
	ball[3].radius = 8;
	ball[3].color = 0xC3;
	ball[3].sleeping = 0;
	
	ball[4].p.x = itofx(280); 
	ball[4].p.y = itofx(30); 
	ball[4].v.x = -1;
	ball[4].v.y = 0;
	ball[4].a.x = 0;
	ball[4].a.y = 0;
	ball[4].mass = 3;
	ball[4].radius = 10;
	ball[4].color = 0x35;
	ball[4].sleeping = 0;

	generateLineSeg(0,p0,p1,c1);
	generateLineSeg(1,p1,p2,c1);
	generateLineSeg(2,p2,p3,c2);
	generateLineSeg(3,p3,p4,c2);
	generateLineSeg(4,p4,p5,c2);
	generateLineSeg(5,p5,p6,c2);
	generateLineSeg(6,p6,p7,c1);
	generateLineSeg(7,p7,p8,c1);

} 

void step(void) { 
	uint8_t i,j;
	bool cont;
	
	initCollisionStack(); 	
	
	for(i = 0;i<numBalls;i++) {
		if(ball[i].sleeping<SLEEP_THRESHOLD) { 
			ball[i].v.y += G;	
			ball[i].p.x += ball[i].v.x;
			ball[i].p.y += ball[i].v.y;
		} 
	} 
	 
	
	for(i=0;i<numBalls;i++) { 
		if(ball[i].sleeping<SLEEP_THRESHOLD) { 
			for(j=0;j<numBalls;j++) { 
				collide_BallBall(i,j);
			} 
			for(j=0;j<numLines;j++) {
				collide_BallLine(i,j);
			}
		} 
	}
		
	solveCollisions(); 
	
	for(i = 0;i<numBalls;i++) { 
		ball[i].v.y += ball[i].a.y;
		ball[i].v.x += ball[i].a.x;
		ball[i].a.x = 0;
		ball[i].a.y = 0; 
		
		if(abs(ball[i].v.y)<768 && abs(ball[i].v.x)<768) { 
			if(ball[i].collided)
				ball[i].sleeping++; 
		} else { 
			ball[i].sleeping = 0; 
		} 
		
		ball[i].collided = false;
	}
	
} 

void render(void) { 
	uint8_t i;
	gfx_ZeroScreen(); 
	
	renderBalls(); 
	renderLineSegs();
	
	gfx_SetTextXY(20,10);
	gfx_PrintUInt(getTimer(),0);
		
	gfx_SwapDraw(); 
}