#include <stdio.h>
#include <list>
#include "htable.h"

// ----------------------------------------------------------------------------

#define BOARD_WIDTH    4
#define BOARD_HEIGHT   4

#define VERSION_MESSAGE "Boogle Generator ver 0.9\n[tOA]C. Brandon Patterson\n"

// i use the x and y position in the board to create a unique ID for each word
#define ENCODE(x,y)  ((x << 8) | y)
#define GIMME_X(a)   (a>>8)
#define GIMME_Y(a)   (a&0x0FF)

#define HUGE_DICT    0
#define SMALL_DICT   1

// ----------------------------------------------------------------------------

struct CWord
{
	// the word
	char mszWord[128];
	
	// list to keep track of letters that we have already used within the board
	std::list<int> mID;
};

// ----------------------------------------------------------------------------

// the boogle board
char gcBoard[BOARD_HEIGHT][BOARD_WIDTH];

// the dictionary hash table
CHTable gTable;

// temporary list to keep track of words we are currently considering
std::list<CWord> gWordQue;

// words that have been found
std::list<CWord> gFinalList;

// ----------------------------------------------------------------------------

void loadDictionary(int aiSize);
void preFinalizeWord(CWord& entry);
void fillBoard(void);
void printBoard(void);

bool len_compare(const CWord& first, const CWord& second);

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Function name	: main
// Description	    : 
// Return type		: int 
// Argument         : void
// ----------------------------------------------------------------------------
int main(void)
{
	printf(VERSION_MESSAGE);
	
	loadDictionary(HUGE_DICT);

	fillBoard();

	printBoard();

	// finds words
	for (int i = 0; i < BOARD_HEIGHT; i++)
	{
		for (int j = 0; j < BOARD_WIDTH; j++)
		{
			std::list<CWord>::iterator iter;
			CWord tempWord;
			int tableReturn = 0;

			// grab first letter
			tempWord.mID.clear();
			tempWord.mID.push_front(ENCODE(j, i));
			tempWord.mszWord[0] = gcBoard[i][j];
			tempWord.mszWord[1] = '\0';

			gWordQue.push_front(tempWord);

			while(!gWordQue.empty())
			{
				iter = gWordQue.begin();

				// check to see if current word is in dictionary
				tableReturn = gTable.checkEntry((*iter).mszWord);
				tempWord = (*iter);

				// get rid of word
				gWordQue.pop_front();

				// if the word is in no way good, skip to next word
				if (tableReturn == BAD_WORD)
				{
					continue;
				}
				// if the word was found, prepare to add it to final list
				else
				if (tableReturn == WORD_FOUND)
				{
					preFinalizeWord(tempWord);
				}

				// we checked our current word, we now need to add surrounding 8 letters
				// to our word to consider them for validity
				
				// prepare our lcv for y iteration
				int y = GIMME_Y(tempWord.mID.front());
				int yLCV = (y > 0) ? y - 1 : y;
				int yMax = (y == (BOARD_HEIGHT-1)) ? y : y + 1;

				// add surrounding letters to word
				for (yLCV; yLCV <= yMax; yLCV++)
				{
					// prepare our lcv for x iteration
					int x = GIMME_X(tempWord.mID.front());
					int xLCV = (x > 0) ? x - 1 : x;
					int xMax = (x == (BOARD_WIDTH-1)) ? x : x + 1;

					for (xLCV; xLCV <= xMax; xLCV++)
					{
						// allocate and prepare new words for list
						int len = strlen(tempWord.mszWord);
						bool next = false;
						CWord inntertempWord = tempWord;
						
						// add letter to the temp word, add its code to our list
						// of ids for letters we have already considered
						inntertempWord.mszWord[len] = gcBoard[yLCV][xLCV];
						inntertempWord.mszWord[len+1] = '\0';
						inntertempWord.mID.push_front(ENCODE(xLCV, yLCV));

						std::list<int>::iterator entryIter = inntertempWord.mID.end();

						//ignore letter we just added, since thats what were checking
						entryIter--;

						// checks hash code of letters we have already considered
						// flags 'next' if letter has already been used, hm, might
						// have a bug where it doesnt check the first id in the list...
						do
						{							
							if ((*entryIter) == ENCODE(xLCV, yLCV))
							{
								next = true;
								break;
							}

							entryIter--;				

						}while(entryIter != inntertempWord.mID.begin());

						// if the word is bad, were not going to consider it either
						if (gTable.checkEntry(inntertempWord.mszWord) == BAD_WORD)
							next = true;

						// this letter need not be considered
						if (next)
							continue;
						
						// else must be a good word, throw it onto our list for consideration
						gWordQue.push_front(inntertempWord);
					}
				}				

			}

			// empty word list for next iteration
			gWordQue.clear();
		}
	}

	// sort final word list
	printf("\nPrinting sorted list...\n\n");
	gFinalList.sort(len_compare);

	std::list<CWord>::iterator iter = gFinalList.begin();
	while(iter != gFinalList.end())
	{
		if (strlen((*iter).mszWord) > 3)
		{
			printf("%s\n", (*iter).mszWord);
		}

		iter++;
	}

	printf("\n\n");

	return 1;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Function name	: loadDictionary
// Description	    : loads dictionary into a hash table
// Return type		: void 
// Argument         : int aiSize : size dictionary to load
// ----------------------------------------------------------------------------
void loadDictionary(int aiSize)
{
	char buffer[128];

	FILE* file = NULL;
	
	if (aiSize == HUGE_DICT)
	{
		file = fopen("huge_dict.txt", "r");
	}
	else
	if (aiSize == SMALL_DICT)
	{
		file = fopen("small_dict.txt", "r");
	}
	else
	{
		return;
	}

	printf("\nLoading Dictionary....\n");

	while(!feof(file))
	{
		fscanf(file, "%s", buffer);

		gTable.addEntry(buffer);
	}
	
	printf("Dictionary Loaded....\n\n");

	fclose(file);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Function name	: preFinalizeWord
// Description	    : checks to see if the found word is in dictionary already
//                  : if not, adds it to the final word list
// Return type		: void 
// Argument         : CWord& entry
// ----------------------------------------------------------------------------
void preFinalizeWord(CWord& entry)
{
	std::list<CWord>::iterator iter;
	bool newWord = true;

	iter = gFinalList.begin();

	while(iter != gFinalList.end())
	{
		if (strcmp((*iter).mszWord, entry.mszWord) == 0)
		{
			newWord = false;
			break;
		}

		iter++;
	}

	if (newWord)
	{
		//printf("Word found!  %s\n", entry.mszWord);
		gFinalList.push_back(entry);
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Function name	: fillBoard
// Description	    : queries user for boogle board
// Return type		: void 
// Argument         : void
// ----------------------------------------------------------------------------
void fillBoard(void)
{
	printf("Input Board -\n\n");

	char temp = NULL;

	// fill out boogle board
	for (int i = 0; i < BOARD_HEIGHT; i++)
	{
		for (int j = 0; j < BOARD_WIDTH; j++)
		{
			printf("%d - %d = ", i, j);
			scanf("%c", &temp);
			
			gcBoard[i][j] = temp;
			
			// newline
			scanf("%c", &temp);
		}
	}

	printf("\n");
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Function name	: printBoard
// Description	    : prints the completed boogle board
// Return type		: void 
// Argument         : void
// ----------------------------------------------------------------------------
void printBoard(void)
{
	printf("\nPrinting board\n\n");

	for(int i = 0; i < BOARD_HEIGHT; i++)
	{
		for(int j = 0; j < BOARD_WIDTH; j++)
		{
			printf("%c|", gcBoard[i][j]);
		}

		printf("\n");

		for(int k = 0; k < BOARD_WIDTH; k++)
		{
			printf("--");
		}

		printf("\n");
	}

	printf("\n\n");
}
// ----------------------------------------------------------------------------

bool len_compare(const CWord& first, const CWord& second)
{
	if (strlen(first.mszWord) > strlen(second.mszWord))
		return true;
	else
	    return false;
}
