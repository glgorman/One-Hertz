
#include "stdafx.h"
#include <vector>
#include "intrinsics.h"
#include "btreetype.h"
#include "text_object.h"
#include "drawstuff.h"
#include "ELIZA.h"

typedef char* ascii;
typedef node<char*>* context_ptr;
typedef node_list<char*> text_item;
#define nullptr NULL

#define debug_term output

ELIZA::ELIZA()
{
	int j;
	size_t total_keys = 0;
	size_t total_replies = 0;
	size_t responses;
	j = 0;
	m_keywords = narrative::get_keywords();
	subst *conjugates = narrative::get_conjugates();
	bind_conjugates(conjugates);
	m_keysets.reserve(64);
	m_keysets.resize(0);

	while (m_keywords[total_keys].key_str != NULL)
	{
		responses = m_keywords[total_keys].responses;
		if (responses != -1)
		{
			j = (int)total_replies;
			total_replies += responses;
		}
		m_keywords[total_keys].first_reply = j;
		m_keywords[total_keys].current_reply = j;
		m_keywords[total_keys].last_reply = (int)total_replies - 1;
		bind_keysets (&m_keywords[total_keys]);
		total_keys++;
	}
	n1 = (int)total_keys-1;
	bind_responses (total_replies);
	defaultKey = (int)total_replies-1;
//	bTreeType<pstring<256> >* ptr = make_tree(replies);

	m_s = new solar_elements;
}

#if 0
void ELIZA::initialize ()
{
	int i,j,k;
	size_t total_keys = n1+1;
	for (i=0;i<(int)total_keys;i++)
	{
		writeln(debug_term,"# ",keywords[i].key_str);
		j = keywords[i].first_reply;
		k = keywords[i].last_reply;
		for (;j<=k;j++)
		{
			char *str = narrative::get_response(j);
			writeln(debug_term,"(",j,") \"",str,"\"");
		}
	}
}
#endif

ELIZA::~ELIZA()
{

}

void ELIZA::set_io (LPVOID ptr)
{
	m_uid = (int)ptr;
}

UINT ELIZA::thread_entry (LPVOID ptr1)
{
	bTreeType<pstring<256> > *ptr2 = NULL;
	LPVOID io;
	SYSCOMM::LAUNCH_CONSOLE("Eliza");
	writeln(output,"Console Initialized");
	if (ptr1==NULL)
	{	
		io = (LPVOID) output;
	}
	else io = ptr1;
	ELIZA instance;
	instance.set_io ((LPVOID)io);
	ptr2 = instance.make_tree ();
//	instance.initialize ();
	instance.memcheck ("About to Enter Main Loop");
	instance.do_main (0,NULL);
	return 0;
}

int ELIZA::do_main (int argc, const char **args)
{	
	char *buffer1 = new char [256];
	char *buffer2 = NULL;
	int num = 0, result = 0;
	
	writeln(m_uid,"Hello.  My names is Eliza.  How can I help you today?");
	while (true)
	{
		strcpy_s (buffer1,256,"");		
		write(debug_term,">");
		READLN(m_uid,buffer1);
		writeln(debug_term);
		if (m_uid!=output)
		{
			write(m_uid,">");
			writeln(m_uid,buffer1);
		}
		m_textIn = buffer1;
		memcheck ("Was ready to Call Eliza");
//		m_textOut = response ();
		m_textOut = generate_reply ();
		memcheck ("Just Returned from Eliza");
		m_textOut >> buffer2;
		writeln(m_uid,buffer2);
		delete buffer2;
	}
	return result;
}

void ELIZA::memcheck (char *text)
{
	char message [256];
//	int result;
	int memStat =  _CrtCheckMemory ();
	if (memStat==0) {
		strcpy_s (message,256,"Warning: Possible Heap Corruption!\n");
		strcat_s (message,256,text);
//		result = AfxMessageBox (,MB_OKCANCEL);
//		if (result==IDCANCEL)
//			exit (-1);
	}
}

LPVOID ELIZA::allocate_p(size_t size)
{
	BOOL b_valid;
	LPVOID	p_addr;
	size_t allocation = size;
	p_addr = (LPVOID)VirtualAlloc(NULL, allocation,
		MEM_COMMIT, PAGE_READWRITE | PAGE_NOCACHE);
	ASSERT(p_addr != NULL);
	b_valid = (BOOL)VirtualLock(p_addr, allocation);
	return p_addr;
}

bool ELIZA::detect_nmea ()
{
	char *str;
//	check for "$" as the first token
	m_process.rewind ();
	if (m_process.eof())
		return false;

	m_process.get (str);
	int test;
	test = btext::compare (str,"$");
	if (test==0)
		writeln (debug_term,"$ Token Found:  Possible NMEA messagge!");
	else
		return false;

	if (m_process.eof())
		return false;

	m_process.get (str);
	test = btext::compare (str,"GPGGA");
	if (test==0)
		writeln (debug_term,"GPGGA Token indicating NMEA messagge!");
	else
		return true;

	if (m_process.eof())
		return false;

	m_process.get (str);
	test = btext::compare (str,",");
	if (test!=0) {
		writeln (debug_term,"Invalid Sequence, \",\" expected");
		return false;
	}
	if (m_process.eof())
		return false;

	return true;
}

void ELIZA::parse_nmea0 ()
{
	char *str;
	int test,test1,test2;
	m_process.get (str);
	int t = atoi (str);
	writeln (debug_term,"Parsed GMT time as int = ",t);

	m_s->set_time (str);
	
//	Get GPS Time

	m_process.get (str);
	test = btext::compare (str,".");
	if (test!=0) {
		writeln (debug_term,"Invalid Sequence, \".\" expected");
		return;
	}
	m_process.get (str);
	test = btext::compare (str,"000");
	if (test!=0) {
		writeln (debug_term,"Invalid Sequence, \"000\" expected");
		return;
	}
	m_process.get (str);
	test = btext::compare (str,",");
	if (test!=0) {
		writeln (debug_term,"Invalid Sequence, \",\" expected");
		return;
	}

//	Get Latitude as degrees and minutes
//	and convert to double precision.

	m_process.get (str);
	int lat1 = atoi (str);
	if (m_process.eof())
		return;

	m_process.get (str);
	test = btext::compare (str,".");
	if (test!=0) {
		writeln (debug_term,"Invalid Sequence, \".\" expected");
		return;
	}
	if (m_process.eof())
		return;
	
	m_process.get (str);
	int lat2 = atoi (str);
	size_t sz = strlen (str);

	m_process.get (str);
	test = btext::compare (str,",");
	if (test!=0) {
		writeln (debug_term,"Invalid Sequence, \",\" expected");
		return;
	}

	m_process.get (str);
	test1 = btext::compare (str,"N");
	test2 = btext::compare (str,"S");
	if ((test1!=0)&&(test2!=0)) {
		writeln (debug_term,"Invalid Sequence, \"N\" or \"S\" expected");
		return;
	}

	m_process.get (str);
	test = btext::compare (str,",");
	if (test!=0) {
		writeln (debug_term,"Invalid Sequence, \",\" expected");
		return;
	}
	int lat3 = lat1/100;
	int lat4 = lat1%100;
	double latitude = lat3+(lat4+lat2*0.0001)*(1.0/60.0);

//	Now get longitude as degrees and minutes
//	and convert to double precision.

	m_process.get (str);
	int long1 = atoi (str);
	if (m_process.eof())
		return;

	m_process.get (str);
	test = btext::compare (str,".");
	if (test!=0) {
		writeln (debug_term,"Invalid Sequence, \".\" expected");
		return;
	}
	if (m_process.eof())
		return;

	m_process.get (str);
	int long2 = atoi (str);
	size_t sz1 = strlen (str);

	m_process.get (str);
	test = btext::compare (str,",");
	if (test!=0) {
		writeln (debug_term,"Invalid Sequence, \",\" expected");
		return;
	}
	
	int long3 = long1/100;
	int long4 = long1%100;
	double longitude = long3+(long4+long2*0.0001)*(1.0/60.0);	
	writeln (debug_term,"GPS Longitude = ",longitude," Latitude = ",latitude);
	
	m_s->set_latitude (latitude);
	m_s->set_longitude (longitude);

	m_s->compute_local_aspect ();
	char test_str [256];
	int M,H,S;
	H = m_s->m_hour;
	M = m_s->m_minute;
	S = m_s->m_second;
	double az, el;
	az = m_s->m_az;
	el = m_s->m_el;
	sprintf_s (test_str,256,"LOCAL time = %02d:%02d:%02d  AZIMUTH = %f, EL = %f",H,M,S,az,el);
	writeln (debug_term,test_str);
}

bool ELIZA::bind_conjugates(subst* defines)
{
	size_t sz = 0;
	m_conjugates.resize(0);
	while (defines[sz].wordin != nullptr)
	{
		m_conjugates.resize(sz+1);
		m_conjugates[sz] = &(defines[sz]);
		sz++;
	}
	return true;
}

void ELIZA::debug_keyset()
{
	int i, j, k;
	size_t total_keys = m_keysets.size();
	for (i = 0; i < (int)total_keys; i++)
	{
		writeln(output, "# ", m_keysets[i]->key_str);
		j = m_keysets[i]->first_reply;
		k = m_keysets[i]->last_reply;
		for (; j <= k; j++)
		{
			writeln(output, "(", j, ") \"", m_replies[j], "\"");
		}
	}
}

bool ELIZA::bind_keysets(key_info* ptr)
{
	size_t sz;
	sz = m_keysets.size();
	m_keysets.resize(sz + 1);
	m_keysets[sz] = ptr;
	return true;
}

bool ELIZA::bind_responses (size_t sz)
{
	int i;
	m_replies.reserve(sz);
	m_replies.resize(sz);
	for (i=0;i<(int)sz;i++)
	{
		m_replies[i] = narrative::get_response(i);
	}
	return true;
}

bTreeType<pstring<256> >* ELIZA::make_tree()
{
	char *str;
	size_t sz1 = sizeof(bTreeType<pstring<256> >);
	//	69362 bytes = p-system SANDBOX
	size_t sz2 = sz1 * 256;
	bTreeType<pstring<256> >* ptr = (bTreeType<pstring<256> >*) allocate_p(sz2);
	void* ptr2;
	bTreeType <pstring<256> >* bTree[256];
	int i;
	for (i = 0; i < 256; i++)
	{
		//	If the pointer arithmetic works the way it is supposed
		//	to, we should get an array which is 69632 bytes 
		//	which contains 256 binary tree nodes - each of which
		//	directly encapsulates an array of 256 bytes of char  
		
		ptr2 = (void*)(ptr + i);
		str = narrative::get_response (i);
		bTree[i] = new (ptr2) bTreeType<pstring<256> >;
		if (i < 114)
			bTree[i]->m_pData = str;
	}
	return ptr;
}

void ELIZA::pre_process (const subst *table)
{
	int word;
	bool endofline = false;
	char *wordIn, *wordOut, *str;
	node<char*> *marker;
	m_process.rewind();
	if (m_process.m_nPos==NULL)
		return;
	
	const subst *(&defines) = table;
	while (endofline==false)
	{
		word = 0;
		marker = m_process.m_nPos;
		m_process.get (str);
		endofline = m_process.eof();
		if (str!=NULL)
		for (word=0;;word++) 
		{
			wordIn = (defines[word]).wordin;
			wordOut = (defines[word]).wordout;
			if (wordIn==NULL)
				break; 
			if (btext::compare (wordIn,str)==0) {
				marker->m_pData = wordOut;
				break; }
		}
	}
}

void ELIZA::pre_process (const vector<subst*> &defines)
{
	int word;
	bool eof = false;
	char *wordIn, *wordOut, *str;
	node<char*> *marker;
	m_process.rewind();
	if (m_process.m_nPos==NULL)
		return;
	
//	subst* (defines); = table[word];
	size_t sz = defines.size();
	while (eof==false)
	{
		word = 0;
		marker = m_process.m_nPos;
		m_process.get (str);
		eof = m_process.eof();
//		eof = m_process.m_bEnd;
		if (str!=NULL)
		for (word=0;word<sz;word++) 
		{
			wordIn = (defines[word])->wordin;
			wordOut = (defines[word])->wordout;
			if (btext::compare (wordIn,str)==0) {
				marker->m_pData = wordOut;
				break; }
		}
	}
}

node_list<char*> ELIZA::find_keywords ()
{
	int index, key, test;
	char *str, *keyword;
	size_t sz = m_keysets.size();
	node<char*>* position;
	node<char*>  *found;
	node_list<char*> keys_found;

	key = -1;
	m_process.rewind ();
	while (m_process.eof() == false)
	{
		position = m_process.m_nPos;
		m_process.peek (str);
		for (index=0;index<sz;index++)
		{
			keyword = m_keysets[index]->key_str;
			test = btext::compare (keyword,str);
			if (test != 0)
				continue;
			found = new node<char*>(str);
			keys_found.concat(found);
			if (key == -1)
				key = index;
		}
		m_process.get (str);
	}
	m_key = key;
	return keys_found;
}

void ELIZA::report_keywords(const node_list<char*>& keys_found) const
{
	const node<char*>* key = keys_found.begin();
	if (key!=nullptr)
	{
		write(output, "Keywords found: ");
		keys_found.print_debug();
	}
	else
		writeln (output, "No Keywords found.");
}

char *ELIZA::select_template (int key=-1)
{
	char *result;
	int index;
	if (key < 0)
		key = (int)m_keysets.size() - 1;

	index = m_keysets[key]->current_reply;
	m_keysets[key]->current_reply++;
	if (m_keysets[key]->current_reply > m_keysets[key]->last_reply)
		m_keysets[key]->current_reply = m_keysets[key]->first_reply;

	writeln(output, "Building Response using template: ", index, " ", m_replies[index]);
	result = m_replies[index];
	return result;
}

#if 0
vector<int> get_markers(const node_list <char*>&)
{
	vector<int> key_tags;
	return key_tags;
}
#endif

text_object ELIZA::generate_reply()
{
	bool verbose = true;
	int n = 0;
	ascii response_template, str;
	text_object head;
	text_object tail;
	text_object result;

	context_ptr keyword;
	context_ptr tail_word;
	context_ptr last_word;
	vector<context_ptr>	markers;
	vector<int>	key_positions;

	// Identify keywords in input and select a templae for the reply.
	// It is not necessary to run the preporcessor at this point
	// and in fact, doing so would mess up reply selection when
	// responding to statemetns like "YOU DONT REALLY LOVE ME, DO YOU?"

	m_process = m_textIn;
	
	if (detect_nmea()==true)
		parse_nmea0 ();

	markers.reserve(8);
	markers.resize(1);
	m_process.rewind();
	m_process.m_nList.m_refCount = 1;

	// FIXME!! This is OK to get away with just so long as we arent
	// multithreaded - but the whole CriticalSection stuff needs to
	// be completely redone - before this will ever be ready to try
	// out on CUDA!  YES reference counting is BROKEN!!

	if (verbose == true) {
		writeln(output, "Tokens found: ");
		m_process.m_nList.print_debug();
	}
	
	text_item keywords = find_keywords();
	int num_keys = keywords.count_words();
	markers.resize(num_keys);

	if (verbose == true) {
		report_keywords(keywords);
		writeln(output, "Total Keys Found: ", num_keys);
		writeln(output, "Performing substitutions: ");
	}
	
	pre_process(m_conjugates);
	if (verbose == true) {
		m_process.m_nList.print_debug();
	}

#if 0
	key_positions = get_markers (keywords);
#endif
	int key_id = m_key;
	response_template = select_template(key_id);
	result = response_template;
	
	//  Build_response (index);
	//	Hopefually - now we have selected an appropraie reply that
	//	is relavant to the provided prompt.

	if (key_id!=-1)
	while (markers[n]!=nullptr)
	{
		keyword = keywords.begin();
		if (keyword!=nullptr)
			tail.m_nList.m_nBegin = markers[n];
		else
			tail = "?";

		markers[n] = m_process.m_nPos->m_pNext;
		if (markers[n]!=nullptr)
			m_process.m_nPos = markers[n];
	}

	tail_word = result.findPenultimate (str);
	result.get (str);
	result.peek (str);
	if (strcmp(str,"*")==0) {
		last_word = tail_word->m_pNext;
		delete last_word;
		tail_word->m_pNext = NULL;
		result.m_nList.m_nEnd = tail_word;
		result.append (tail); 
		result.append ("?");
	}
	result.rewind();
	return result;
}

