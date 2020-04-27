#include <stdbool.h>
#include <stdint.h> 
#include <math.h> 
#include <stdlib.h> 

#include "shape.h" 
#include "collision.h" 
#include "glob.h"


static enum COLLISION_TYPE { 
	BALL_LINE, 
	BALL_BALL
};  

typedef struct { 
	uint8_t i;
	uint8_t j;  
	enum TYPE type; 
} CollisionEntry; 

static bool pushCollision(uint8_t o1,uint8_t o2,enum COLLISION_TYPE type); 

static uint8_t stackdepth;
#define STACK_SIZE 32
static CollisionEntry CollisionStack[STACK_SIZE]; 


void initCollisionStack(void) { 
	stackdepth = 0;
} 

void solveCollisions(void) {
	Ball* b1;
	Ball* b2; 
	LineSeg* l; 
	Vector2 db; 
	int dist,sumr;
	uint8_t i = stackdepth;
	uint8_t numPasses = 2;
	
RESOLVE: 
	while(stackdepth<STACK_SIZE) { 
		CollisionEntry e = CollisionStack[stackdepth--];
		switch(e.type) { 
		case BALL_LINE: 
			b1 = &ball[e.i];
			l = &line[e.j]; 
			db.x = fxtoi(b1->p.x) - l->p0.x;
			db.y = fxtoi(b1->p.y) - l->p0.y;
			dist = dotProduct(db,l->n); 
			
			// fix position 
			if(abs(dist) < itofx(b1->radius)) { 
				db.x = ((itofx(b1->radius) - dist)*l->n.x)>>8;
				db.y = ((itofx(b1->radius) - dist)*l->n.y)>>8;
				b1->p.x += db.x;
				b1->p.y += db.y;
			} 
			
			break; 
			
		case BALL_BALL: 
			b1 = &ball[e.i];
			b2 = &ball[e.j];
			sumr = b1->radius + b2->radius; 
			db.x = fxtoi(b2->p.x - b1->p.x); 
			db.y = fxtoi(b2->p.y - b1->p.y); 
			dist = sqrt(db.x*db.x + db.y*db.y);
			
			if(sumr > dist) { 
				if((b1->v.x*b1->v.x + b1->v.y*b1->v.y) > (b2->v.x*b2->v.x + b2->v.y*b2->v.y)) {
					b1->p.x = ((sumr+1)*(b1->p.x - b2->p.x))/dist + b2->p.x;
					b1->p.y = ((sumr+1)*(b1->p.y - b2->p.y))/dist + b2->p.y;
				} else { 
					b2->p.x = ((sumr+1)*(b2->p.x - b1->p.x))/dist + b1->p.x;
					b2->p.y = ((sumr+1)*(b2->p.y - b1->p.y))/dist + b1->p.y;
				} 
			}
			break; 
			
		} 
	}
	
	numPasses--;
	if(numPasses>0) { 
		stackdepth = i;
		goto RESOLVE; 
	} 
}


static bool pushCollision(uint8_t o1,uint8_t o2,enum COLLISION_TYPE type) { 
	uint8_t i; 
	if(stackdepth<STACK_SIZE) {
		
		for(i = 0;i<stackdepth;i++) { 
			CollisionEntry e = CollisionStack[i]; 
			if(e.type==type && e.i==o1 && e.j==o2) { 
				return false; 
			} 
		} 
		
		CollisionStack[stackdepth].i = o1;
		CollisionStack[stackdepth].j = o2;
		CollisionStack[stackdepth].type = type;
		stackdepth++; 
		
		return true; 
	} 
	
	return false; 
} 

void collide_BallLine(uint8_t b,uint8_t l) { 
	Vector2 db;
	int dist; 
	Ball* bp = &ball[b];
	LineSeg* lp = &line[l];
	
	db.x = fxtoi(bp->p.x) - lp->p0.x;
	db.y = fxtoi(bp->p.y) - lp->p0.y;
	dist = fxtoi(dotProduct(db,lp->v)); // scalar projection of ball center onto line
	
	if(dist <= lp->length && dist > 0) { 
		dist = fxtoi(dotProduct(db,lp->n)); // scalar projection of ball center onto line normal
		
		if(abs(dist) < bp->radius) { 
			if(!pushCollision(b,l,BALL_LINE)) 
				return; 
			bp->collided |= true;
			// reflect velocity 
			dist = (((bp->v.x>>8)*lp->n.x) + (bp->v.y>>8)*lp->n.y)>>8;
			db.x = dist*lp->n.x;
			db.y = dist*lp->n.y;
			
			// find friction 
			
			bp->a.x -= (17*db.x)/10; 
			bp->a.y -= (17*db.y)/10; 
		}
	}
} 


void collide_BallBall(uint8_t ball1,uint8_t ball2) {
	int sumr,dx,dy,dist;
	int dvx,dvy,ax,ay; 
	int summ;
	Ball* b1;
	Ball* b2;
	if (ball1==ball2) return;
	if (ball1>ball2) { 
		summ = ball1; 
		ball1 = ball2; 
		ball2 = summ; 
	} 
	
	b1 = &ball[ball1]; 
	b2 = &ball[ball2];
	
	sumr = b1->radius + b2->radius; 
	dx = fxtoi(b2->p.x - b1->p.x); 
	dy = fxtoi(b2->p.y - b1->p.y); 
	dist = dx*dx + dy*dy;
	
	if(sumr*sumr >= dist) { 
		if(!pushCollision(ball1,ball2,BALL_BALL))
			return;
		
		b1->collided |= true;
		b2->collided |= true;
		// calculate impulse 
		summ = 2*(b1->mass + b2->mass); 
		dvx = b2->v.x - b1->v.x; 
		dvy = b2->v.y - b1->v.y;
		
		ax = (3*b2->mass*dvx)/summ; 
		ay = (3*b2->mass*dvy)/summ;
		b1->a.x += ax;
		b1->a.y += ay;
		
		ax = (-3*b1->mass*dvx)/summ;; 
		ay = (-3*b1->mass*dvy)/summ;	
		b2->a.x += ax;
		b2->a.y += ay;
	} 
} 