/************************************************************************
**
** NAME:        mtopitop.c
**
** DESCRIPTION: Topitop
**
** AUTHOR:      
**
** DATE:        BEGIN: 02/20/2006
**	              END: 04/09/2006
**
**************************************************************************/

#include <stdio.h>
#include "gamesman.h"

/* Defines */
#define BLUE 'B'
#define RED 'R'

#define BLUEBUCKETPIECE 'B'
#define REDBUCKETPIECE 'R'
#define SMALLPIECE 'S'
#define LARGEPIECE 'L'
#define CASTLEPIECE 'C'
#define BLUESMALLPIECE 'X'
#define REDSMALLPIECE 'O'
#define BLUECASTLEPIECE 'P'
#define REDCASTLEPIECE 'Q'
#define BLANKPIECE '-'
#define ANYPIECE '*'

#define BLUEDISPLAY "  B  "
#define REDDISPLAY "  R  "
#define SMALLDISPLAY " /_\\ "
#define LARGEDISPLAY "/___\\"
#define BLANKDISPLAY "     "

#define NULLMOVE 0b1111111111
/***/

POSITION gNumberOfPositions = 2908571488;
POSITION kBadPosition = -1;
//                            TMMMMMMMMMMLLLSSSRRBB888877776666555544443333222211110000
POSITION gInitialPosition = 0b011111111111001001010000000000000000000000000000000000000;
POSITION gMinimalPosition = 0;

int gSymmetryMatrix[8][9] = {
    {0,1,2,3,4,5,6,7,8},
    {6,3,0,7,4,1,8,5,2},
    {8,7,6,5,4,3,2,1,0},
    {2,5,8,1,4,7,0,3,6},
    {2,1,0,5,4,3,8,7,6},
    {0,3,6,1,4,7,2,5,8},
    {6,7,8,3,4,5,0,1,2},
    {8,5,2,7,4,1,6,3,0}
};

int numAdjacencies[9] = {3, 5, 3, 5, 8, 5, 3, 5, 3};

int adjacencyMatrix[9][8] = {
	{1, 3, 4, 9, 9, 9, 9, 9},
	{0, 2, 3, 4, 5, 9, 9, 9},
	{1, 4, 5, 9, 9, 9, 9, 9},
	{0, 1, 4, 6, 7, 9, 9, 9},
	{0, 1, 2, 3, 5, 6, 7, 8},
	{1, 2, 4, 7, 8, 9, 9, 9},
	{3, 4, 7, 9, 9, 9, 9, 9},
	{3, 4, 5, 6, 8, 9, 9, 9},
	{4, 5, 7, 9, 9, 9, 9, 9}
};

STRING kAuthorName = "";
STRING kGameName = "Topitop";
STRING kDBName = "topitop";   /* The name to store the database under */
BOOLEAN kPartizan = TRUE;
BOOLEAN kDebugMenu = FALSE;
BOOLEAN kGameSpecificMenu = FALSE;
BOOLEAN kTieIsPossible = FALSE;
BOOLEAN kLoopy = TRUE;
BOOLEAN kDebugDetermineValue = FALSE;
BOOLEAN kSupportsSymmetries = TRUE; /* Whether we support symmetries */
void* gGameSpecificTclInit = NULL;

STRING kHelpGraphicInterface = "";

STRING kHelpTextInterface = "";

STRING kHelpOnYourTurn =
"Each player takes turns making one valid move, also noting\n\
that player CANNOT reverse an opponent's move that was just made.\n\
Use the BOARD to determine which numbers to choose to\n\
correspond to either a piece to place and a board slot to place it\n\
OR to the location of your piece or a neutral piece and the empty\n\
adjacent position you wish to move that piece to.\n\
\n\
VALID MOVES:\n\
1.) Place 1 of your Buckets or 1 of the Small or Large Sand Piles\n\
    on any free space of the board.\n\
2.) Move 1 of your Buckets or any Small or Large Sand Pile already\n\
    placed on the board from one square to another (one space at\n\
    a time in any direction).\n\
       a.) Any piece already on the board can be moved to an \n\
           adjacent free space.\n\
       b.) A Bucket can go on top of a Small Sand Pile.\n\
       c.) A Small Sand Pile can go on top of a Large Sand Pile.\n\
       d.) A Small Sand Pile with a Bucket on its top can go on\n\
           top of a Large Sand Pile.\n\
       e.) A Bucket can go on top of a Sand Castle.\n\
    NOTE: You CANNOT place a piece on top of another piece.\n\
          Pieces must be MOVED on top of other pieces.\n\
3.) Move any combination of Sand Piles with your Buckets on top,\n\
    or any Sand Castle, to any free space.";

STRING kHelpStandardObjective =
"Be the first to have your 2 Buckets on top of 2 Sand Castles,\n\
where a Sand Castle consists of 1 Small Sand Pile put on top\n\
of 1 Large Sand Pile.";

STRING kHelpReverseObjective = "";

STRING kHelpTieOccursWhen = "A tie never occurs.";

STRING kHelpExample = "";

/*************************************************************************
**
** Everything above here must be in every game file
**
**************************************************************************/

void InitializeGame();
void DebugMenu();
char* unhashPosition(POSITION position, char *turn, MOVE *prevMove, int *blueLeft, int *redLeft, int *smallLeft, int *largeLeft);
POSITION hashPosition(char* board, char turn, MOVE prevMove);
void unhashMove(MOVE move, char *piece, int *from, int *to);
MOVE hashMove(char piece, int from, int to);
void GameSpecificMenu();
void SetTclCGameSpecificOptions(int theOptions[]);
POSITION DoMove(POSITION position, MOVE move);
POSITION GetInitialPosition();
void PrintComputersMove(MOVE computersMove, STRING computersName);
char* PrintHelper(char piece, int level);
void PrintPosition(POSITION position, STRING playerName, BOOLEAN usersTurn);
MOVELIST *GenerateMoves(POSITION position);
POSITION GetCanonicalPosition(POSITION position);
USERINPUT GetAndPrintPlayersMove(POSITION thePosition, MOVE *theMove, STRING playerName);
BOOLEAN ValidTextInput(STRING input);
MOVE ConvertTextInputToMove(STRING input);
void PrintMove(MOVE theMove);
STRING MoveToString(MOVE move);
int NumberOfOptions();
int getOption();
void setOption(int option);
POSITION InteractStringToPosition(STRING board);
STRING InteractPositionToString(POSITION pos);
STRING InteractPositionToEndData(POSITION pos);
STRING InteractMoveToString(POSITION pos, MOVE mv);

void countPiecesOnBoard(char *board, int *bb, int *rb, int *bs, int *rs, int *bc, int *rc, int *s, int *l, int *c);

void unhashTier(TIER tier, int *bb, int *rb, int *bs, int *rs, int *bc, int *rc, int *s, int *l, int *c);
TIER hashTier(TIER bb, TIER rb, TIER bs, TIER rs, TIER bc, TIER rc, TIER s, TIER l, TIER c);
TIERLIST* gTierChildren(TIER tier);
TIERPOSITION gNumberOfTierPositions(TIER tier);
STRING TierToString(TIER tier);

POSITION fact(int n);

int (*symmetriesToUse)[9];

void InitializeGame() {
	gCanonicalPosition = GetCanonicalPosition;
	gMoveToStringFunPtr = &MoveToString;

	kSupportsTierGamesman = TRUE;
	kExclusivelyTierGamesman = TRUE;
	gInitialTier = 0;
	gInitialTierPosition = 0;
	gTierChildrenFunPtr = &gTierChildren;
	gNumberOfTierPositionsFunPtr = &gNumberOfTierPositions;
	gTierToStringFunPtr = &TierToString;

	symmetriesToUse = gSymmetryMatrix;
}

void DebugMenu() {

}

POSITION fact(int n) {
     if (n <= 1) return 1;
	 POSITION prod = 1;
	 for (int i = 2; i <= n; i++)
		 prod *= i;
     return prod;
}

char* unhashBoard(TIER tier, TIERPOSITION tierposition, MOVE *prevMove, int *blueLeft, int *redLeft, int *smallLeft, int *largeLeft) {
	char *board = (char*) SafeMalloc(sizeof(char) * 9);
	return;
}

void hashBoard(char *board, TIER *tier, TIERPOSITION *tierposition) {
	return;
}

char* unhashPosition(POSITION position, char *turn, MOVE *prevMove, int *blueLeft, int *redLeft, int *smallLeft, int *largeLeft) {
	if (gHashWindowInitialized) {
		TIER tier; TIERPOSITION tierposition;
		gUnhashToTierPosition(position, &tierposition, &tier);
		generic_hash_context_switch(tier);
		char *board = unhashToBoard(tier, tierposition, turn, prevMove, blueLeft, redLeft, smallLeft, largeLeft);//////
		return board;
	} else { // Not supported.
		return board;
	}
}

POSITION hashPosition(char* board, char turn, MOVE prevMove) {
	if (gHashWindowInitialized) {
		TIER tier; TIERPOSITION tierposition;
		hashBoard(board, &tier, &tierposition);
		generic_hash_context_switch(tier);
		POSITION position = gHashToWindowPosition(tierposition, tier);
		return position;
	} else { // Not supported.
		return 0;
	}
}

void countPiecesOnBoard(char *board, int *bb, int *rb, int *bs, int *rs, int *bc, int *rc, int *s, int *l, int *c) {
	*bb = *rb = *bs = *rs = *bc = *rc = *s = *l = *c = 0;
	for (int i = 0; i < 9; i++) {
		switch(board[i]) {
			case BLUEBUCKETPIECE:
				(*bb)++;
				break;
			case REDBUCKETPIECE:
				(*rb)++;
				break;
			case BLUESMALLPIECE:
				(*bs)++;
				break;
			case REDSMALLPIECE:
				(*rs)++;
				break;
			case BLUECASTLEPIECE:
				(*bc)++;
				break;
			case REDCASTLEPIECE:
				(*rc)++;
				break;
			case SMALLPIECE:
				(*s)++;
				break;
			case LARGEPIECE:
				(*l)++;
				break;
			case CASTLEPIECE:
				(*c)++;
				break;
			default:
				break;
		}
	}
}

void unhashTier(TIER tier, int *bb, int *rb, int *bs, int *rs, int *bc, int *rc, int *s, int *l, int *c) {
	int mask = 0b11;
	*bb = tier & mask;
	tier >>= 2;
	*rb = tier & mask;
	tier >>= 2;
	*bs = tier & mask;
	tier >>= 2;
	*rs = tier & mask;
	tier >>= 2;
	*bc = tier & mask;
	tier >>= 2;
	*rc = tier & mask;
	tier >>= 2;

	mask = 0b111;
	*s = tier & mask;
	tier >>= 3;
	*l = tier & mask;
	tier >>= 3;
	*c = tier & mask;
}

TIER hashTier(TIER bb, TIER rb, TIER bs, TIER rs, TIER bc, TIER rc, TIER s, TIER l, TIER c) {
	return bb | (rb << 2) | (bs << 4) | (rs << 6) | (bc << 8) | (rc << 10) | (s << 12) | (l << 15) | (c << 18);
}

TIERLIST* gTierChildren(TIER tier) {
	int bb, rb, bs, rs, bc, rc, s, l, c;
	unhashTier(tier, &bb, &rb, &bs, &rs, &bc, &rc, &s, &l, &c);
	int total = bb + rb + bs + rs + bc + rc + s + l + c;

	TIERLIST* list = NULL;
	if (bc == 2 || rc == 2) return list; // Primitive tiers contain two castles of the same color
	if (tier != gInitialTier) list = CreateTierlistNode(tier, list);
	
	// Placement
	if (total < 9) {
		if (bb + bs + bc < 2) list = CreateTierlistNode(hashTier(bb + 1, rb, bs, rs, bc, rc, s, l, c), list);
		if (rb + rs + rc < 2) list = CreateTierlistNode(hashTier(bb, rb + 1, bs, rs, bc, rc, s, l, c), list); // If there are red buckets to place
		if (s + c + rc + bc + rs + bs < 4) list = CreateTierlistNode(hashTier(bb, rb, bs, rs, bc, rc, s + 1, l, c), list); // If there are smalls to place
		if (l + c + rc + bc < 4) list = CreateTierlistNode(hashTier(bb, rb, bs, rs, bc, rc, s, l + 1, c), list); // If there are larges to place
	}

	// Sliding
	if (bb > 0 && s > 0) list = CreateTierlistNode(hashTier(bb - 1, rb, bs + 1, rs, bc, rc, s - 1, l, c), list); // Blue + Small = BlueSmall
	if (rb > 0 && s > 0) list = CreateTierlistNode(hashTier(bb, rb - 1, bs, rs + 1, bc, rc, s - 1, l, c), list); // Red + Small = RedSmall
	if (bs > 0 && l > 0) list = CreateTierlistNode(hashTier(bb, rb, bs - 1, rs, bc + 1, rc, s, l - 1, c), list); // BlueSmall + Large = BlueCastle
	if (rs > 0 && l > 0) list = CreateTierlistNode(hashTier(bb, rb, bs, rs - 1, bc, rc + 1, s, l - 1, c), list); // RedSmall + Large = RedCastle
	if (s > 0 && l > 0) list = CreateTierlistNode(hashTier(bb, rb, bs, rs, bc, rc, s - 1, l - 1, c + 1), list); // Small + Large = Castle
	if (bb > 0 && c > 0) list = CreateTierlistNode(hashTier(bb - 1, rb, bs, rs, bc + 1, rc, s, l, c - 1), list); // Blue + Castle = BlueCastle
	if (rb > 0 && c > 0) list = CreateTierlistNode(hashTier(bb, rb - 1, bs, rs, bc, rc + 1, s, l, c - 1), list); // Red + Castle = RedCastle
	return list;
}

BOOLEAN isLegal(POSITION position) {
	char turn;
	MOVE prevMove;
	int blueLeft, redLeft, smallLeft, largeLeft;
	char *board = unhashPosition(position, &turn, &prevMove, &blueLeft, &redLeft, &smallLeft, &largeLeft);

	

	SafeFree(board);
	return FALSE;
}

TIERPOSITION gNumberOfTierPositions(TIER tier) {
	int bb, rb, bs, rs, bc, rc, s, l, c;
	unhashTier(tier, &bb, &rb, &bs, &rs, &bc, &rc, &s, &l, &c);
	int numBlanks = 9 - bb - rb - bs - rs - bc - rc - s - l - c;
	// 9! * 41 (Upper bound on # of possible prevMoves) * 2 (to account for turn) = 29756160 
	return 29756160 / (fact(numBlanks) * fact(bb) * fact(rb) * fact(bs) * fact(rs) * fact(bc) * fact(rc) * fact(s) * fact(l) * fact(c));
}

STRING TierToString(TIER tier) {
	int bb, rb, bs, rs, bc, rc, s, l, c;
	unhashTier(tier, &bb, &rb, &bs, &rs, &bc, &rc, &s, &l, &c);
	STRING str = (STRING) SafeMalloc(sizeof(char) * 100);
	sprintf(str, "BB: %d, RB: %d, BS: %d, RS: %d, BC: %d, RC: %d, S: %d, L: %d, C: %d", bb, rb, bs, rs, bc, rc, s, l, c);
	return str;
}

void unhashMove(MOVE move, char *piece, int *from, int *to) {
	*to = move & 0b1111;
	move >>= 4;
	*from = move & 0b1111;
	move >>= 4;
	switch (move) {
		case 1:
			*piece = REDBUCKETPIECE;
			break;
		case 2:
			*piece = SMALLPIECE;
			break;
		case 3:
			*piece = LARGEPIECE;
			break;
		default:
			*piece = BLUEBUCKETPIECE;
			break;
	}
}

MOVE hashMove(char piece, int from, int to) {
	MOVE theHash = 0;
	theHash |= to;
	theHash |= from << 4;
	switch (piece) {
		case REDBUCKETPIECE:
			theHash |= 1 << 8;
			break;
		case SMALLPIECE:
			theHash |= 2 << 8;
			break;
		case LARGEPIECE:
			theHash |= 3 << 8;
			break;
		default:
			break;
	}
	return theHash;
}

/************************************************************************
**
** NAME: GameSpecificMenu
**
** DESCRIPTION: Menu used to change game-specific parmeters, such as
** the side of the board in an nxn Nim board, etc. Does
** nothing if kGameSpecificMenu == FALSE
**
************************************************************************/

void GameSpecificMenu() {
}

/************************************************************************
**
** NAME: SetTclCGameSpecificOptions
**
** DESCRIPTION: Set the C game-specific options (called from Tcl)
** Ignore if you don't care about Tcl for now.
**
************************************************************************/
void SetTclCGameSpecificOptions(int theOptions[])
{
}

/************************************************************************
**
** NAME: DoMove
**
** DESCRIPTION: Apply the move to the position.
**
** INPUTS: POSITION position : The old position
** MOVE move : The move to apply.
**
** OUTPUTS: (POSITION) : The position that results after the move.
**
** CALLS: PositionToBlankOX(POSITION,*BlankOX)
** BlankOX WhosTurn(*BlankOX)
**
************************************************************************/

POSITION DoMove(POSITION position, MOVE move) {
	char turn;
	MOVE prevMove;
	int blueLeft, redLeft, smallLeft, largeLeft;
	char *board = unhashPosition(position, &turn, &prevMove, &blueLeft, &redLeft, &smallLeft, &largeLeft);

	if (move == NULLMOVE) {
		position = hashPosition(board, (turn == BLUE) ? RED : BLUE, move);
		SafeFree(board);
		return position;
	}
	
	char piece;
	int from, to;
	unhashMove(move, &piece, &from, &to);
	if (from == to) { // Placement
		board[to] = piece;
		switch (piece) {
			case BLUEBUCKETPIECE:
				blueLeft--;
				break;
			case REDBUCKETPIECE:
				redLeft--;
				break;
			case SMALLPIECE:
				smallLeft--;
				break;
			case LARGEPIECE:
				largeLeft--;
				break;
			default:
				break;
		}
	} else { // SLIDING
		switch (board[to]) {
			case BLANKPIECE:
				board[to] = board[from];
				break;
			case SMALLPIECE:
				if (board[from] == BLUEBUCKETPIECE) {
					board[to] = BLUESMALLPIECE;
				} else {
					board[to] = REDSMALLPIECE;
				}
				break;
			case LARGEPIECE:
				if (board[from] == SMALLPIECE) {
					board[to] = CASTLEPIECE;
				} else if (board[from] == BLUESMALLPIECE) {
					board[to] = BLUECASTLEPIECE;
				} else {
					board[to] = REDCASTLEPIECE;
				}
				break;
			case CASTLEPIECE:
				if (board[from] == BLUEBUCKETPIECE) {
					board[to] = BLUECASTLEPIECE;
				} else {
					board[to] = REDCASTLEPIECE;
				}
				break;
			default:
				break;
		}
		board[from] = BLANKPIECE;
	}

	position = hashPosition(board, (turn == BLUE) ? RED : BLUE, move);

	SafeFree(board);

	return position;
}


/************************************************************************
**
** NAME: GetInitialPosition
**
** DESCRIPTION: Ask the user for an initial position for testing. Store
** it in the space pointed to by initialPosition;
**
** OUTPUTS: POSITION initialPosition : The position to fill.
**
************************************************************************/

POSITION GetInitialPosition() {
	char board[9] = {BLANKPIECE, BLANKPIECE, BLANKPIECE, BLANKPIECE, BLANKPIECE, BLANKPIECE, BLANKPIECE, BLANKPIECE, BLANKPIECE};
	return hashPosition(board, BLUE, NULLMOVE);
}

/************************************************************************
**
** NAME: PrintComputersMove
**
** DESCRIPTION: Nicely format the computers move.
**
** INPUTS: MOVE computersMove : The computer's move.
** STRING computersName : The computer's name.
**
************************************************************************/

void PrintComputersMove(MOVE computersMove, STRING computersName) {
	STRING str = MoveToString(computersMove);
	printf("%8s's move: %s\n", computersName, str);
	SafeFree(str);
}

VALUE Primitive(POSITION position) {
	char turn;
	MOVE prevMove;
	int blueLeft, redLeft, smallLeft, largeLeft;
	char *board = unhashPosition(position, &turn, &prevMove, &blueLeft, &redLeft, &smallLeft, &largeLeft);
	int bb, rb, bs, rs, bc, rc, s, l, c;
	countPiecesOnBoard(board, &bb, &rb, &bs, &rs, &bc, &rc, &s, &l, &c);
	SafeFree(board);
	if (bc == 2 || rc == 2) return lose;
	return undecided;
}

/************************************************************************
**
** NAME: PrintPosition
**
**     *-------*-------*-------*
**     |   B   |       |       |  Turn: Blue
**     |  /_\  |       |       |  Previous Move: 3-4
**     | /___\ |  /_\  |   R   |
**     *-------+-------+-------*  Remaining Pieces to Place:
**     |       |       |       |  Blue Bucket (0)
**     |       |       |       |  Red Bucket (0)
**     |       | /___\ | /___\ |  Small Sand Pile (0)
**     *-------+-------+-------*  Large Sand Pile (0)
**     |       |       |       |
**     |  /_\  |   R   |       |  <Prediction>
**     | /___\ |  /_\  |   B   |
**     *-------*-------*-------*
**
************************************************************************/

char* PrintHelper(char piece, int level) {
	if (level == 0) {
		switch (piece) {
			case BLUECASTLEPIECE:
			case REDCASTLEPIECE:
			case CASTLEPIECE:
			case LARGEPIECE:
				return LARGEDISPLAY;
			case BLUESMALLPIECE:
			case REDSMALLPIECE:
			case SMALLPIECE:
				return SMALLDISPLAY;
			case BLUEBUCKETPIECE:
				return BLUEDISPLAY;
			case REDBUCKETPIECE:
				return REDDISPLAY;
			default:
				return BLANKDISPLAY;
		}
	} else if (level == 1) {
		switch (piece) {
			case BLUECASTLEPIECE:
			case REDCASTLEPIECE:
			case CASTLEPIECE:
				return SMALLDISPLAY;
			case BLUESMALLPIECE:
				return BLUEDISPLAY;
			case REDSMALLPIECE:
				return REDDISPLAY;
			default:
				return BLANKDISPLAY;
		}
	} else {
		if (piece == BLUECASTLEPIECE) return BLUEDISPLAY;
		if (piece == REDCASTLEPIECE) return REDDISPLAY;
		return BLANKDISPLAY;
	}
}

void PrintPosition(POSITION position, STRING playerName, BOOLEAN usersTurn) {
	char turn;
	MOVE prevMove;
	int blueLeft, redLeft, smallLeft, largeLeft;
	char *board = unhashPosition(position, &turn, &prevMove, &blueLeft, &redLeft, &smallLeft, &largeLeft);
	char *strPrevMove = MoveToString(prevMove);

	printf("PRINTING POSITION %llu\n", position);
	for (int i = 0; i < 9; i++) {
		printf("<%c>", board[i]);
	}
	printf("\n");

	printf("\n");
	printf("        *-------*-------*-------*\n");
	printf("        | %s | %s | %s |  Turn: %s\n", PrintHelper(board[0], 2), PrintHelper(board[1], 2), PrintHelper(board[2], 2), (turn == BLUE) ? "Blue" : "Red");
	printf("        | %s | %s | %s |  Previous Move: %s\n", PrintHelper(board[0], 1), PrintHelper(board[1], 1), PrintHelper(board[2], 1), strPrevMove);
	printf("        | %s | %s | %s |\n", PrintHelper(board[0], 0), PrintHelper(board[1], 0), PrintHelper(board[2], 0));
	printf("        *-------*-------*-------*  Remaining Pieces to Place:\n");
	printf("        | %s | %s | %s |  Blue Bucket (%d)\n", PrintHelper(board[3], 2), PrintHelper(board[4], 2), PrintHelper(board[5], 2), blueLeft);
	printf("        | %s | %s | %s |  Red Bucket (%d)\n", PrintHelper(board[3], 1), PrintHelper(board[4], 1), PrintHelper(board[5], 1), redLeft);
	printf("        | %s | %s | %s |  Small Sand Pile (%d)\n", PrintHelper(board[3], 0), PrintHelper(board[4], 0), PrintHelper(board[5], 0), smallLeft);
	printf("        *-------*-------*-------*  Large Sand Pile (%d)\n", largeLeft);
	printf("        | %s | %s | %s |\n", PrintHelper(board[6], 2), PrintHelper(board[7], 2), PrintHelper(board[8], 2));
	printf("        | %s | %s | %s |  %s\n", PrintHelper(board[6], 1), PrintHelper(board[7], 1), PrintHelper(board[8], 1), GetPrediction(position, playerName, usersTurn));
	printf("        | %s | %s | %s |\n", PrintHelper(board[6], 0), PrintHelper(board[7], 0), PrintHelper(board[8], 0));
	printf("        *-------*-------*-------*\n\n");

	SafeFree(board);
	SafeFree(strPrevMove);
}

MOVELIST *GenerateMoves(POSITION position) {
	MOVELIST *moveList = NULL;

	char turn;
	MOVE prevMove;
	int blueLeft, redLeft, smallLeft, largeLeft;
	char *board = unhashPosition(position, &turn, &prevMove, &blueLeft, &redLeft, &smallLeft, &largeLeft);

	char prevPiece;
	int prevFrom, prevTo;
	unhashMove(prevMove, &prevPiece, &prevFrom, &prevTo);

	for (int to = 0; to < 9; to++) {
		if (board[to] == BLANKPIECE) {
	
			// PLACEMENT //
			if (blueLeft > 0 && turn == BLUE)
				moveList = CreateMovelistNode(hashMove(BLUEBUCKETPIECE, to, to), moveList);
			else if (redLeft > 0 && turn == RED)
				moveList = CreateMovelistNode(hashMove(REDBUCKETPIECE, to, to), moveList);

			if (smallLeft > 0) moveList = CreateMovelistNode(hashMove(SMALLPIECE, to, to), moveList);
      		if (largeLeft > 0) moveList = CreateMovelistNode(hashMove(LARGEPIECE, to, to), moveList);

			// SLIDE TO BLANK //
			for (int j = 0; j < numAdjacencies[to]; j++) {
				int from = adjacencyMatrix[to][j];
				if (to != prevFrom || from != prevTo) { // Don't undo previous person's slide
					if ((turn == BLUE && board[from] != BLANKPIECE && board[from] != REDBUCKETPIECE && board[from] != REDSMALLPIECE && board[from] != REDCASTLEPIECE) || (turn == RED && board[from] != BLANKPIECE && board[from] != BLUEBUCKETPIECE && board[from] != BLUESMALLPIECE && board[from] != BLUECASTLEPIECE))
						moveList = CreateMovelistNode(hashMove(ANYPIECE, from, to), moveList);
				}
			}
		} else if (board[to] == SMALLPIECE || board[to] == CASTLEPIECE) { // SLIDE ONTO SMALL PIECE OR CASTLE PIECE //
			for (int j = 0; j < numAdjacencies[to]; j++) {
				int from = adjacencyMatrix[to][j];
				if ((turn == BLUE && board[from] == BLUEBUCKETPIECE) || (turn == RED && board[from] == REDBUCKETPIECE))
					moveList = CreateMovelistNode(hashMove(ANYPIECE, from, to), moveList);
			}
		} else if (board[to] == LARGEPIECE) { // SLIDE ONTO LARGE PIECE //
			for (int j = 0; j < numAdjacencies[to]; j++) {
				int from = adjacencyMatrix[to][j];
				if (board[from] == SMALLPIECE || (turn == BLUE && board[from] == BLUESMALLPIECE) || (turn == RED && board[from] == REDSMALLPIECE))
					moveList = CreateMovelistNode(hashMove(ANYPIECE, from, to), moveList);
			}
		}
	}

	SafeFree(board);

	// Add passing move if no legal moves.
	if (moveList == NULL) {
		moveList = CreateMovelistNode(NULLMOVE, moveList);
	}

	return moveList;
}

/************************************************************************
**
** NAME: GetCanonicalPosition
**
** DESCRIPTION: Go through all of the positions that are symmetrically
** equivalent and return the SMALLEST, which will be used
** as the canonical element for the equivalence set.
**
** INPUTS: POSITION position : The position return the canonical elt. of.
**
** OUTPUTS: POSITION : The canonical element of the set.
**
************************************************************************/

POSITION GetCanonicalPosition(POSITION position) {
	char turn;
	MOVE prevMove;
	int blueLeft, redLeft, smallLeft, largeLeft;
	char *originalBoard = unhashPosition(position, &turn, &prevMove, &blueLeft, &redLeft, &smallLeft, &largeLeft);

	char piece;
	int from, to;
	unhashMove(prevMove, &piece, &from, &to);

	char canonBoard[9];
	MOVE canonPrevMove;
    POSITION canonPos = position;
    int bestSymmetryNum = 0;

    for (int symmetryNum = 1; symmetryNum < 8; symmetryNum++)
        for (int i = 0; i < 9; i++) {
            char pieceInSymmetry = originalBoard[symmetriesToUse[symmetryNum][i]];
            char pieceInBest = originalBoard[symmetriesToUse[bestSymmetryNum][i]];
            if (pieceInSymmetry != pieceInBest) {
                if (pieceInSymmetry > pieceInBest) // If new smallest hash.
                    bestSymmetryNum = symmetryNum;
                break;
            }
        };

    if (bestSymmetryNum == 0) {
		SafeFree(originalBoard);
        return position;
	}
    
    for (int i = 0; i < 9; i++) // Transform the rest of the board.
        canonBoard[i] = originalBoard[symmetriesToUse[bestSymmetryNum][i]];
	
	canonPrevMove = hashMove(piece, symmetriesToUse[bestSymmetryNum][from], symmetriesToUse[bestSymmetryNum][to]);

    canonPos = hashPosition(canonBoard, turn, canonPrevMove);
	
	SafeFree(originalBoard);

    return canonPos;
}

/************************************************************************
**
** NAME: GetAndPrintPlayersMove
**
** DESCRIPTION: This finds out if the player wanted an undo or abort or not.
** If so, return Undo or Abort and don't change theMove.
** Otherwise get the new theMove and fill the pointer up.
**
** INPUTS: POSITION *thePosition : The position the user is at.
** MOVE theMove : The move to fill with user's move.
** STRING playerName : The name of the player whose turn it is
**
** OUTPUTS: USERINPUT : Oneof( Undo, Abort, Continue )
**
** CALLS: ValidMove(MOVE, POSITION)
** BOOLEAN PrintPossibleMoves(POSITION) ...Always True!
**
************************************************************************/

USERINPUT GetAndPrintPlayersMove(POSITION position, MOVE *move, STRING playerName) {
	USERINPUT input;
	do {
		printf("%8s's move: (u)ndo", playerName);
		printf(": ");

		input = HandleDefaultTextInput(position, move, playerName);

		if (input != Continue)
			return input;
	} while (TRUE);

	/* NOT REACHED */
	return Continue;
}

/************************************************************************
**
** NAME: ValidTextInput
**
** DESCRIPTION: Return TRUE iff the string input is of the right 'form'.
** For example, if the user is allowed to select one slot
** from the numbers 1-9, and the user chooses 0, it's not
** valid, but anything from 1-9 IS, regardless if the slot
** is filled or not. Whether the slot is filled is left up
** to another routine.
**
** INPUTS: STRING input : The string input the user typed.
**
** OUTPUTS: BOOLEAN : TRUE iff the input is a valid text input.
**
************************************************************************/

BOOLEAN ValidTextInput(STRING input) {
	return (input[0] >= 48 && input[0] <= 56 && input[1] == '-' && input[2] >= 48 && input[2] <= 56 && input[3] == '\0') || // Movement
			((input[0] == 'B' || input[0] == 'R' || input[0] == 'S' || input[0] == 'L') && input[1] >= 48 && input[1] <= 56 && input[2] == '\0') || // Placement
			strcmp(input, "None") == 0; // Pass Turn
}

/************************************************************************
**
** NAME: ConvertTextInputToMove
**
** DESCRIPTION: Convert the string input to the internal move representation.
**
** INPUTS: STRING input : The string input the user typed.
**
** OUTPUTS: MOVE : The move corresponding to the user's input.
**
************************************************************************/

MOVE ConvertTextInputToMove(STRING input) {
	char piece = ANYPIECE;
	if (input[0] >= 48 && input[0] <= 56) {// Movement
		return hashMove(piece, input[0] - '0', input[2] - '0');
	} else if (input[0] == 'N') {
		return NULLMOVE;
	} else {
		switch(input[0]) {
			case 'B':
				piece = BLUEBUCKETPIECE;
				break;
			case 'R':
				piece = REDBUCKETPIECE;
				break;
			case 'S':
				piece = SMALLPIECE;
				break;
			case 'L':
				piece = LARGEPIECE;
				break;
			default:
				break;
		}
		return hashMove(piece, input[1] - '0', input[1] - '0');
	}
}

/************************************************************************
**
** NAME: PrintMove
**
** DESCRIPTION: Print the move in a nice format.
**
** INPUTS: MOVE *theMove : The move to print.
**
************************************************************************/

void PrintMove(MOVE move) {
	STRING str = MoveToString(move);
	printf("%s", str);
	SafeFree(str);
}

/************************************************************************
**
** NAME: MoveToString
**
** DESCRIPTION: Returns the move as a STRING
**
** INPUTS: MOVE *move : The move to put into a string.
**
************************************************************************/

STRING MoveToString(MOVE move) {
	char piece;
	int from, to;
	if (move == NULLMOVE) {
		STRING moveString = (STRING) SafeMalloc(7);
		sprintf(moveString, "[None]");
		return moveString;
	}
	unhashMove(move, &piece, &from, &to);
	if (from == to) { // Placement
		STRING moveString = (STRING) SafeMalloc(5);
		sprintf(moveString, "[%c%d]", piece, from);
		return moveString;
	} else { // Sliding
		STRING moveString = (STRING) SafeMalloc(6);
		sprintf(moveString, "[%d-%d]", from, to);
		return moveString;
	}
}

int NumberOfOptions() {
	return 1;
}

int getOption() {
	return 0;
}

void setOption(int option) {
}

POSITION InteractStringToPosition(STRING board) {
	return 0;
}

STRING InteractPositionToString(POSITION pos) {
	return NULL;
}

STRING InteractPositionToEndData(POSITION pos) {
	return NULL;
}

STRING InteractMoveToString(POSITION pos, MOVE mv) {
	return MoveToString(mv);
}

