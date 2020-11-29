#include "parse.h"
#include "Get_Next_Token.cpp"
#include <string>
#include <iostream>
#include <fstream>

inline bool Prog(istream& in, int& line){
  bool status;
  LexItem t = Parser::GetNextToken(in,line);

  if(t.GetToken() == BEGIN){
    status = StmtList(in, line);
    t = Parser::GetNextToken(in,line);
    if(t.GetToken() == END){
      status = true;
    }
    else if(t.GetToken() == DONE){
      ParseError(line, "No END Statment");
      return false;
    }
  }
  else{
    ParseError(line, "No BEGIN Statement");
    return false;
  }
  return status;
}

inline bool StmtList(istream& in, int& line){
  bool status;
  LexItem t = Parser::GetNextToken(in,line);

  Parser::PushBackToken(t);
  status = Stmt(in,line);
  if(status){
    t = Parser::GetNextToken(in,line);
    if(t.GetToken() == SCOMA){
      status = true;
    }
    else if(t.GetToken() != SCOMA){
      ParseError(line, "Missing semicolon");
      return false;
    }
    status = Stmt(in,line);
    while(status){
      t = Parser::GetNextToken(in,line);
      if(t.GetToken() != SCOMA){
        ParseError(line, "Missing semicolon");
        return false;
      }
      else{
        status = Stmt(in,line);
      }
    }

  }
  else if(t.GetToken() == END || t.GetToken() == DONE){
    ParseError(line, "No statements in program");
    return false;
  }
  return status;
}

inline bool Stmt(istream& in, int& line){
  bool status;
  LexItem t = Parser::GetNextToken(in,line);

  if(t.GetToken() == PRINT){
    status = PrintStmt(in, line);
  }
  else if(t.GetToken() == IF){
    Parser::PushBackToken(t);
    status = IfStmt(in, line);
  }
  else if(t.GetToken() == IDENT){
    Parser::PushBackToken(t);
    status = AssignStmt(in, line);
  }
  else if(t.GetToken() == END || t.GetToken() == DONE){
    Parser::PushBackToken(t);
    status = false;
  }
  else{
    ParseError(line, "Invalid statement ("+t.GetLexeme()+")");
    return false;
  }
  return status;
}

inline bool PrintStmt(istream& in, int& line){
  bool status;  status = ExprList(in, line);
  if(!status){
    ParseError(line, "Invalid expression after print");
    return false;
  }
  return status;
}

inline bool IfStmt(istream& in, int& line){
  bool status;
  LexItem t = Parser::GetNextToken(in,line);

  if(t.GetToken() == IF){
    t = Parser::GetNextToken(in,line);
    if(t.GetToken()==LPAREN){
      if(Expr(in,line)){
        t = Parser::GetNextToken(in,line);
        if(t.GetToken() == RPAREN){
          status = true;
        }
        else{
          ParseError(line, "Missing Right Parenthesis");
          return false;
        }
      }
      else{
        ParseError(line, "Missing if statement expression");
        return false;
      }
    }
    else{
      ParseError(line, "Missing Left Parenthesis");
      return false;
    }

    t = Parser::GetNextToken(in,line);
    if(t.GetToken() == THEN){
      status = Stmt(in, line);
    }
    else{
      ParseError(line, "Missing THEN");
      return false;
    }
  }
  return status;
}

inline bool AssignStmt(istream& in, int& line){
  bool status;
  LexItem t = Parser::GetNextToken(in,line);
  Parser::PushBackToken(t);

  if(Var(in,line)){
    t = Parser::GetNextToken(in,line);
    if(t.GetToken() == EQ){
      status = Expr(in, line);
      if(status){
        status = true;
      }
      else{
        ParseError(line, "Missing expression after EQ");
        return false;
      }
    }
    else{
      ParseError(line, "Missing Assignment Operator =");
      return false;
    }
  }
  else{
    ParseError(line, "Missing Expression in Assignment Statment");
    return false;
  }
  return status;
}

inline bool ExprList(istream& in, int& line){
  bool status;
  LexItem t = Parser::GetNextToken(in,line);

  Parser::PushBackToken(t);
  status = Expr(in,line);
  if(status){
    t = Parser::GetNextToken(in,line);
    while(t.GetToken() == COMA){

      if(Expr(in,line)){
        t = Parser::GetNextToken(in,line);
        status = true;
      }
    }
  }
  if(t.GetToken() == ERR){
    ParseError(line, "Missing Expression");
    return false;
  }
  Parser::PushBackToken(t);
  return status;
}

inline bool Expr(istream& in, int& line){
  bool status;
  LexItem t = Parser::GetNextToken(in,line);

  Parser::PushBackToken(t);
  status = Term(in,line);
  if(status){
    t = Parser::GetNextToken(in,line);
    while(t.GetToken() == PLUS || t.GetToken() == MINUS){

      status = Term(in,line);
      if(status){
        t = Parser::GetNextToken(in,line);
        status = true;
      }
    }
    if(t.GetToken() == ERR){
      ParseError(line, "Unrecognized input ("+t.GetLexeme()+")");
      return false;
    }
  }
  if(status){
    Parser::PushBackToken(t);
  }
  return status;
}

inline bool Term(istream& in, int& line){
  bool status;
  LexItem t = Parser::GetNextToken(in,line);

  Parser::PushBackToken(t);
  status = Factor(in,line);
  if(status){
    t = Parser::GetNextToken(in,line);
    while(t.GetToken() == MULT || t.GetToken() == DIV){

      status = Factor(in,line);
      if(status){
        t = Parser::GetNextToken(in,line);
        status = true;
      }
    }
    if(t.GetToken() == ERR){
      ParseError(line, "Unrecognized input ("+t.GetLexeme()+")");
      return false;
    }
  }
  if(status){
    Parser::PushBackToken(t);
  }
  return status;
}

inline bool Var(istream& in, int& line){
  bool status;
  string identstr;

  LexItem t = Parser::GetNextToken(in,line);

  if(t.GetToken() == IDENT){
    identstr=t.GetLexeme();
    if(!(defVar[identstr])){
      defVar[identstr]=true;
    }
    return true;
  }
  return false;
}

inline bool Factor(istream& in, int& line){
  bool status;
  LexItem t = Parser::GetNextToken(in,line);
  if(t.GetToken()==IDENT && (defVar.find(t.GetLexeme())->second)){
    return true;
  }
  else if(t.GetToken()==IDENT && !(defVar.find(t.GetLexeme())->second)){
    ParseError(line, "Undefined Variable");
    return false;
  }
  else if(t.GetToken()==ICONST){
    return true;
  }
  else if(t.GetToken()==RCONST){
    return true;
  }
  else if(t.GetToken()==SCONST){
    return true;
  }
  else if(t.GetToken()==LPAREN){
    if(Expr(in,line)){
      t = Parser::GetNextToken(in,line);
      if(t.GetToken() == RPAREN){
        return true;
      }
      else{
        ParseError(line, "Missing RPAREN");
        return false;
      }
    }
    else{
      ParseError(line, "Missing expression after LPAREN");
      return false;
    }
  }
  else{
    ParseError(line, "Unrecognized input ("+t.GetLexeme()+")");
    return false;
  }
  return status;
}

int main(int argc, char const *argv[]) {
  std::string userargs[argc];
  std::string filename;
  std::ifstream myfile;

  for(int i = 0; i<argc; i++)
  {
    userargs[i]= argv[i];
  }

  if(argc>1&&!myfile.is_open())
  {
    int filecount = 0;
    for(int i = 0; i<argc; i++)
    {
      if(userargs[i].find(".txt")!= std::string::npos)
      {
        filename = userargs[i];
        filecount++;
      }
    }
    if(filecount > 1){
      std::cout<<"ONLY ONE FILE NAME ALLOWED"<<std::endl;
      exit(0);
    }

    myfile.open(filename);
    if (!myfile.is_open())
    {
      std::cout<<"CANNOT OPEN THE FILE "<< filename <<std::endl;
      exit(0);
    }
  }

  int number = 1;
  bool final = Prog(myfile,number);
  if (final){
    std::cout<<"Successful Parsing"<<std::endl;
  }
  else{
    std::cout<<"Unsuccessful Parsing "<<std::endl;
    std::cout<<"Number of Syntax Errors: "<< error_count<<std::endl;
  }
}
