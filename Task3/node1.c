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
} dt1;


/* students to write the following two routines, and maybe some others */
#define INFINITY 999

float clocktime;

int connectcosts1[4] = { 1,  0,  1, 999 };
struct rtpkt pkt1[4];
int min_cost_node1[4];

int min_1 ( int a, int b ) { return a < b ? a : b; }
int min1 ( int a[] ) {
    return min_1(min_1(min_1(a[0], a[1]), a[2]), a[3]);
}

void calc_min_cost_node1(){
    for(int i=0;i<4;i++){
        min_cost_node1[i] = min1(dt1.costs[i]);
    }
}

void sendpkt1(){
    for(int i=0;i<4;i++) {
        pkt1[i].sourceid = 1;
        pkt1[i].destid = i; 
        memcpy(pkt1[i].mincost, min_cost_node1, sizeof(min_cost_node1));
    }
    for(int i=0;i<3;i++) { 
        if(i!=1) {            
            tolayer2(pkt1[i]);
            printf("At time t=%.3f, node %d sends packet to node %d with: (%d  %d  %d  %d)\n",
                   clocktime, pkt1[i].sourceid, pkt1[i].destid, pkt1[i].mincost[0], pkt1[i].mincost[1],
                   pkt1[i].mincost[2],pkt1[i].mincost[3]);
        }
    }
}

void calc_send_pkt1() {
    int old_min_cost[4];
    memcpy(old_min_cost, min_cost_node1, sizeof(min_cost_node1));
    int change = 0;
    calc_min_cost_node1();
    for (int i = 0; i < 4; i++) {
        if (old_min_cost[i] != min_cost_node1[i]) {
            change = 1;
        }
    }
    if (change == 1) {    
        sendpkt1();
    }
    else
        printf("\nMinimum cost didn't change. No new packets are sent\n");

}

rtinit1() {
  printf("rtinit1() is called at time t=: %0.3f\n", clocktime);

  for (int destNode = 0; destNode < 4; destNode++) {
    for (int viaNode = 0; viaNode < 4; viaNode++) {
        if (destNode == viaNode)
            dt1.costs[destNode][viaNode] = connectcosts1[destNode];
        else
            dt1.costs[destNode][viaNode] = INFINITY;
    }
}
  printdt1(&dt1);

    calc_min_cost_node1();
    sendpkt1();
}


void rtupdate1(struct rtpkt *incomingPacket){
    int neighborId = incomingPacket->sourceid;
    int receivingNodeId = incomingPacket->destid;
    int neighborMincosts[4];

    for (int i = 0; i < 4; i++) {
        neighborMincosts[i] = incomingPacket->mincost[i];
    }

    printf("rtupdate1() is called at time t=: %0.3f as node %d sent a pkt with (%d  %d  %d  %d)\n",
           clocktime, neighborId,
           neighborMincosts[0], neighborMincosts[1],
           neighborMincosts[2], neighborMincosts[3]);

    for (int destination = 0; destination < 4; destination++) {
        int newCostViaNeighbor = dt1.costs[neighborId][neighborId] + neighborMincosts[destination];

        if (newCostViaNeighbor < INFINITY)
            dt1.costs[destination][neighborId] = newCostViaNeighbor;
        else
            dt1.costs[destination][neighborId] = INFINITY;
    }

    printdt1(&dt1);       
    calc_send_pkt1();    
}



printdt1(dtptr)
  struct distance_table *dtptr;
  
{
  printf("             via   \n");
  printf("   D1 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);
  printf("     3|  %3d   %3d\n",dtptr->costs[3][0], dtptr->costs[3][2]);

}

printmincostnode1(){
    printf("Minimum cost from %d to other nodes : %d %d %d %d\n", 1, min_cost_node1[0], min_cost_node1[1],
           min_cost_node1[2], min_cost_node1[3] );
}

void linkhandler1(linkid, newcost)
int linkid, newcost;   
/* called when cost from 1 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
	
{
    int old_dist_linkid_to_others[4];
    for(int i=0;i<4;i++){
        old_dist_linkid_to_others[i] = dt1.costs[i][linkid] - dt1.costs[linkid][linkid];
    }

    int new_dist_0_to_linkid = newcost;

    for(int i=0;i<4;i++){
        dt1.costs[i][linkid] = new_dist_0_to_linkid + old_dist_linkid_to_others[i]; // = new_dist_1_to_i (1 to i via linkid)
    }

    printdt1(&dt1);
    calc_send_pkt1();
}

