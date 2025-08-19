
#include "stdafx.h"
#include <vector>
#include "defines.h"
#include "intrinsics.h"
#include "btreetype.h"
#include "text_object.h"
#include "ELIZA.h"

template <class X> class bTreeType;

#define nullptr NULL

subst conjugates[];
key_info keywords[];
char* replies[];

char* narrative::get_response(int i)
{
    char *result;
    result = replies[i];
    return result;
}

size_t narrative::get_response_count()
{
    size_t sz = 0;
    char *ptr = replies[sz];
    while (ptr != nullptr)
    {   
        ptr = replies[sz];
        sz++;
    } 
    return sz;
}

subst *narrative::get_conjugates()
{
    subst *result = &(conjugates[0]);
    return result;
}

key_info *narrative::get_keywords()
{
    key_info *result = &(keywords[0]);
    return result;
}

key_info keywords[] = 
{
	key_info("THANK",2),
	key_info("CAN YOU",3),
	key_info("CAN I",2),
	key_info("YOU ARE",4),
	key_info("YOU'RE",-1),
	key_info("I DON'T",4),
    key_info("I FEEL",3),
	key_info("WHY DON'T YOU",3),
	key_info("WHY CAN'T I",2),
	key_info("ARE YOU",3),
    key_info("I CAN'T",3),
	key_info("I AM",4),
	key_info("I'M",-1),
	key_info("YOUR",2),
	key_info("I WANT",5),
	key_info("WHAT",9),
    key_info("HOW",-1),
	key_info("WHO",-1),
	key_info("WHERE",-1),
	key_info("WHEN",-1),
	key_info("WHY",-1),
	key_info("NAME",2),
	key_info("CAUSE",4),
    key_info("SORRY",4),
	key_info("DREAM",4),
	key_info("HELLO",1),
	key_info("HI",-1),
	key_info("MAYBE",5),
	key_info("NO",5),
	key_info("NOT",-1),
    key_info("NOTHING",-1),
	key_info("YOU",3),
	key_info("ALWAYS",4),
	key_info("THINK",3),
	key_info("ALIKE",6),
	key_info("YES",4),
    key_info("FRIEND",6),
	key_info("COMPUTER",7),
	key_info("NOKEYFOUND",7),
	key_info(NULL,0),
};

subst conjugates [] = 
{
	subst("ARE","AM"),
	subst("WERE","WAS"),
	subst("YOUR","MY"),
	subst("I'VE","YOU'VE"),
	subst("I'M","YOU'RE"),
	subst("ME","YOU"),
	subst("AM","ARE"),
	subst("WAS","WERE"),
	subst("I","YOU"),
	subst("MY","YOUR"),
	subst("YOU'VE","I'VE"),
	subst("YOU'RE","I'M"),
	subst("YOU","I"),
	subst("MYSELF","YOURSELF"),
	subst("YOURSELF","MYSELF"),
	subst(NULL,NULL),
};

subst pascal2c [] = 
{
	subst(";",";"),
	subst("ASSIGN_EQ","="),
	subst("COMPARE_EQ","=="),
	subst("AND","&&"),
	subst("OR","||"),
	subst("NOT","!"),
	subst("MOD","%"),
	subst("DIV","/"),
	subst("INTEGER","int"),
	subst("REAL","float"),
	subst("BOOLEAN","bool"),
	subst("PROCEDURE","void"),
	subst("PACKED",""),
	subst("ARRAY","[]"),
	subst("BEGIN","{"),
	subst("END","}"),
	subst("RECORD","struct {"),
	subst("CASE","switch ("),
	subst("CASE","union {"),
	subst("OF",")"),	
	subst("IF","if ("),
	subst("THEN",")"),
	subst("ELSE","else"),
	subst("FOR","for ("),
	subst("TO",";$1<="),
	subst("DOWNTO",";$1>="),
	subst("DO",")"),
	subst("REPEAT","do {"),
//	subst("UNTIL","} UNTIL_CAPTURE"),	
//	subst("UNTIL_CAPTURE","(!"),
	subst("UNTIL"," } while (!("),
	subst("WHILE","while ("),	
	subst("(*","/*"),
	subst("(*$","/*"),
	subst("*)","*/"),
	subst("{","/*"),
	subst("}","*/"),
//	subst("= [","= SET("),
//	subst("]",")"),
	subst("^.","->"),
//	subst(":=","ASSIGN_EQ"),
//	subst("=","COMPARE_EQ"),
	subst(":=","="),
	subst("=","=="),
	subst("<>","!="),
	subst("WITH","with ("),
	subst("'","\""),
	subst(NULL,NULL),
};

char* replies[] =
{
    /* THANK */
    "YOU'RE WELCOME",
    "I'M GLAD I COULD HELP",

    /* CAN YOU */
    "DON'T YOU BELIEVE THAT I CAN *",
    "PERHAPS YOU BELIEVE I SHOULD *",
    "YOU WANT ME TO BE ABLE TO *",

    /* CAN I */
    "PERHAPS YOU DON'T WANT TO *",
    "DO YOU WANT TO BE ABLE TO *",

    /* YOU ARE, YOU'RE */
    "WHAT MAKES YOU THINK I AM *",
    "DOES IT PLEASE YOU TO BELIEVE I AM *",
    "PERHAPS YOU WOULD LIKE TO BE *",
    "DO YOU SOMETIMES WISH YOU WERE *",

    /* I DON'T */
    "DON'T YOU REALLY *",
    "WHY DON'T YOU *",
    "DO YOU WISH TO BE ABLE TO *",
    "DOES IT TROUBLE YOU TO NOT *",

    /* I FEEL */
    "DO YOU OFTEN FEEL *",
    "WHY DO YOU THINK YOU FEEL *",
    "DO YOU ENJOY FEELING *",

    /* WHY DON'T YOU */
    "DO YOU REALLY BELIEVE I DON'T *",
    "PERHAPS IN GOOD TIME I WILL *",
    "DO YOU WANT ME TO *",

    /* WHY CAN'T I */
    "DO YOU THINK YOU SHOULD BE ABLE TO *",
    "WHY CAN'T YOU *",

    /* ARE YOU */
    "WHY ARE YOU INTERESTED IN WHETHER OR NOT I AM *",
    "WOULD YOU PREFER IF I WERE NOT *",
    "PERHAPS IN YOUR FANTASIES I AM *",

    /* I CAN'T */
    "HOW DO YOU KNOW YOU CAN'T *",
    "HAVE YOU TRIED?",
    "PERHAPS YOU CAN NOW *",

    /* I AM, I'M */
    "DID YOU COME TO ME BECAUSE YOU ARE *",
    "HOW LONG HAVE YOU BEEN *",
    "DO YOU BELIEVE IT IS NORMAL TO BE *",
    "DO YOU ENJOY BEING *",

    /* YOUR */
    "WHY ARE YOU CONCERNED ABOUT MY *",
    "WHAT ABOUT YOUR OWN *",

    /* I WANT */
    "WHAT WOULD IT MEAN TO YOU IF YOU GOT *",
    "WHY DO YOU WANT *",
    "SUPPOSE YOU SOON GOT *",
    "WHAT IF YOU NEVER GOT *",
    "I SOMETIMES ALSO WANT *",

    /* HOW, WHO, WHERE, WHEN, WHY */
    "WHY DO YOU ASK?",
    "DOES THAT QUESTION INTEREST YOU?",
    "WHAT ANSWER WOULD PLEASE YOU THE MOST?",
    "WHAT DO YOU THINK?",
    "ARE SUCH QUESTIONS ON YOUR MIND OFTEN?",
    "WHAT IS IT THAT YOU REALLY WANT TO KNOW?",
    "HAVE YOU ASKED ANYONE ELSE?",
    "HAVE YOU ASKED SUCH QUESTIONS BEFORE?",
    "WHAT else COMES TO MIND WHEN YOU ASK THAT?",

    /* NAME */
    "NAMES DON'T INTEREST ME.",
    "I DON'T CARE ABOUT NAMES --PLEASE GO ON.",

    /* CAUSE */
    "IS THAT THE REAL REASON?",
    "DON'T ANY OTHER REASONS COME TO MIND?",
    "DOES THAT REASON EXPLAIN ANYTHING ELSE?",
    "WHAT OTHER REASONS MIGHT THERE BE?",

    /* SORRY */
    "PLEASE DON'T APOLOGIZE!",
    "APOLOGIES ARE NOT NECESSARY.",
    "WHAT FEELINGS DO YOU HAVE WHEN YOU APOLOGIZE?",
    "DON'T BE SO DEFENSIVE!",

    /* DREAM */
    "WHAT DOES THAT DREAM SUGGEST TO YOU?",
    "DO YOU DREAM OFTEN?",
    "WHAT PERSONS APPEAR IN YOUR DREAMS?",
    "ARE YOU DISTURBED BY YOUR DREAMS?",

    /* HELLO, HI */
    "HOW DO YOU DO ...PLEASE STATE YOUR PROBLEM.",

    /* MAYBE */
    "YOU DON'T SEEM QUITE CERTAIN.",
    "WHY THE UNCERTAIN TONE?",
    "CAN'T YOU BE MORE POSITIVE?",
    "YOU AREN'T SURE?",
    "DON'T YOU KNOW?",

    /* NO, NOT, NOTHING */
    "ARE YOU SAYING NO JUST TO BE NEGATIVE?",
    "YOU ARE BEING A BIT NEGATIVE.",
    "WHY NOT?",
    "ARE YOU SURE?",
    "WHY NO?",

    /* YOU */
    "WE WERE DISCUSSING YOU--NOT ME.",
    "OH, I*",
    "YOU'RE NOT REALLY TALKING ABOUT ME, ARE YOU?",

    /* ALWAYS */
    "CAN YOU THINK OF A SPECIFIC EXAMPLE?",
    "WHEN?",
    "WHAT ARE YOU THINKING OF?",
    "REALLY, ALWAYS?",

    /* THINK */
    "DO YOU REALLY THINK SO?",
    "BUT YOU ARE NOT SURE*",
    "DO YOU DOUBT*",

    /* ALIKE */
    "IN WHAT WAY?",
    "WHAT RESEMBLANCE DO YOU SEE?",
    "WHAT DOES THE SIMILARITY SUGGEST TO YOU?",
    "WHAT OTHER CONNECTIONS DO YOU SEE?",
    "COULD THERE REALLY BE SOME CONNECTION?",
    "HOW?",

    /* YES */
    "ARE YOU SURE?",
    "I SEE.",
    "I UNDERSTAND.",
    "YOU SEEM QUITE POSITIVE.",

    /* FRIEND */
    "WHY DO YOU BRING UP THE TOPIC OF FRIENDS?",
    "DO YOUR FRIENDS WORRY YOU?",
    "DO YOUR FRIENDS PICK ON YOU?",
    "ARE YOU SURE YOU HAVE ANY FRIENDS?",
    "DO YOU IMPOSE ON YOUR FRIENDS?",
    "PERHAPS YOUR LOVE FOR FRIENDS WORRIES YOU.",

    /* COMPUTER */
    "DO COMPUTERS WORRY YOU?",
    "ARE YOU TALKING ABOUT ME IN PARTICULAR?",
    "ARE YOU FRIGHTENED BY MACHINES?",
    "WHY DO YOU MENTION COMPUTERS?",
    "WHAT DO YOU THINK MACHINES HAVE TO DO WITH YOUR PROBLEM?",
    "DON'T YOU THINK COMPUTERS CAN HELP PEOPLE?",
    "WHAT IS IT ABOUT MACHINES THAT WORRIES YOU?",

    /* NOKEYFOUND */
    "WHAT DOES THAT SUGGEST TO YOU?",
    "I SEE.",
    "I'M NOT SURE I UNDERSTAND YOU FULLY.",
    "COME COME ELUCIDATE YOUR THOUGHTS.",
    "CAN YOU ELABORATE ON THAT?",
    "THAT IS QUITE INTERESTING.",
    "SAY, DO YOU HAVE ANY PSYCHOLOGICAL PROBLEMS?"
};