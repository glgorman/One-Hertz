
#include "stdafx.h"
#include <vector>

using namespace std;

template <class nodeType, class leafType> class s_node;
template <size_t sz> class pstring;

class text_object;

class atari
{
public:
	void train_ann();
	int main ();
};

struct subst
{
	char *wordin;
	char *wordout;
	subst();
	subst (char *str1, char *str2)
	{
		wordin = str1;
		wordout = str2;
	}
};

struct key_info
{
	char *key_str;
	int	  responses;
	int	  first_reply;
	int	  current_reply;
	int	  last_reply;
	key_info(char *str, int N)
	{
		key_str = str;
		responses = N;
	}
};

namespace narrative
{
	size_t get_response_count();
	char *get_response(int i);
	subst *get_conjugates();
	key_info *get_keywords();
}

class solar_elements;

class ELIZA
{
public:
	text_object m_process;

protected:
	solar_elements *m_s;
	int m_uid;
	int n1, defaultKey, m_key;
	
	text_object m_textIn;
	text_object m_textOut;
	key_info *m_keywords;

	vector<subst*> m_conjugates;
	vector<key_info*> m_keysets;
	vector<char*>	m_replies;

public:
	ELIZA();
	~ELIZA();
	static UINT thread_entry (LPVOID ptr);
	void ELIZA::set_io (LPVOID ptr);
	void ELIZA::initialize ();

	bTreeType<pstring<256> >* make_tree();
	void pre_process(const vector<subst*>&);
	void ELIZA::pre_process (const subst *table);
	int do_main(int argc, const char** args);
	text_object generate_reply();

protected:
	static void memcheck (char *text);
	LPVOID allocate_p(size_t size);
	node<char*> *ELIZA::find_keyword ();	
	text_object ELIZA::response ();

	bool bind_conjugates (subst *);
	bool bind_keysets (key_info*);  
	bool bind_responses (size_t sz);

	void debug_keyset();
	node_list <char*> find_keywords();
	bool is_blank ();
	bool is_repeat ();
	bool detect_nmea ();
	void parse_nmea0 ();
	void report_keywords(const node_list<char*>&) const;
	vector<int> get_markers(const node_list <char*>&) const;
	char *select_template (int key);
};

