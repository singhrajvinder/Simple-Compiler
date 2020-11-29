#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "lex.h"
#include <cctype>
#include <map>



inline LexItem getNextToken(istream&in,int&linenum){
	std::string word;
	enum TokState{START,INID,INSTRING,ININT,INREAL,INCOMMENT,SIGN} lexstate = START;
	LexItem myLex;
	char char1;
	bool reals = false;
	bool strend;
	while(in.get(char1))
	{
		switch (lexstate)
		{
		  case START:
			if(char1=='\n'){
				linenum++;
				continue;
			}
			else if(isspace(char1)){
				continue;
			}

			word = word + char1;

			if(isalpha(char1)){
				lexstate=INID;
			}
			else if(char1=='/'){
				lexstate=INCOMMENT;
			}
			else if(isdigit(char1)){
				lexstate=ININT;
			}
			else if(char1=='"'){
				strend = false;
				word = "";
				lexstate=INSTRING;
			}
			else if(char1=='.'){
				lexstate=INREAL;
			}
			else{
				lexstate=SIGN;
			}
			continue;

		  case INID:
			if(isalnum(char1))
			{
				word = word+char1;
			}
			else if(word == "begin"){
				in.putback(char1);
				return LexItem(BEGIN, word,linenum);
			}
			else if(word == "print"){
				in.putback(char1);
				return LexItem(PRINT, word,linenum);
			}
			else if(word == "end"){
				in.putback(char1);
				return LexItem(END, word,linenum);
			}
			else if(word == "if"){
				in.putback(char1);
				return LexItem(IF, word,linenum);
			}
			else if(word == "then"){
				in.putback(char1);
				return LexItem(THEN, word,linenum);
			}
			else if(isalnum(char1) == false){
				in.putback(char1);
				return LexItem(IDENT, word,linenum);
			}
			else{
				return LexItem(ERR, word,linenum);
			}
			continue;

		  case INSTRING:
			if(in.peek()=='\n'){
				return LexItem(ERR, word,linenum);
			}
			else if(char1=='"'){
				return LexItem(SCONST, word,linenum);
			}
			else{
				word= word +char1;
			}
			continue;

		  case ININT:
			if(isdigit(char1)){
				word = word +char1;
				lexstate = START;
			}
			else if(isdigit(char1)==false && char1!= '.'){
				in.putback(char1);
				return LexItem(ICONST, word,linenum);
			}
			else if(char1 == '.'){
				word = word +char1;
				lexstate = INREAL;
			}
			continue;

		  case INREAL:
			if(isdigit(char1)){
				reals = true;
				word = word +char1;
			}
			else if(isdigit(char1)==false && reals){
				in.putback(char1);
				return LexItem(RCONST, word,linenum);
			}
			else{
				return LexItem(ERR, word,linenum);
			}
			continue;

		  case INCOMMENT:
			if (char1=='/'){
				word = "";
			}
			else if (word == "/"){
				in.putback(char1);
				return LexItem(DIV, word,linenum);
			}
			else if(char1=='\n'){
				linenum++;
				lexstate=START;
			}
			continue;

		  case SIGN:
			if(word=="+"){
				in.putback(char1);
				return LexItem(PLUS,word,linenum);
			}
			else if(word=="-"){
				in.putback(char1);
				return LexItem(MINUS,word,linenum);
			}
			else if(word=="*"){
				in.putback(char1);
				return LexItem(MULT,word,linenum);
			}
			else if(word=="/"){
				in.putback(char1);
				return LexItem(DIV,word,linenum);
			}
			else if(word=="="){
				in.putback(char1);
				return LexItem(EQ,word,linenum);
			}
			else if(word=="("){
				in.putback(char1);
				return LexItem(LPAREN,word,linenum);
			}
			else if(word==")"){
				in.putback(char1);
				return LexItem(RPAREN,word,linenum);
			}
			else if(word==";"){
				in.putback(char1);
				return LexItem(SCOMA,word,linenum);
			}
			else if(word==","){
				in.putback(char1);
				return LexItem(COMA,word,linenum);
			}
			else{
				return LexItem(ERR, word,linenum);
			}
			continue;
		}
	}

	bool intend;
	bool realend;
	for (int i = 0; i < word.length(); i++){
		if (isdigit(word[i]) == false){
      intend = false;
		}
		else if(word[i] == '.'){
			intend = false;
			realend = true;
		}
		else if(isdigit(word[i])){
			intend = true;
		}
	}

	bool alnum;
	for (int i = 0; i < word.length(); i++){
		if(isalnum(char1)){
			alnum = true;
		}
	}
	if(word.length() >0){
		if(word == "begin"){
			return LexItem(BEGIN, word,linenum);
		}
		else if(word == "print"){
			return LexItem(PRINT, word,linenum);
		}
		else if(word == "end"){
			return LexItem(END, word,linenum);
		}
		else if(word == "if"){
			return LexItem(IF, word,linenum);
		}
		else if(word == "then"){
			return LexItem(THEN, word,linenum);
		}
		else if(intend == true){
			return LexItem(ICONST, word,linenum);
		}
		else if(intend == true && realend == true){
			return LexItem(RCONST, word,linenum);
		}
		else if(word=="+"){
			in.putback(char1);
			return LexItem(PLUS,word,linenum);
		}
		else if(word=="-"){
			in.putback(char1);
			return LexItem(MINUS,word,linenum);
		}
		else if(word=="*"){
			in.putback(char1);
			return LexItem(MULT,word,linenum);
		}
		else if(word=="/"){
			in.putback(char1);
			return LexItem(DIV,word,linenum);
		}
		else if(word=="="){
			in.putback(char1);
			return LexItem(EQ,word,linenum);
		}
		else if(word=="("){
			in.putback(char1);
			return LexItem(LPAREN,word,linenum);
		}
		else if(word==")"){
			in.putback(char1);
			return LexItem(RPAREN,word,linenum);
		}
		else if(word==";"){
			in.putback(char1);
			return LexItem(SCOMA,word,linenum);
		}
		else if(word==","){
			in.putback(char1);
			return LexItem(COMA,word,linenum);
		}
		else if(alnum && strend){
			return LexItem(IDENT, word,linenum);
		}
		else{
			return LexItem(ERR, word,linenum);
		}
	}
	return LexItem(DONE, word,linenum);
}
