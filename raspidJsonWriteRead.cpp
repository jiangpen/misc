#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include "fstream"
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/reader.h>
#include "rapidjson/filereadstream.h"
#include <rapidjson/document.h>
using namespace rapidjson;
using namespace std;

int main()
{
    StringBuffer s;
    Writer<StringBuffer> writer( s );
    Document m_document;
    writer.StartObject();               // Between StartObject()/EndObject(), 
    writer.Key( "records" );
    writer.StartArray();
    int count = 0;

    for( int i = 0; i < 10; i++ )
    {

        writer.StartObject();
        writer.Key( "dataI" );
        writer.Uint( count );
        writer.Key( "dataS" );
        writer.String( "test" );
        writer.Key( "dataB" );
        writer.Bool( true );
        count++;

        writer.EndObject();
        // all values are elements of the array.

    }
    writer.EndArray();
    writer.EndObject();

    FILE* fp = fopen( "output.json", "wb" );
    fprintf( fp, "%s", s.GetString() );
    fclose( fp );

    FILE * fpr = fopen( "output.json", "rb" );
    char read_buffer[1024];
    rapidjson::FileReadStream is( fpr, read_buffer, sizeof(read_buffer) );
    rapidjson::ParseResult ok = m_document.ParseStream( is );



    if( !ok.IsError() )
    {
        if( m_document.HasMember( "records" ) && m_document["records"].IsArray() )
        {
            for( int i = 0; i < m_document["records"].Size(); i++ )
            {
                if(m_document["records"][i].HasMember("dataI"))
                {
                    printf( "%d\n", m_document["records"][i]["dataI"].GetInt() );
                }
                if(m_document["records"][i].HasMember("dataS"))
                {
                    printf( "%s\n", m_document["records"][i]["dataS"].GetString() );
                }
                if(m_document["records"][i].HasMember("dataB"))
                {
                    printf( "%d\n", m_document["records"][i]["dataB"].GetBool() );
                }


            }
        }
    }


    return 0;
}
