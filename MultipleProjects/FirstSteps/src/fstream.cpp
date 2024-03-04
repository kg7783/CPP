#include <fstream>
#include <string>
#include <iostream>

using namespace std ;



void manage_fstream(void)
{
    string theNames = "Edsger Dijkstra: Made advances in algorithms, the semaphore (programming).\n" ;
	theNames.append( "Donald Knuth: Wrote The Art of Computer Programming and created TeX.\n" ) ;
	theNames.append( "Leslie Lamport: Formulated algorithms in distributed systems (e.g. the bakery algorithm).\n") ; 
	theNames.append( "Stephen Cook: Formalized the notion of NP-completeness.\n" ) ;

	ofstream ofs( "theNames.txt"/*, ios::app*/ ) ;

	if( ! ofs )	
    {
		cout << "Error opening file for output" << endl ;
	} 	

	ofs << theNames << endl ;
	ofs.close() ;

    ifstream reader( "theNames.txt" ) ;

    if( ! reader ) 
    {
        cout << "Error opening input file" << endl ;
    }

    //! Read letter by letter
    cout << "Letter: " << endl;
    for( int i = 0; !reader.eof() ; i++ ) 
    {
        char letter;
        reader.get(letter) ;
        cout << letter ;
    }
    
    //! Move back to begin of file
    reader.clear();
    reader.seekg(0, ios::beg);

    //! Read line by line
    cout << "String: " << endl;
    for( int i = 0; !reader.eof() ; i++ ) 
    {
        string line;
        getline(reader, line) ;
        cout << line << endl ;
    }
}