
#include "stdafx.h"
#include "afxmt.h"
#include <vector>
#include "symbol_table.h"
//#include "../Frame Lisp/btreetype.h"
#include "node_list.h"

using namespace std;
class key_list;

class text_object
{
	CCriticalSection critical;

protected:
	bool	m_bEnd;
	vector<node_list<char*> > m_content;

public:
	UINT	m_code_page;
	node_list<char*> m_nList;
	node<char*> *m_nPos;
	s_node<char*, language> m_sList;

public:
	text_object ();
	text_object (char *m_pText);
	text_object (text_object &copy);
	text_object (node_list<char *> *copyFrom);
	text_object (bTreeType<char*> **source);
	~text_object ();

	text_object &operator = (text_object &arg);
	void operator = (char *arg);
	void operator >> (char *(&));

	inline node<char*> *begin() const
		{ return m_nList.m_nBegin; }
	inline node<char*> *end() const
		{ return m_nList.m_nEnd; }
	inline node<char*> *pos() const
		{ return m_nList.m_nPos; }
	
	void append (char *aWord);
	void append (s_node<char*, enum language> *theWord);
	void append (text_object &source);
	void get (char *(&));
	char *get (language &theType);
	void peek(char* (&));
	text_object split();
	
	inline bool eof() { return m_bEnd; }
	node<char*>	*begin();
	node<char*> *findPenultimate (char *(&));
	s_node<char*, enum language> findType (language whatType);

	void markLocation ();
	void rewind ();
	void rewind (s_node<char*, enum language> *location);

	text_object getSentence ();
	bool find_keyword (key_list &keyWords);
	void count_words (int &count);

	void transferFrom (text_object &m_pText);
	void transferTo (text_object &m_pText);	
	void truncate ();
	
private:
	node_list<char *> *duplicate ();
	void tokenize (char *arg);
	void detatch ();
	void putTempWord (char *theWord);
	s_node<char*, enum language> *removeWord (s_node<char*, enum language> *killer);
	s_node<char*, enum language> *search (char *keyword);
};
