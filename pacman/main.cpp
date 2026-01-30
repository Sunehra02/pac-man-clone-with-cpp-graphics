#include <graphics.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const int CELL = 42;
const int ROWS = 11;
const int COLS = 15;
const int WIN_W = COLS * CELL;
const int WIN_H = ROWS * CELL;

// Maze template
int mazeTemplate[ROWS][COLS] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,0,2,2,2,1,2,2,2,0,2,2,1},
    {1,2,1,1,1,0,2,1,2,0,1,1,1,2,1},
    {1,2,2,2,1,2,2,2,2,2,1,2,2,2,1},
    {1,2,1,2,1,2,1,1,1,2,1,2,1,2,1},
    {1,2,2,2,2,2,2,0,2,2,2,2,2,2,1},
    {1,2,1,2,1,2,1,1,1,2,1,2,1,2,1},
    {1,2,2,2,1,2,2,2,2,2,1,2,2,2,1},
    {1,2,1,1,1,1,1,0,1,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

struct Entity {
    int row, col;
    int dx, dy;
};

// Check if wall
bool isWall(int r,int c,int maze[ROWS][COLS]){
    if(r<0 || r>=ROWS || c<0 || c>=COLS) return true;
    return maze[r][c]==1;
}

// Draw maze
void drawMaze(int maze[ROWS][COLS]){
    for(int r=0;r<ROWS;r++){
        for(int c=0;c<COLS;c++){
            int x=c*CELL, y=r*CELL;
            if(maze[r][c]==1){
                setcolor(BLUE);
                setfillstyle(SOLID_FILL, BLUE);
                bar(x,y,x+CELL,y+CELL);
            } else {
                setcolor(BLACK);
                setfillstyle(SOLID_FILL, BLACK);
                bar(x,y,x+CELL,y+CELL);
                if(maze[r][c]==2){
                    setcolor(YELLOW);
                    setfillstyle(SOLID_FILL, YELLOW);
                    fillellipse(x+CELL/2, y+CELL/2, 5,5);
                }
            }
        }
    }
}

// Draw Pac-Man or ghost
void drawEntity(Entity e, int color, bool isPacman){
    int cx = e.col*CELL + CELL/2;
    int cy = e.row*CELL + CELL/2;
    int radius = CELL/2 - 5;

    if(isPacman){
        int startAngle=45,endAngle=315;
        if(e.dx==1){ startAngle=315; endAngle=225; }
        else if(e.dx==-1){ startAngle=135; endAngle=45; }
        else if(e.dy==1){ startAngle=45; endAngle=315; }
        else if(e.dy==-1){ startAngle=225; endAngle=135; }

        setcolor(YELLOW);
        setfillstyle(SOLID_FILL, YELLOW);
        pieslice(cx,cy,startAngle,endAngle,radius);
    } else {
        int half = CELL/2 -6;
        setcolor(BLACK);
        rectangle(cx-half, cy-half, cx+half, cy+half);
        setfillstyle(SOLID_FILL,color);
        bar(cx-half+1, cy-half+1, cx+half-1, cy+half-1);

        // Eyes
        setfillstyle(SOLID_FILL, WHITE);
        fillellipse(cx-half/2, cy-half/2, 4,4);
        fillellipse(cx+half/2, cy-half/2, 4,4);
    }
}
// Move ghosts one cell at a time, simpler version
void moveGhost(Entity &g, int maze[ROWS][COLS]){
    int nr = g.row + g.dx;
    int nc = g.col + g.dy;

    if(!isWall(nr,nc,maze)){
        g.row = nr;
        g.col = nc;
    } else {
        // Pick one random valid direction immediately
        int directions[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
        while(true){
            int r = rand() % 4;
            int ndx = directions[r][0];
            int ndy = directions[r][1];
            if(!isWall(g.row+ndx,g.col+ndy,maze)){
                g.dx = ndx;
                g.dy = ndy;
                g.row += ndx;
                g.col += ndy;
                break;
            }
        }
    }
}

int main(){
    srand(time(NULL));
    int gd=DETECT, gm;
    initgraph(&gd, &gm, "");

    while(true){ // restart loop
        int maze[ROWS][COLS];
        memcpy(maze, mazeTemplate, sizeof(maze)); // reset dots

        Entity pac = {1,1,0,1};
        Entity g1 = {9,13,0,-1};
        Entity g2 = {9,1,0,1};
        Entity g3 = {5,7,1,0};

        bool running=true;
        bool win=false;
        int ghostTick=0;

        while(running){
            cleardevice();
            drawMaze(maze);

            // Player input
            int pr=0, pc=0;
            if(kbhit()){
                int key = getch();
                if(key==0||key==224) key=getch();
                if(key==27){ running=false; win=false; break; }
                if(key==72) pr=-1;
                if(key==80) pr=1;
                if(key==75) pc=-1;
                if(key==77) pc=1;
            }

            // Move Pac-Man
            if(pr!=0 || pc!=0){
                int nr = pac.row + pr;
                int nc = pac.col + pc;
                if(!isWall(nr,nc,maze)){
                    pac.row = nr;
                    pac.col = nc;
                    pac.dx = pr;
                    pac.dy = pc;
                }
            }

            // Eat dot
            if(maze[pac.row][pac.col]==2) maze[pac.row][pac.col]=0;

            // Move ghosts
            ghostTick++;
            if(ghostTick>=2){
                ghostTick=0;
                moveGhost(g1, maze);
                moveGhost(g2, maze);
                moveGhost(g3, maze);
            }

            // Draw entities
            drawEntity(pac,YELLOW,true);
            drawEntity(g1,RED,false);
            drawEntity(g2,CYAN,false);
            drawEntity(g3,GREEN,false);

            // Check collisions
            if((pac.row==g1.row && pac.col==g1.col)||
               (pac.row==g2.row && pac.col==g2.col)||
               (pac.row==g3.row && pac.col==g3.col)){
                running=false;
                win=false;
            }

            // Check dots left
            int dotsLeft=0;
            for(int r=0;r<ROWS;r++)
                for(int c=0;c<COLS;c++)
                    if(maze[r][c]==2) dotsLeft++;
            if(dotsLeft==0){ running=false; win=true; }

            delay(50);
        }

        // End screen
        const char* msg = win?"YOU WIN!":"GAME OVER";
        settextstyle(DEFAULT_FONT,HORIZ_DIR,2);
        setcolor(WHITE);
        outtextxy(WIN_W/2-60, WIN_H/2-20, (char*)msg);
        outtextxy(WIN_W/2-90, WIN_H/2+20,"R=Restart ESC=Exit");

        // Wait for restart or exit
        bool waitInput = true;
        while(waitInput){
            if(kbhit()){
                int key = getch();
                if(key=='r'||key=='R'){ waitInput=false; }
                if(key==27){ closegraph(); return 0; }
            }
            delay(50);
        }
    }

    closegraph();
    return 0;
}
