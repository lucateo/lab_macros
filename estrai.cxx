
#include <fstream>
#include <iostream>

using namespace std;
int main() 
{
  ifstream OpenFile("provvisorio.txt");
  char ch;
  while(!OpenFile.eof())
  {
	  if(ch=='	')
	{
		OpenFile.get(ch);
		cout << ch;
	
	}
	else
	{	
    OpenFile.get(ch);
    if (ch=='	')  //ho messo il tab, devo mettere tra ' ' e non tra " " sennò non funziona
        cout << ch << " & ";
    else if (ch=='\n') //a capo
		cout << "\\\\" << ch; //per mettere doppio backslash
    
    else
		cout << ch;
	}
    
  }
  OpenFile.close();
  return 0;
}
