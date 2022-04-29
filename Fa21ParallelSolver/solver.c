#include "Game.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>

/*
This one is same as discover fragment, but without a starting file
Especially for root node of game tree
*/
void discoverstartingfragment(char* workingfolder, char fragmentsize) {

}

/*
Downward Traversal

workingfolder - save the child discovery and my solves to
parentshards - Shards of my parents
numParents - number of parents
minhash - the root starting game piece of parent shard (current shard)
fragmentsize - shard size (usually a define, a constant)
*/
int discoverfragment(char* workingfolder, uint64_t* parentshards, int numParents, gamehash minhash, char fragmentsize) {
	uint64_t currentshard = (minhash >> fragmentsize);
	uint64_t* childrenshards;
	// Gives list of children shards that we send discovery children to that we send to
	// TODO
	int childrenshardcount = getchildrenshards(&childrenshards, workingfolder, fragmentsize, currentshard);

	Game g;
	game newg;
	gamehash h;
	gamehash minindex = maxHash();
	gamehash maxindex = 0;
	char primitive;

	int index = 0;

	// Next Tier: Use CUDA Malloc when moving to GPU for "moves" and "fringe"
	solverdata* primitives = initializesolverdata(hashLength());
	char moves[getMaxMoves()];
	game* fringe = calloc(sizeof(game), getMaxMoves() * getMaxDepth());
	if (primitives == NULL || fringe == NULL) {
		printf("Memory allocation error\n");
		return 1;
	}

	// Add incoming Discovery states from parent
	// Multiple top node in shard
	for (int i = 0; i < numParents; i++) {
		// TODO
		GET_DISCOVERY_STATES_FROM_SHARD(&fringe, &index, parentshards[i]);
	}

	int movecount, i, minprimitive, oldindex;
	while (index) {
		minprimitive = 255;
		oldindex = index;
		g = fringe[index - 1];
		h = getHash(g);
		if (solverread(primitives, h) == 0) {
			movecount = generateMoves((char*) &moves, g);
			for (i = 0; i < movecount; i++) {
				newg = doMove(g, moves[i]);
				h = getHash(newg);
				// Was this state previously sovled?
				primitive = solverread(primitives, h);
				if (!primitive) {
					// "isPrimitive" returns game status
					primitive = isPrimitive(newg, moves[i]);
					if (primitive != (char)NOT_PRIMITIVE) {
						// Terminal state with win, loss, or tie
						solverinsert(primitives, h, primitive);
						if (h < minindex) {
							minindex = h;
						}
						if (h > maxindex) {
							maxindex = h;
						}
						// printf("Position 0x%08x determined primitive\n", newg);
						positionsfound[primitive]++;
						minprimitive = minprimitive <= primitive ? minprimitive : primitive;
					} else {
						if ((h >> fragmentsize) == currentshard) {
							// Still in current shard, ready to process next
							fringe[index++] = newg;
						} else {
							// Send to child
							// TODO
							DETERMINE_AND_SEND_TO_CHILD(childrenshards, childrenshardcount, newg, workingfolder);
						}
					}
				} else {
					minprimitive = minprimitive <= primitive ? minprimitive : primitive;
				}
			}
			if (index == oldindex) {
				if (minprimitive & 128) {
					minprimitive = (minprimitive & 64) ? (257 - minprimitive) : (minprimitive + 1);
				} else {
					minprimitive = 255 - minprimitive;
				}
				h = getHash(g);
				solverinsert(primitives, h, minprimitive);
				if (h < minindex) minindex = h;
				if (h > maxindex) maxindex = h;
				// printf("Position 0x%08x determined fully\n", g);
				positionsfound[minprimitive]++;
				index--;
			}
		} else {
			index--;
		}
	}
	// TODO any way to save the data without this loop ... seems kinda wasteful
	int totalpositioncount = 0;
	for (int i = 1; i < 64; i++) {
		if (positionsfound[i]) printf("Loss in %d: %d\n", i - 1, positionsfound[i]);
		totalpositioncount += positionsfound[i];
	}
	for(int i = 128; i < 192; i++) {
		if (positionsfound[i]) printf("Tie in %d: %d\n", i - 128, positionsfound[i]);
		totalpositioncount += positionsfound[i];
	}
	for(int i = 254; i >= 192; i--) {
		if (positionsfound[i]) printf("Win in %d: %d\n", 255 - i, positionsfound[i]);
		totalpositioncount += positionsfound[i];
	}
	// ^^^^^^^^^^^^^^^

	return 0;
}

// Upward Traversal
// Run after all children solved
void solvefragment(char* workingfolder, gamehash minhash, char fragmentsize) {
	/*
	gamehash* childrenshardsG;
	int shardcount = getchildrenshards(&childrenshards, fragmentsize, minhash);
	// Load solved fragments into array on GPU
	// Load list of target hashes into array on GPU
	// Create a new solver on GPU
	for(int i = ; i >= ; i--) // First two values in input file contain the max and min depth
	{
		// GPU runs one tier of the solver
	}
	// Collect the data from the GPU
	// Compress and save the shard into solvedfragmentfolder
	*/


}

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}
  	initialize_constants();
    game pos = getStartingPositions();
    solverdata* primitives = initializesolverdata(hashLength());
	game* fringe = calloc(sizeof(game), getMaxMoves()*getMaxDepth());
	if(primitives == NULL || fringe == NULL) {
		printf("Memory allocation error\n");
		return 1;
	}
	int index = 1;
	fringe[0] = getStartingPositions();
	game g;
	game newg;
	gamehash h;
	char primitive;
	char minprimitive;
	char moves[getMaxMoves()];
	int movecount;
	int i;
	int oldindex;
	gamehash minindex = maxHash();
	gamehash maxindex = 0;
	int* positionsfound = calloc(sizeof(int), 256);
	printf("Beginning main loop\n");
	fflush(stdout);
	while(index)
	{
		minprimitive = 255;
		oldindex = index;
		g = fringe[index-1];
		h = getHash(g);
		if(solverread(primitives, h)== 0)
		{
		movecount = generateMoves((char*)&moves, g);
		for(i=0;i<movecount;i++)
		{
			newg = doMove(g, moves[i]);
			h = getHash(newg);
			primitive = solverread(primitives, h);
			if(!primitive)
			{
				primitive = isPrimitive(newg, moves[i]);
				if(primitive != (char) NOT_PRIMITIVE) {
					solverinsert(primitives,h,primitive);
					if(h < minindex) {minindex = h;}
					if(h > maxindex) {maxindex = h;}
					//printf("Position 0x%08x determined primitive\n", newg);
					positionsfound[primitive]++;
					minprimitive = minprimitive <= primitive ? minprimitive : primitive;
				}
				else {
					fringe[index] = newg;
					index++;
				}
			}
			else
			{
				minprimitive = minprimitive <= primitive ? minprimitive : primitive;
			}
		}
		if(index == oldindex) 
		{
			if(minprimitive & 128)
			{
				if(minprimitive & 64) minprimitive = 257-minprimitive;
				else minprimitive = minprimitive + 1;
			}
			else minprimitive = 255-minprimitive;
			h = getHash(g);
			solverinsert(primitives,h,minprimitive);
			if(h < minindex) minindex = h;
			if(h > maxindex) maxindex = h;
			//printf("Position 0x%08x determined fully\n", g);
			positionsfound[minprimitive]++;
			index--;
		}
		}
		else { index--;}
	}
	printf("Done computing, listing statistics\n");
	fflush(stdout);
	int totalpositioncount = 0;
	for(int i = 1; i < 64; i++)
	{
		if(positionsfound[i]) printf("Loss in %d: %d\n", i-1, positionsfound[i]);
		totalpositioncount+=positionsfound[i];
	}
	for(int i = 128; i < 192; i++)
	{
		if(positionsfound[i]) printf("Tie in %d: %d\n", i-128, positionsfound[i]);
		totalpositioncount+=positionsfound[i];
	}
	for(int i = 254; i >= 192; i--)
	{
		if(positionsfound[i]) printf("Win in %d: %d\n", 255-i, positionsfound[i]);
		totalpositioncount+=positionsfound[i];
	}
	printf("In total, %d positions found\n", totalpositioncount);
	printf("%d primitive positions found\n", positionsfound[LOSS]+positionsfound[TIE]);
	printf("Starting position has value %d\n", solverread(primitives, getStartingPositions()));
	printf("%llx, %llx\n", minindex, maxindex);
	printf("Starting output to file %s\n", argv[1]);
	fflush(stdout);
	FILE* file = fopen(argv[1], "w");
	solversave(primitives, file);
	freesolver(primitives);
	printf("Done outputting\n");
	fflush(stdout);
	free(fringe);
	free(positionsfound);
	return 0;
}
