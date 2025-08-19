
template <class X> class bTreeType;

typedef enum language
{
	whitespace,
	delimiter,
	command,
	reserved_lisp,
	reserved_fortran,
	reserved_basic,
	reserved_c,
	reserved_pascal,
	reserved_perl,
	reserved_python,
	reserved_spin,
	unknown
} _language;

typedef enum parse
{
	comment,
	numeric,
	string,
	function,
	procedure,
	expression,
	block,
} _parse;

template <class nodeType> class node
{
private:
	volatile int m_refCount;

public:
	nodeType m_pData;
	node *m_pNext;
	node ();
	~node ();
	node (nodeType);
};

template <class nodeType, class enumType>
class s_node
{
#ifdef debug
	char *m_tagid;
#endif
	volatile int	refCount;

public:
	s_node *m_pNext;
	nodeType m_pData;
	enumType m_typeid;

	s_node ();
	~s_node ();
	s_node (nodeType);
	enumType check (char *(&arg));
//	enumType check (const nodeType &arg);
};

template <class Type> class node_list
{
friend class key_list;
friend class text_object;
friend class ELIZA;

protected:
	volatile int m_refCount;
	node<Type>* m_nBegin;
	node<Type>* m_nEnd;
	node<Type>* m_nPos;

public:
	node_list ();
	node_list (Type m_pText, bool);
	node_list (Type m_pText);
	node_list (bTreeType<Type> **source);
	~node_list ();

	node_list &operator = (node_list& arg)
	{
		arg.m_refCount++;
		this->m_refCount = arg.m_refCount;
		this->m_nBegin = arg.m_nBegin;
		this->m_nEnd = arg.m_nEnd;
		this->m_nPos = arg.m_nPos;
		return *this;
	}

	void print_debug () const;
	inline node<Type> *begin() const
		{ return m_nBegin; }
	inline node<Type> *end() const
		{ return m_nEnd; }
	inline node<Type> *pos() const
		{ return m_nPos; }

	int count_words() const
	{
		int result = 0;
		node<Type>* ptr = begin();
		while (ptr != nullptr) {
			ptr = ptr->m_pNext;
		result++; }
		return result;
	}
	node<Type> *pop_front();
	void push_back (node_list &);
	Type &operator -> ();

	void putTempWord (char *theWord);
	void concat(node<Type>*);
	void append_node (Type);
	void append_list (node_list*);
	void transferFrom (node_list*);
	void transferTo (node_list*);

// functions that operate on the sList
	void attachWord (s_node<Type,enum language> *theWord);
	Type putNextNode (s_node<Type,enum language> *(&theWord));
	void putLastNode (s_node<Type,enum language> *(&theWord));

//	char *findPenultimate ();
	s_node<Type,enum language> *removeWord (s_node<Type,enum language> *);
	s_node<Type,enum language> *search (Type);

//	void friend rewind (node_list *m_nList);
	node<Type> &operator >> (Type&);
//	node<Type> &operator >> (char*&);
};
