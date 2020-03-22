#ifndef __PARSE_H_
#define __PARSE_H_


enum exp{Fail, Var, Par, Abs, App};

struct str {
	char *ptr;
	int len;
};

struct term {
	char *(*print)(struct term*);
	char *(*printT)(struct term*);
	struct str id;
	enum exp type;
	struct term *node1;
	struct term *node2;
};

int parseTerm(struct str, struct term*);
int verify(struct str);

#endif // __PARSE_H_
