#include <stdio.h>
#include<string.h>

extern struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent 
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
  };

extern int TRACE;
extern int YES;
extern int NO;

struct distance_table {
  int costs[4][4];
} dt3;

/* students to write the following two routines, and maybe some others */
#define INFINITY 999

float clocktime;

int connectcosts3[4] = { 7,  999,  2, 0 };
struct rtpkt pkt3[4];
int min_cost_node3[4];

int min_3 ( int a, int b ) { return a < b ? a : b; }
int min3 ( int a[] ) {
    return min_3(min_3(min_3(a[0], a[1]), a[2]), a[3]);
}

void calc_min_cost_node3() {
    for(int i=0;i<4;i++){
        min_cost_node3[i] = min3(dt3.costs[i]);
    }
}
void sendpkt3() {
    for(int i=0;i<4;i++) {
        pkt3[i].sourceid = 3;
        pkt3[i].destid = i; 
        memcpy(pkt3[i].mincost, min_cost_node3, sizeof(min_cost_node3));
    }
    for(int i=0;i<4;i++) {
        if(i!=3 && i!=1) {
            tolayer2(pkt3[i]);
            printf("At time t=%.3f, node %d sends packet to node %d with: (%d  %d  %d  %d)\n",
                   clocktime, pkt3[i].sourceid, pkt3[i].destid, pkt3[i].mincost[0], pkt3[i].mincost[1],
                   pkt3[i].mincost[2],pkt3[i].mincost[3]);
        }
    }
}


void calc_send_pkt3() {
    int old_min_cost[4];
    memcpy(old_min_cost, min_cost_node3, sizeof(min_cost_node3));
    int change = 0;
    calc_min_cost_node3
  ();
    for (int i = 0; i < 4; i++) {
        if (old_min_cost[i] != min_cost_node3[i]) {
            change = 1;
        }
    }
    if (change == 1) {    
        sendpkt3();
    }
    else
        printf("\nMinimum cost didn't change. No new packets are sent\n");

}
void rtinit3() {
  printf("rtinit3() is called at time t=: %0.3f\n", clocktime);

  for (int targetNode = 0; targetNode < 4; targetNode++) {
    for (int viaNode = 0; viaNode < 4; viaNode++) {
        if (targetNode == viaNode)
            dt3.costs[targetNode][viaNode] = connectcosts3[targetNode];
        else
            dt3.costs[targetNode][viaNode] = INFINITY;
    }
  }
  printdt3(&dt3);
    calc_min_cost_node3
  ();
    sendpkt3();
}


void rtupdate3(struct rtpkt *incomingPacket){
    int neighborId = incomingPacket->sourceid;
    int receivingNodeId = incomingPacket->destid;
    int receivedMinCosts[4];

    for (int i = 0; i < 4; i++) {
        receivedMinCosts[i] = incomingPacket->mincost[i];
    }

    printf("rtupdate3() is called at time t=: %0.3f as node %d sent a pkt with (%d  %d  %d  %d)\n",
           clocktime, neighborId,
           receivedMinCosts[0], receivedMinCosts[1],
           receivedMinCosts[2], receivedMinCosts[3]);

    for (int destNode = 0; destNode < 4; destNode++) {
        int newCostViaNeighbor = dt3.costs[neighborId][neighborId] + receivedMinCosts[destNode];

        if (newCostViaNeighbor < INFINITY)
            dt3.costs[destNode][neighborId] = newCostViaNeighbor;
        else
            dt3.costs[destNode][neighborId] = INFINITY;
    }

    printdt3(&dt3);     
    calc_send_pkt3();   
}


printdt3(dtptr)
  struct distance_table *dtptr;
  
{
  printf("             via     \n");
  printf("   D3 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 1|  %3d   %3d\n",dtptr->costs[1][0], dtptr->costs[1][2]);
  printf("     2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);

}

printmincostnode3(){
    printf("Minimum cost from %d to other nodes : %d %d %d %d\n", 3, min_cost_node3[0], min_cost_node3[1],
           min_cost_node3[2], min_cost_node3[3] );
}