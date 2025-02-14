#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "Game.h"
#include "memory.h"

typedef struct shardgraph {
	uint64_t shardid;
	int childrencount;
	int parentcount;
	int parentsdiscovered;
	int childrensolved;
	struct shardgraph** childrenshards;
	struct shardgraph** parentshards;

	//For use in shard work queue. Owned by maindriver.c
	struct shardgraph* nextinqueue;
	int discovered;
} shardgraph;


void discoverfragment(char* workingfolder, shardgraph* targetshard, char fragmentsize, bool isstartingfragment);

void solvefragment(char* workingfolder, shardgraph* targetshard, char fragmentsize, bool isstartingfragment);

//Shard graph functions

shardgraph* getstartingshard(shardgraph* shardlist, int shardsize);

int initializeshardlist(shardgraph** shardlistptr, uint32_t shardsize);

void freeshardlist(shardgraph* shardlist, uint32_t shardsize);
