#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parse.h"


int parseNotApp(struct str, struct term*);
int parseVar(struct str, struct term*);
int parsePar(struct str, struct term*);
int parseAbs(struct str, struct term*);
int parseApp(struct str, struct term*);
char *printVar(struct term*);
char *printPar(struct term*);
char *printAbs(struct term*);
char *printApp(struct term*);
char *printVarT(struct term*);
char *printParT(struct term*);
char *printAbsT(struct term*);
char *printAppT(struct term*);

int verify(struct str target) {
		int pLevel = 0;
		int lLevel = 0;
		for (int i = 0; i < target.len; i++) {
			if (target.ptr[i] == '(') pLevel++;
			if (target.ptr[i] == ')') pLevel--;
			if (memcmp(target.ptr + i, "lambda", 5) == 0) {
				lLevel++;
				//i += 5;
			}
			if (target.ptr[i] == '.') lLevel--;
			if (pLevel < 0 || lLevel < 0) {
				break;
			}
		}
		if (pLevel) {
			return 1;
		}
		if (lLevel) {
			return 2;
		}
		return 0;
}

int parseTerm(struct str target, struct term *out) {
	return parseNotApp(target, out)
		|| parseApp(target, out);
}

int parseNotApp(struct str target, struct term *out) {
	return parseVar(target, out)
		|| parsePar(target, out)
		|| parseAbs(target, out);
}

int parseVar(struct str target, struct term *out) {
	if (target.len == 1 && *(target.ptr) >= 97 && *(target.ptr) <= 122) {
		out->type = Var;
		out->id = target;
		out->print = &printVar;
		out->printT = &printVarT;
		return 1;
	}
	return 0;
}

int parsePar(struct str target, struct term *out) {
	if (target.ptr[0] == '(' && target.ptr[target.len - 1] == ')') {
		struct str inner;
		inner.ptr = target.ptr + 1;
		inner.len = target.len - 2;
		if (verify(inner)) return 0;
		struct term *inTerm = (struct term*)malloc(sizeof(struct term));
		if(!parseTerm(inner, inTerm)) return 0;
		out->type = Par;
		out->node1 = inTerm;
		out->print = &printPar;
		out->printT = &printParT;
		return 1;
	}
	return 0;
}

int parseAbs(struct str target, struct term *out) {
	if (target.len > 7 && memcmp(target.ptr, "lambda", 5) == 0) {
		int i;
		for (i = 0; i < target.len; i++) {
			if (target.ptr[i] == '.') break;
		}
		// "lambdax.x" len  9
		//  012345678
		//         i = 7
		struct str var;
		var.ptr = target.ptr + 6;
		var.len = i - 6;
		struct str rule;
		rule.ptr = target.ptr + i + 1;
		rule.len = target.len - (i + 1);
		struct term *varTerm = (struct term*)malloc(sizeof(struct term));
		struct term *ruleTerm = (struct term*)malloc(sizeof(struct term));
		if(!(parseVar(var, varTerm) && parseTerm(rule, ruleTerm))) return 0;
		out->type = Abs;
		out->id = varTerm->id;
		out->node1 = varTerm;
		out->node2 = ruleTerm;
		out->print = &printAbs;
		out->printT = &printAbsT;
		return 1;
	}
	return 0;
}

int parseApp(struct str target, struct term *out) {
	int i = target.len - 1;
	struct str n;
	n.ptr = target.ptr + i;
	n.len = target.len - i;
	struct term *nTerm = (struct term*)malloc(sizeof(struct term));
	nTerm->type = Fail;
	int pLevel = 0;
	int lLevel = 0;
	for (; i >= 0; i--) {
		if (target.ptr[i] == '(') pLevel++;
		if (target.ptr[i] == ')') pLevel--;
		if (memcmp(target.ptr + i, "lambda", 5) == 0) lLevel++;
		if (target.ptr[i] == '.') lLevel--;
		if (pLevel == 0 && lLevel == 0 && parseNotApp(n, nTerm)) break;
		n.ptr --;
		n.len ++;
	}
	if (nTerm->type == Fail) return 0;
	struct str m;
	m.ptr = target.ptr;
	m.len = i;
	struct term *mTerm = (struct term*)malloc(sizeof(struct term));
	if (!parseTerm(m, mTerm)) return 0;
	out->type = App;
	out->node1 = mTerm;
	out->node2 = nTerm;
	out->print = &printApp;
	out->printT = &printAppT;
	return 1;
}

char *printVar(struct term *var) {
	char *id = (char*)malloc((var->id.len + 1) * sizeof(char));
	memcpy(id, var->id.ptr, var->id.len);
	id[var->id.len] = '\0';
	return id;
}
char *printPar(struct term *par) {
	char *in = par->node1->print(par->node1);
	char *out = (char*)malloc((strlen(in) + 2 + 1) * sizeof(char));
	sprintf(out, "(%s)", in);
	return out;
}
char *printAbs(struct term *abs) {
	char *id = abs->node1->print(abs->node1);
	char *term = abs->node2->print(abs->node2);
	char *out = (char*)malloc((strlen(id)+strlen(term)+7+1) * sizeof(char));
	sprintf(out, "lambda%s.%s", id, term);
	return out;
}
char *printApp(struct term *app) {
	char *m = app->node1->print(app->node1);
	char *n = app->node2->print(app->node2);
	char *out = (char*)malloc((strlen(m)+strlen(n)+1) * sizeof(char));
	sprintf(out, "%s%s", m, n);
	return out;
}

char *printVarT(struct term *var) {
	char *id = (char*)malloc((var->id.len + 1) * sizeof(char));
	memcpy(id, var->id.ptr, var->id.len);
	id[var->id.len] = '\0';
	char *out = (char*)malloc((strlen(id) + 5 + 1) * sizeof(char));
	sprintf(out, "Var(%s)", id);
	return out;
}
char *printParT(struct term *par) {
	char *in = par->node1->printT(par->node1);
	char *out = (char*)malloc((strlen(in) + 5 + 1) * sizeof(char));
	sprintf(out, "Par(%s)", in);
	return out;
}
char *printAbsT(struct term *abs) {
	char *id = abs->node1->printT(abs->node1);
	char *term = abs->node2->printT(abs->node2);
	char *out = (char*)malloc((strlen(id)+strlen(term)+7+1) * sizeof(char));
	sprintf(out, "Abs(%s, %s)", id, term);
	return out;
}
char *printAppT(struct term *app) {
	char *m = app->node1->printT(app->node1);
	char *n = app->node2->printT(app->node2);
	char *out = (char*)malloc((strlen(m)+strlen(n)+7+1) * sizeof(char));
	sprintf(out, "App(%s, %s)", m, n);
	return out;
}
