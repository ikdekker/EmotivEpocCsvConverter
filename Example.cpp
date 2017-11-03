/*
Copyright (C) 2014 Jay Satiro <raysatiro@yahoo.com>
All rights reserved.

This file is part of project Example (CSV/jay::util).

Example is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Example is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Example. If not, see <http://www.gnu.org/licenses/>.
*/

/** CSV class example
*/

#include <Windows.h>
#include <stdio.h>

#include <iostream>
#include <list>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include <algorithm>

#include "CSV.hpp"


using namespace std;


static void pause( void )
{
        system( "pause" );
        return;
}

void init()
{
    /* If the program is started in its own window then pause before exit
    (eg user clicks on program in explorer, or vs debugger initiated program)
    */
    {
        HANDLE hOutput = GetStdHandle( STD_OUTPUT_HANDLE );
        CONSOLE_SCREEN_BUFFER_INFO csbi;

        ZeroMemory( &csbi, sizeof( csbi ) );

        if( ( hOutput != INVALID_HANDLE_VALUE )
            && ( GetFileType( hOutput ) == FILE_TYPE_CHAR )
            && GetConsoleScreenBufferInfo( hOutput, &csbi )
            && !csbi.dwCursorPosition.X
            && !csbi.dwCursorPosition.Y
            && ( csbi.dwSize.X > 0 )
            && ( csbi.dwSize.Y > 0 )
            )
        {
            // increase console buffer size to 10000 lines and pause before exit

            if( csbi.dwSize.Y < 10000 )
                csbi.dwSize.Y = 10000;

            SetConsoleScreenBufferSize( hOutput, csbi.dwSize );
            atexit( pause );
        }
    }
}


int main()// int argc, char *argv[] )
{
    init();

    jay::util::CSVread csv_read( "Example.csv",
        jay::util::CSVread::strict_mode
        //| jay::util::CSVread::text_mode
        | jay::util::CSVread::process_empty_records
        //| jay::util::CSVread::skip_utf8_bom_check
    );

	jay::util::CSVwrite csv_write("Example_fixed.csv");
	csv_write.WriteField("Time (s)");
    if( csv_read.error )
    {
        cerr << "CSVread failed: " << csv_read.error_msg << endl;
        exit( 1 );
    }

    while( csv_read.ReadRecord() )
    {
			cout << endl << "Record #" << csv_read.record_num << endl;
	
        for( unsigned i = 0; i < csv_read.fields.size(); ++i )
		{
			if (csv_read.record_num == 1) {
				cout << endl << "rec 1" << endl;
				string curVal = csv_read.fields[i];
				int lbl = curVal.find("label");
				int rawCQ = curVal.find("RAW_CQ");

				if (lbl != string::npos) {
					curVal.erase(rawCQ - 1); // remove stuff after the locations
					curVal.erase(0, lbl + 28); // remove "labels: COUNTER INTERPOLATED"

					//replace(curVal.begin(), curVal.end(), ' ', ',');
					cout << curVal << endl;
					
					istringstream iss(curVal);
					string s;
					while (getline(iss, s, ' ')) {
						csv_write.WriteField(s.c_str());
						printf("`%s'\n", s.c_str());
					}
					csv_write.WriteField("Sampling Rate");
					csv_write.WriteTerminator();
				}


			}
			if (csv_read.record_num > 1) {
				float hz = 1.0 / 128.0;
				float timeStamp = (csv_read.record_num - 1) * hz;
				stringstream tsStr(stringstream::in | stringstream::out);
				tsStr << timeStamp;
				if (i == 0) {
					csv_write.WriteField(tsStr.str());
				}
				if (i > 1 && i < 16) {
					csv_write.WriteField(csv_read.fields[i]);
				}
			}
	    }

		if (csv_read.record_num == 2) {
			csv_write.WriteField("128");
		}
		if (csv_read.record_num > 1)
		csv_write.WriteTerminator();
    }

    if( csv_read.eof && ( csv_read.record_num == csv_read.end_record_num ) )
    {
        cout << "All records read successfully. (" << csv_read.end_record_num << ")" << endl;
        if( csv_read.end_record_not_terminated )
        {
            cout << "WARNING: End record not terminated!" << endl;
        }
    }
    else if( csv_read.error )
    {
        cerr << "Error: " << csv_read.error_msg << endl;
    }

    return 0;
}
