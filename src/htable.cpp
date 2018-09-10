#include "htable.h"

// -------------------------------------------------------------------
// Function name	: CHTable::CHTable
// Description	    : 
// Return type		: 
// Argument         : void
// -------------------------------------------------------------------
CHTable::CHTable(void)
{
	for (int i = 0; i < HTABLE_SIZE; i++)
	{
		mTable[i].clear();
	}
}
// -------------------------------------------------------------------

// -------------------------------------------------------------------
// Function name	: CHTable::~CHTable
// Description	    : 
// Return type		: 
// Argument         : void
// -------------------------------------------------------------------
CHTable::~CHTable(void)
{
	for (int i = 0; i < HTABLE_SIZE; i++)
	{
		mIter = mTable[i].begin();

		while(mIter != mTable[i].end())
		{
			delete (*mIter);
			(*mIter) = NULL;
			mIter++;
		}

		mTable[i].clear();
	}
}
// -------------------------------------------------------------------

// -------------------------------------------------------------------
// Function name	: CHTable::addEntry
// Description	    : adds a new word the the hashed dictionary
// Return type		: void 
// Argument         : char* pszWord
// -------------------------------------------------------------------
void CHTable::addEntry(char* pszWord)
{
	int i = GetHashValue(pszWord) % HTABLE_SIZE;

	CEntry* temp = new CEntry;

	strcpy(temp->mszWord, pszWord);

	mTable[i].push_back(temp);
}
// -------------------------------------------------------------------

// -------------------------------------------------------------------
// Function name	: CHTable::checkEntry
// Description	    : checks to see if the passed string (or subset)
//                  : is in the dictionary
// Return type		: int 
// Argument         : char* pszWord
// -------------------------------------------------------------------
int CHTable::checkEntry(char* pszWord)
{
	int i = GetHashValue(pszWord) % HTABLE_SIZE;

	mIter = mTable[i].begin();

	while (mIter != mTable[i].end())
	{
		// if the word was found
		if (strcmp(pszWord, (*mIter)->mszWord) == 0)
			return WORD_FOUND;

		// if a partial chunk was found
		if (strCheck((*mIter)->mszWord, pszWord))
			return KEEP_GOING;

		mIter++;
	}

	// nothing was found
	return BAD_WORD;
}
// -------------------------------------------------------------------

// -------------------------------------------------------------
// Function name	: strCheck
// Description	    : checks to see if the second parameter is
//                  : a subset of the first
// Return type		: bool 
// Argument         : char* pszFirst  : dictionary word
// Argument         : char* pszSecond : other word
// -------------------------------------------------------------
bool strCheck(char* pszFirst, char* pszSecond)
{
	// bad pointer
	if (!pszFirst || !pszSecond)
		return false;

	// if the subset is longer than the entry
	if (strlen(pszSecond) > strlen(pszFirst))
		return false;

	for (int i = 0; i < strlen(pszSecond); i++)
	{
		if (pszFirst[i] != pszSecond[i])
			return false;
	}

	return true;
}
// -------------------------------------------------------------

// -------------------------------------------------------------------
// Function name	: GetHashValue
// Description	    : good hasing function for strings (found on net)
// Return type		: int 
// Argument         : char* pszEntry
// -------------------------------------------------------------------
int GetHashValue(char* pszEntry)
{
	int l = strlen(pszEntry);

	if(l >= 4)
	{
		return pszEntry[0] + (pszEntry[1] << 12) + (pszEntry[2] << 6) + (pszEntry[3] << 18) + (pszEntry[l- 1] << 3) + l;
	}
	else
	if(l >= 1)
	{
		return pszEntry[0] + (pszEntry[l - 1] << 4) + l;
	}
	else
	{
		return 0;
	}
}
// -------------------------------------------------------------------

// -------------------------------------------------------------------
// Function name	: CHTable::tableReport
// Description	    : reports the node count in each bucket
// Return type		: void 
// Argument         : void
// -------------------------------------------------------------------
void CHTable::tableReport(void)
{
	for (int i = 0; i < HTABLE_SIZE; i++)
	{
		printf("bucket %d: %d\n", i, mTable[i].size());
	}
}
// -------------------------------------------------------------------
