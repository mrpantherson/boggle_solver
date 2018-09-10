#ifndef _HTABLE_H_
#define _HTABLE_H_

// -------------------------------------------------------------------

#include <list>

// -------------------------------------------------------------------

#define HTABLE_SIZE  500
#define WORD_LENGTH   50

#define BAD_WORD       0
#define KEEP_GOING     1
#define WORD_FOUND     2

// -------------------------------------------------------------------

struct CEntry
{
	char mszWord[WORD_LENGTH];
};

// -------------------------------------------------------------------

class CHTable
{

private:

	std::list<CEntry*> mTable[HTABLE_SIZE];
	std::list<CEntry*> ::iterator mIter;

public:

	CHTable(void);
	~CHTable(void);

	void addEntry(char* pszWord);

	int checkEntry(char* pszWord);

	void tableReport(void);
};

// -------------------------------------------------------------------

extern bool strCheck(char* pszFirst, char* pszSecond);

extern int GetHashValue(char* pszEntry);

// -------------------------------------------------------------------

#endif
