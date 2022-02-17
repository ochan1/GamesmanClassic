/************************************************************************
**
** NAME:        mtopitop.c
**
** DESCRIPTION: Topitop
**
** AUTHOR:      Cameron Cheung & Matthew Yu
**
** DATE:        BEGIN: 02/20/2006
**                END: 04/09/2006
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




#define BLUEDISPLAY "  B  "
#define REDDISPLAY "  R  "
#define SMALLDISPLAY " /_\\ "
#define LARGEDISPLAY "/___\\"
#define BLANKDISPLAY "     "

#define NULLMOVE 0b1111111111
/***/

POSITION gNumberOfPositions = 0;
POSITION kBadPosition = -1;
//                            TMMMMMMMMMMLLLSSSRRBB888877776666555544443333222211110000
POSITION gInitialPosition = 0b011111111111001001010000000000000000000000000000000000000;
POSITION gMinimalPosition = 0;

STRING kAuthorName = "";
STRING kGameName = "Topitop";
STRING kDBName = "topitop";   /* The name to store the database under */
BOOLEAN kPartizan = TRUE;
BOOLEAN kDebugMenu = FALSE;
BOOLEAN kGameSpecificMenu = FALSE;
BOOLEAN kTieIsPossible = FALSE;
BOOLEAN kLoopy = TRUE;
BOOLEAN kDebugDetermineValue = FALSE;
BOOLEAN kSupportsSymmetries = FALSE; /* Whether we support symmetries */
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
POSITION hashPosition(char* board, char turn, MOVE prevMove, int blueLeft, int redLeft, int smallLeft, int largeLeft);
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

void InitializeGame() {
  gCanonicalPosition = GetCanonicalPosition;
  gMoveToStringFunPtr = &MoveToString;
}

void DebugMenu() {

}

char* unhashPosition(POSITION position, char *turn, MOVE *prevMove, int *blueLeft, int *redLeft, int *smallLeft, int *largeLeft) {
  char *board = (char*) SafeMalloc(sizeof(char) * 9);
  int mask = 0b1111;
  for (int i = 0; i < 9; i++) {
    switch (position & mask) {
      case 1:
        board[i] = BLUEBUCKETPIECE;
        break;
      case 2:
        board[i] = REDBUCKETPIECE;
        break;
      case 3:
        board[i] = SMALLPIECE;
        break;
      case 4:
        board[i] = LARGEPIECE;
        break;
      case 5:
        board[i] = BLUESMALLPIECE;
        break;
      case 6:
        board[i] = REDSMALLPIECE;
        break;
      case 7:
        board[i] = CASTLEPIECE;
        break;
      case 8:
        board[i] = BLUECASTLEPIECE;
        break;
      case 9:
        board[i] = REDCASTLEPIECE;
        break;
      default:
        board[i] = BLANKPIECE;
        break;
    };
    position >>= 4;
  }

  mask = 0b11;
  *blueLeft = position & mask;
  position >>= 2;
  *redLeft = position & mask;
  position >>= 2;

  mask = 0b111;
  *smallLeft = position & mask;
  position >>= 3;
  *largeLeft = position & mask;
  position >>= 3;
  
  mask = 0b1111111111;
  *prevMove = position & mask;
  position >>= 10;

  *turn = (position & 1) ? RED : BLUE;
  
  return board;
}

POSITION hashPosition(char* board, char turn, MOVE prevMove, int blueLeft, int redLeft, int smallLeft, int largeLeft) {
  POSITION theHash = 0;
  for (int i = 0; i < 9; i++) {
    switch (board[i]) {
      case BLUEBUCKETPIECE:
        theHash |= 1 << (i * 4);
        break;
      case REDBUCKETPIECE:
        theHash |= 2 << (i * 4);
        break;
      case SMALLPIECE:
        theHash |= 3 << (i * 4);
        break;
      case LARGEPIECE:
        theHash |= 4 << (i * 4);
        break;
      case BLUESMALLPIECE:
        theHash |= 5 << (i * 4);
        break;
      case REDSMALLPIECE:
        theHash |= 6 << (i * 4);
        break;
      case CASTLEPIECE:
        theHash |= 7 << (i * 4);
        break;
      case BLUECASTLEPIECE:
        theHash |= 8 << (i * 4);
        break;
      case REDCASTLEPIECE:
        theHash |= 9 << (i * 4);
        break;
      default:
        continue;
    }
  }
  theHash |= ((POSITION) blueLeft) << 36;
  theHash |= ((POSITION) redLeft) << 38;
  theHash |= ((POSITION) smallLeft) << 40;
  theHash |= ((POSITION) largeLeft) << 43;
  theHash |= ((POSITION) prevMove) << 46;
  theHash |= ((POSITION) ((turn == BLUE) ? 0 : 1)) << 56;
  return theHash;
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


MOVE surroundingMoves(char board, char piece, int from) {
  int zero[3] = {1,3,4};
  int one[5] = {0,2,3,4,5};
  int two[3] = {1,4,5};
  int three[5] = {0,1,4,6,7};
  int four[8] = {0,1,2,3,5,6,7,8};
  int five[5] = {1,2,4,7,8};
  int six[3] = {3,4,7};
  int seven[5] = {3,4,5,6,8};
  int eight[3] = {4,5,7};
  if (i == 0) {
    for (int i = 0; i < 3; i++) {
      if (board[zero[i]] == BLANKPIECE) {

      }
    }
  }
  if (i == 1) {

  }
      if (i == 2) {

      }
      if (i == 3) {

      }
      if (i == 4) {

      }


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
  
  char piece;
  int from, to;
  unhashMove(move, &piece, &from, &to);
  if (from == to && board[to] == BLANKPIECE) {
    board[to] = piece;
    if (piece == BLUEBUCKETPIECE) {
      blueLeft--;
    }
    if (piece == REDBUCKETPIECE) {
      redleft--;
    }
    if (piece == SMALLPIECE) {
      smallLeft--;
    }
    if (piece == LARGEPIECE) {
      largeLeft--;
    }
  } 
  else {


    if (board[to] == BLANKPIECE) {
      board[to] = board[from];
    };


    else if (board[to] != BLANKPIECE) {
      if (piece == BLUEBUCKETPIECE) {
        if (board[to] == SMALLPIECE) {
          board[to] = BLUESMALLPIECE;
        }
        else if (board[to] == CASTLEPIECE) {
          board[to] = BLUECASTLEPIECE;
        }
      }

      else if (piece == REDBUCKETPIECE) {
        if (board[to] == SMALLPIECE) {
          board[to] = REDSMALLPIECE;
        }
        else if (board[to] == CASTLEPIECE) {
          board[to] = REDCASTLEPIECE;
        }
      }

      else if (piece == SMALLPIECE && board[to] == LARGEPIECE) {
        board[to] = CASTLEPIECE;
      }
    };
    board[from] = BLANKPIECE;
  }



  if (turn == BLUE) {
    position = hashPosition(board, RED, move, blueLeft, redLeft, smallLeft, largeLeft);
  }
  if (turn == RED) {
    position = hashPosition(board, BLUE, move, blueLeft, redLeft, smallLeft, largeLeft);
  }
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
  char board[9];
  return hashPosition(board, BLUE, NULLMOVE, 2, 2, 4, 4);
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

  printf("\n");
  printf("        *-------*-------*-------*\n");
  printf("        | %s | %s | %s |  Turn: %s\n", PrintHelper(board[0], 2), PrintHelper(board[1], 2), PrintHelper(board[2], 2), (turn == BLUE) ? "Blue" : "Red");
  printf("        | %s | %s | %s |  Previous Move: %s\n", PrintHelper(board[0], 1), PrintHelper(board[1], 1), PrintHelper(board[2], 1), strPrevMove);
  printf("        | %s | %s | %s |\n", PrintHelper(board[0], 0), PrintHelper(board[1], 0), PrintHelper(board[2], 0));
  printf("        *-------*-------*-------*  Remaining Pieces to Place:\n");
  printf("        | %s | %s | %s |  Blue Bucket (%d)\n", PrintHelper(board[3], 2), PrintHelper(board[4], 2), PrintHelper(board[5], 2), blueLeft);
  printf("        | %s | %s | %s |  Red Bucket (%d)\n", PrintHelper(board[3], 2), PrintHelper(board[4], 2), PrintHelper(board[5], 2), redLeft);
  printf("        | %s | %s | %s |  Small Sand Pile (%d)\n", PrintHelper(board[3], 2), PrintHelper(board[4], 2), PrintHelper(board[5], 2), smallLeft);
  printf("        *-------*-------*-------*  Large Sand Pile (%d)\n", largeLeft);
  printf("        | %s | %s | %s |\n", PrintHelper(board[6], 2), PrintHelper(board[7], 2), PrintHelper(board[8], 2));
  printf("        | %s | %s | %s |  %s\n", PrintHelper(board[6], 2), PrintHelper(board[7], 2), PrintHelper(board[8], 2), GetPrediction(position, playerName, usersTurn));
  printf("        | %s | %s | %s |\n", PrintHelper(board[6], 2), PrintHelper(board[7], 2), PrintHelper(board[8], 2));
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

  int zero[3] = {1,3,4};
  int one[5] = {0,2,3,4,5};
  int two[3] = {1,4,5};
  int three[5] = {0,1,4,6,7};
  int four[8] = {0,1,2,3,5,6,7,8};
  int five[5] = {1,2,4,7,8};
  int six[3] = {3,4,7};
  int seven[5] = {3,4,5,6,8};
  int eight[3] = {4,5,7};

  for (int i = 0; i < 9; i++) {
    if (board[i] == BLANKPIECE) {
      if (blueLeft > 0 && turn == BLUE) {
        moveList = CreateMovelistNode(hashMove(BLUEBUCKETPIECE, i, i), moveList);
      }
      if (redLeft > 0 && turn == RED) {
        moveList = CreateMovelistNode(hashMove(REDBUCKETPIECE, i, i), moveList);
      }
      if (smallLeft > 0) {
        moveList = CreateMovelistNode(hashMove(SMALLPIECE, i, i), moveList);
      }
      if (largeLeft > 0) {
        moveList = CreateMovelistNode(hashMove(LARGEPIECE, i, i), moveList);
      }
    }
    if (turn == BLUE) {
      if (board[i] == BLUEBUCKETPIECE) {
        if (i == 0) {
          for (int j = 0; j < 3; j++) {
            if (board[zero[j]] == BLANKPIECE || board[zero[j]] == SMALLPIECE || board[zero[j]] == CASTLEPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUEBUCKETPIECE, i, zero[j]), moveList);
            }
          }
        }
        if (i == 1) {
          for (int j = 0; j < 5; j++) {
            if (board[one[j]] == BLANKPIECE || board[one[j]] == SMALLPIECE || board[one[j]] == CASTLEPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUEBUCKETPIECE, i, one[j]), moveList);
            }
          }
        }
        if (i == 2) {
          for (int j = 0; j < 3; j++) {
            if (board[two[j]] == BLANKPIECE || board[two[j]] == SMALLPIECE || board[two[j]] == CASTLEPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUEBUCKETPIECE, i, two[j]), moveList);
            }
          }
        }
        if (i == 3) {
          for (int j = 0; j < 5; j++) {
            if (board[three[j]] == BLANKPIECE || board[three[j]] == SMALLPIECE || board[three[j]] == CASTLEPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUEBUCKETPIECE, i, three[j]), moveList);
            }
          }
        }
        if (i == 4) {
          for (int j = 0; j < 8; j++) {
            if (board[four[j]] == BLANKPIECE || board[four[j]] == SMALLPIECE || board[four[j]] == CASTLEPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUEBUCKETPIECE, i, four[j]), moveList);
            }
          }
        }
        if (i == 5) {
          for (int j = 0; j < 5; j++) {
            if (board[five[j]] == BLANKPIECE || board[five[j]] == SMALLPIECE || board[five[j]] == CASTLEPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUEBUCKETPIECE, i, five[j]), moveList);
            }
          }
        }
        if (i == 6) {
          for (int j = 0; j < 3; j++) {
            if (board[six[j]] == BLANKPIECE || board[six[j]] == SMALLPIECE || board[six[j]] == CASTLEPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUEBUCKETPIECE, i, six[j]), moveList);
            }
          }
        }
        if (i == 7) {
          for (int j = 0; j < 5; j++) {
            if (board[seven[j]] == BLANKPIECE || board[seven[j]] == SMALLPIECE || board[seven[j]] == CASTLEPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUEBUCKETPIECE, i, seven[j]), moveList);
            }
          }
        }
        if (i == 8) {
          for (int j = 0; j < 3; j++) {
            if (board[eight[j]] == BLANKPIECE || board[eight[j]] == SMALLPIECE || board[eight[j]] == CASTLEPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUEBUCKETPIECE, i, eight[j]), moveList);
            }
          }
        }
      }
      if (board[i] == BLUESMALLPIECE) {
        if (i == 0) {
          for (int j = 0; j < 3; j++) {
            if (board[zero[j]] == BLANKPIECE || board[zero[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUESMALLPIECE, i, zero[j]), moveList);
            }
          }
        }
        if (i == 1) {
          for (int j = 0; j < 5; j++) {
            if (board[one[j]] == BLANKPIECE || board[one[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUESMALLPIECE, i, one[j]), moveList);
            }
          }
        }
        if (i == 2) {
          for (int j = 0; j < 3; j++) {
            if (board[two[j]] == BLANKPIECE || board[two[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUESMALLPIECE, i, two[j]), moveList);
            }
          }
        }
        if (i == 3) {
          for (int j = 0; j < 5; j++) {
            if (board[three[j]] == BLANKPIECE || board[three[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUESMALLPIECE, i, three[j]), moveList);
            }
          }
        }
        if (i == 4) {
          for (int j = 0; j < 8; j++) {
            if (board[four[j]] == BLANKPIECE || board[four[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUESMALLPIECE, i, four[j]), moveList);
            }
          }
        }
        if (i == 5) {
          for (int j = 0; j < 5; j++) {
            if (board[five[j]] == BLANKPIECE || board[five[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUESMALLPIECE, i, five[j]), moveList);
            }
          }
        }
        if (i == 6) {
          for (int j = 0; j < 3; j++) {
            if (board[six[j]] == BLANKPIECE || board[six[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUESMALLPIECE, i, six[j]), moveList);
            }
          }
        }
        if (i == 7) {
          for (int j = 0; j < 5; j++) {
            if (board[seven[j]] == BLANKPIECE || board[seven[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUESMALLPIECE, i, seven[j]), moveList);
            }
          }
        }
        if (i == 8) {
          for (int j = 0; j < 3; j++) {
            if (board[eight[j]] == BLANKPIECE || board[eight[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUESMALLPIECE, i, eight[j]), moveList);
            }
          }
        }
      }
      if (board[i] == BLUECASTLEPIECE) {
        if (i == 0) {
          for (int j = 0; j < 3; j++) {
            if (board[zero[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUECASTLEPIECE, i, zero[j]), moveList);
            }
          }
        }
        if (i == 1) {
          for (int j = 0; j < 5; j++) {
            if (board[one[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUECASTLEPIECE, i, one[j]), moveList);
            }
          }
        }
        if (i == 2) {
          for (int j = 0; j < 3; j++) {
            if (board[two[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUECASTLEPIECE, i, two[j]), moveList);
            }
          }
        }
        if (i == 3) {
          for (int j = 0; j < 5; j++) {
            if (board[three[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUESMALLPIECE, i, three[j]), moveList);
            }
          }
        }
        if (i == 4) {
          for (int j = 0; j < 8; j++) {
            if (board[four[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUESMALLPIECE, i, four[j]), moveList);
            }
          }
        }
        if (i == 5) {
          for (int j = 0; j < 5; j++) {
            if (board[five[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUESMALLPIECE, i, five[j]), moveList);
            }
        }
        if (i == 6) {
          for (int j = 0; j < 3; j++) {
            if (board[six[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUESMALLPIECE, i, six[j]), moveList);
            }
          }
        }
        if (i == 7) {
          for (int j = 0; j < 5; j++) {
            if (board[seven[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUESMALLPIECE, i, seven[j]), moveList);
            }
          }
        }
        if (i == 8) {
          for (int j = 0; j < 3; j++) {
            if (board[eight[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(BLUESMALLPIECE, i, eight[j]), moveList);
            }
          }
        }
      }
    }


    if (turn == RED) {
      if (board[i] == REDBUCKETPIECE) {
        if (i == 0) {
          for (int j = 0; j < 3; j++) {
            if (board[zero[j]] == BLANKPIECE || board[zero[j]] == SMALLPIECE || board[zero[j]] == CASTLEPIECE) {
              moveList = CreateMovelistNode(hashMove(REDBUCKETPIECE, i, zero[j]), moveList);
            }
          }
        }
        if (i == 1) {
          for (int j = 0; j < 5; j++) {
            if (board[one[j]] == BLANKPIECE || board[one[j]] == SMALLPIECE || board[one[j]] == CASTLEPIECE) {
              moveList = CreateMovelistNode(hashMove(REDBUCKETPIECE, i, one[j]), moveList);
            }
          }
        }
        if (i == 2) {
          for (int j = 0; j < 3; j++) {
            if (board[two[j]] == BLANKPIECE || board[two[j]] == SMALLPIECE || board[two[j]] == CASTLEPIECE) {
              moveList = CreateMovelistNode(hashMove(REDBUCKETPIECE, i, two[j]), moveList);
            }
          }
        }
        if (i == 3) {
          for (int j = 0; j < 5; j++) {
            if (board[three[j]] == BLANKPIECE || board[three[j]] == SMALLPIECE || board[three[j]] == CASTLEPIECE) {
              moveList = CreateMovelistNode(hashMove(REDBUCKETPIECE, i, three[j]), moveList);
            }
          }
        }
        if (i == 4) {
          for (int j = 0; j < 8; j++) {
            if (board[four[j]] == BLANKPIECE || board[four[j]] == SMALLPIECE || board[four[j]] == CASTLEPIECE) {
              moveList = CreateMovelistNode(hashMove(REDBUCKETPIECE, i, four[j]), moveList);
            }
          }
        }
        if (i == 5) {
          for (int j = 0; j < 5; j++) {
            if (board[five[j]] == BLANKPIECE || board[five[j]] == SMALLPIECE || board[five[j]] == CASTLEPIECE) {
              moveList = CreateMovelistNode(hashMove(REDBUCKETPIECE, i, five[j]), moveList);
            }
          }
        }
        if (i == 6) {
          for (int j = 0; j < 3; j++) {
            if (board[six[j]] == BLANKPIECE || board[six[j]] == SMALLPIECE || board[six[j]] == CASTLEPIECE) {
              moveList = CreateMovelistNode(hashMove(REDBUCKETPIECE, i, six[j]), moveList);
            }
          }
        }
        if (i == 7) {
          for (int j = 0; j < 5; j++) {
            if (board[seven[j]] == BLANKPIECE || board[seven[j]] == SMALLPIECE || board[seven[j]] == CASTLEPIECE) {
              moveList = CreateMovelistNode(hashMove(REDBUCKETPIECE, i, seven[j]), moveList);
            }
          }
        }
        if (i == 8) {
          for (int j = 0; j < 3; j++) {
            if (board[eight[j]] == BLANKPIECE || board[eight[j]] == SMALLPIECE || board[eight[j]] == CASTLEPIECE) {
              moveList = CreateMovelistNode(hashMove(REDBUCKETPIECE, i, eight[j]), moveList);
            }
          }
        }
      }
      if (board[i] == REDSMALLPIECE) {
        if (i == 0) {
          for (int j = 0; j < 3; j++) {
            if (board[zero[j]] == BLANKPIECE || board[zero[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(REDSMALLPIECE, i, zero[j]), moveList);
            }
          }
        }
        if (i == 1) {
          for (int j = 0; j < 5; j++) {
            if (board[one[j]] == BLANKPIECE || board[one[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(REDSMALLPIECE, i, one[j]), moveList);
            }
          }
        }
        if (i == 2) {
          for (int j = 0; j < 3; j++) {
            if (board[two[j]] == BLANKPIECE || board[two[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(REDSMALLPIECE, i, two[j]), moveList);
            }
          }
        }
        if (i == 3) {
          for (int j = 0; j < 5; j++) {
            if (board[three[j]] == BLANKPIECE || board[three[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(REDSMALLPIECE, i, three[j]), moveList);
            }
          }
        }
        if (i == 4) {
          for (int j = 0; j < 8; j++) {
            if (board[four[j]] == BLANKPIECE || board[four[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(REDSMALLPIECE, i, four[j]), moveList);
            }
          }
        }
        if (i == 5) {
          for (int j = 0; j < 5; j++) {
            if (board[five[j]] == BLANKPIECE || board[five[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(REDSMALLPIECE, i, five[j]), moveList);
            }
          }
        }
        if (i == 6) {
          for (int j = 0; j < 3; j++) {
            if (board[six[j]] == BLANKPIECE || board[six[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(REDSMALLPIECE, i, six[j]), moveList);
            }
          }
        }
        if (i == 7) {
          for (int j = 0; j < 5; j++) {
            if (board[seven[j]] == BLANKPIECE || board[seven[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(REDSMALLPIECE, i, seven[j]), moveList);
            }
          }
        }
        if (i == 8) {
          for (int j = 0; j < 3; j++) {
            if (board[eight[j]] == BLANKPIECE || board[eight[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(REDSMALLPIECE, i, eight[j]), moveList);
            }
          }
        }
      }
      if (board[i] == REDCASTLEPIECE) {
        if (i == 0) {
          for (int j = 0; j < 3; j++) {
            if (board[zero[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(REDCASTLEPIECE, i, zero[j]), moveList);
            }
          }
        }
        if (i == 1) {
          for (int j = 0; j < 5; j++) {
            if (board[one[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(REDCASTLEPIECE, i, one[j]), moveList);
            }
          }
        }
        if (i == 2) {
          for (int j = 0; j < 3; j++) {
            if (board[two[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(REDCASTLEPIECE, i, two[j]), moveList);
            }
          }
        }
        if (i == 3) {
          for (int j = 0; j < 5; j++) {
            if (board[three[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(REDCASTLEPIECE, i, three[j]), moveList);
            }
          }
        }
        if (i == 4) {
          for (int j = 0; j < 8; j++) {
            if (board[four[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(REDCASTLEPIECE, i, four[j]), moveList);
            }
          }
        }
        if (i == 5) {
          for (int j = 0; j < 5; j++) {
            if (board[five[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(REDCASTLEPIECE, i, five[j]), moveList);
            }
        }
        if (i == 6) {
          for (int j = 0; j < 3; j++) {
            if (board[six[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(REDCASTLEPIECE, i, six[j]), moveList);
            }
          }
        }
        if (i == 7) {
          for (int j = 0; j < 5; j++) {
            if (board[seven[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(REDCASTLEPIECE, i, seven[j]), moveList);
            }
          }
        }
        if (i == 8) {
          for (int j = 0; j < 3; j++) {
            if (board[eight[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(REDCASTLEPIECE, i, eight[j]), moveList);
            }
          }
        }
      }
    }


    if (board[i] == SMALLPIECE) {
      if (i == 0) {
          for (int j = 0; j < 3; j++) {
            if (board[zero[j]] == BLANKPIECE || board[zero[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(SMALLPIECE, i, zero[j]), moveList);
            }
          }
        }
        if (i == 1) {
          for (int j = 0; j < 5; j++) {
            if (board[one[j]] == BLANKPIECE || board[one[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(SMALLPIECE, i, one[j]), moveList);
            }
          }
        }
        if (i == 2) {
          for (int j = 0; j < 3; j++) {
            if (board[two[j]] == BLANKPIECE || board[two[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(SMALLPIECE, i, two[j]), moveList);
            }
          }
        }
        if (i == 3) {
          for (int j = 0; j < 5; j++) {
            if (board[three[j]] == BLANKPIECE || board[three[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(SMALLPIECE, i, three[j]), moveList);
            }
          }
        }
        if (i == 4) {
          for (int j = 0; j < 8; j++) {
            if (board[four[j]] == BLANKPIECE || board[four[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(SMALLPIECE, i, four[j]), moveList);
            }
          }
        }
        if (i == 5) {
          for (int j = 0; j < 5; j++) {
            if (board[five[j]] == BLANKPIECE || board[five[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(SMALLPIECE, i, five[j]), moveList);
            }
        }
        if (i == 6) {
          for (int j = 0; j < 3; j++) {
            if (board[six[j]] == BLANKPIECE || board[six[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(SMALLPIECE, i, six[j]), moveList);
            }
          }
        }
        if (i == 7) {
          for (int j = 0; j < 5; j++) {
            if (board[seven[j]] == BLANKPIECE || board[seven[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(SMALLPIECE, i, seven[j]), moveList);
            }
          }
        }
        if (i == 8) {
          for (int j = 0; j < 3; j++) {
            if (board[eight[j]] == BLANKPIECE || board[eight[j]] == LARGEPIECE) {
              moveList = CreateMovelistNode(hashMove(SMALLPIECE, i, eight[j]), moveList);
            }
          }
        }
    }

    if (board[i] == LARGEPIECE) {
      if (i == 0) {
          for (int j = 0; j < 3; j++) {
            if (board[zero[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(LARGEPIECE, i, zero[j]), moveList);
            }
          }
        }
        if (i == 1) {
          for (int j = 0; j < 5; j++) {
            if (board[one[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(LARGEPIECE, i, one[j]), moveList);
            }
          }
        }
        if (i == 2) {
          for (int j = 0; j < 3; j++) {
            if (board[two[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(LARGEPIECE, i, two[j]), moveList);
            }
          }
        }
        if (i == 3) {
          for (int j = 0; j < 5; j++) {
            if (board[three[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(LARGEPIECE, i, three[j]), moveList);
            }
          }
        }
        if (i == 4) {
          for (int j = 0; j < 8; j++) {
            if (board[four[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(LARGEPIECE, i, four[j]), moveList);
            }
          }
        }
        if (i == 5) {
          for (int j = 0; j < 5; j++) {
            if (board[five[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(LARGEPIECE, i, five[j]), moveList);
            }
        }
        if (i == 6) {
          for (int j = 0; j < 3; j++) {
            if (board[six[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(LARGEPIECE, i, six[j]), moveList);
            }
          }
        }
        if (i == 7) {
          for (int j = 0; j < 5; j++) {
            if (board[seven[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(LARGEPIECE, i, seven[j]), moveList);
            }
          }
        }
        if (i == 8) {
          for (int j = 0; j < 3; j++) {
            if (board[eight[j]] == BLANKPIECE) {
              moveList = CreateMovelistNode(hashMove(LARGEPIECE, i, eight[j]), moveList);
            }
          }
        }
    }
  return moveList;
}



  /* How to add something to moveList
  int moveHash = 70;
  moveList = CreateMovelistNode(moveHash, moveList);
  */

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
  return position;
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
      ((input[0] == 'B' || input[0] == 'R' || input[0] == 'S' || input[0] == 'L') && input[1] >= 48 && input[1] <= 56 && input[2] == '\0'); // Placement
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
  return 0;
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