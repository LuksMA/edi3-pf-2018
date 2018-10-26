/**
* Copyright (c) 2018 Leonardo Casales - Jonathan Egea. All rights reserved. .0
**/

// #include <libpq-fe.h> // include\vendors\postgreSQL\libpq-fe.h
#include "/Library/PostgreSQL/11/include/libpq-fe.h"
//#include "libpq-fe.h"

#include <iostream>
#include <string>

#include <compset/ComponentFactory.h>

#include <compset/DatabaseHandlerInterface.h>
#include <compset/ComponentInterface.h>

class PosgreSQLDatabaseHandler : public DatabaseHandlerInterface, public ComponentInterface {

    public:
        PosgreSQLDatabaseHandler() ;
        virtual ~PosgreSQLDatabaseHandler() ;
        virtual bool getErrorStatus() ;
        virtual DatabaseHandlerInterface* setQuery( std::string query ) ;
        virtual DatabaseHandlerInterface* setStoredProcedure( std::string storedProcedure ) ;
        virtual DatabaseHandlerInterface* addParameter( std::string key, std::string value ) ;
        virtual void execQuery() ;
        virtual DataType fetch() ;
        virtual DatumType fetchAll() ;

        //ComponentInterface:
        bool implements(std::string interfaceName);
        void* getInstance();
        void release();
        //PosgreSQLDatabaseHandler ( char* host, char* port, char* dataBase, char* user, char* passwd ) ;
        // bool Connect() ;
        // void Disconnect() ;
        // int Show( char* table ) ;

    private:
        std::string host ;
        std::string dataBase ;
        std::string port ;
        std::string user ;
        std::string passwd ;
        PGconn *cnn ;
        PGresult *result ;
        bool connected ;
        std::string qSQL ;

        int referenceCounter ;
        bool implemented ;
        void ReadConfig() ;
};

// PosgreSQLDatabaseHandler::PosgreSQLDatabaseHandler() {
//     PGconn *cnn = NULL;
//     PGresult *result = NULL;
// }

PosgreSQLDatabaseHandler::PosgreSQLDatabaseHandler () {

    ReadConfig() ;

    PGconn* cnn = NULL ;
    PGresult* result = NULL ;
    qSQL = "" ;

	cnn = PQsetdbLogin( host ,
                        port , NULL , NULL ,
                        dataBase , 
                        user ,
                        passwd);

    if (PQstatus(cnn) != CONNECTION_BAD) {
        printf( "Estamos conectados a PostgreSQL!<br>\n" ) ; 
        connected = true ;
        result = PQexec(cnn, "");
	} else {
		printf( "Error de conexion!<br>\n" ) ;
		PQfinish(cnn) ;
		connected = false ;
	}
}

PosgreSQLDatabaseHandler::~PosgreSQLDatabaseHandler() {
	if ( connected ) {
    	PQclear(result) ;
    	PQfinish(cnn) ;
	}
}

DatabaseHandlerInterface* PosgreSQLDatabaseHandler::setQuery( std::string query ) {
    qSQL = query ;
}

void PosgreSQLDatabaseHandler::ReadConfig() {
	std::string data[6];
	std::ifstream inifile;

	inifile.open( "config.ini", std::ios::in );

	if ( inifile.fail() ){
		std::cout << "no se pudo leer" ;
	}

	int i = 0 ;
	while( !inifile.eof() ) {

		std::getline( inifile , data[i] ) ;
		std::cout << data[i] << std::endl ;
                i++;
    }

    // host = data[1] ;
    // dataBase = data[2] ;
    // port = data[3] ;
    // user = data[4] ;
    // passwd = data[5] ;
}

void PosgreSQLDatabaseHandler::execQuery() {

    int i = 0 ;    
    
    if (connected ) {
        
        result = PQexec(cnn, qSQL);
        
        if (result != NULL) {
            int tuplas = PQntuples(result);
            int campos = PQnfields(result);
            printf( "No. Filas: %i<br>\n", tuplas ) ;
            printf( "No. Campos:%i<br>\n", campos ) ;
            
            printf( "Los nombres de los campos son:<br>\n" ) ;
            
            for (i=0; i<campos; i++) {
                printf( "%s | ", PQfname(result,i) ) ;
            }
            
            printf( "Contenido de la tabla<br>\n" ) ;
            
            for (i=0; i<tuplas; i++) {
                for (int j=0; j<campos; j++) {
                    printf( "%s | ",PQgetvalue(result,i,j) );
                }
                printf ( "<br>\n" ) ;
            }
        }
    }

}

bool PosgreSQLDatabaseHandler::getErrorStatus() {
    return ( PQstatus(cnn) == CONNECTION_BAD ) ;
}

/*
bool Postgresql::ExecuteQuery( const char* query , IList< Tuple *> *rowsOfQuery){
    result = PQexec(cnn, query);
    if(PQresultStatus(result)==PGRES_TUPLES_OK){
        ExecuteResponse(rowsOfQuery);
    }
    return ( PQresultStatus( result ) == PGRES_COMMAND_OK ) ? true : false ;
}

void Postgresql::ExecuteResponse(IList< Tuple *> *rowsOfQuery){
	
	int rows = PQntuples(result);
	int fields = PQnfields(result);

    for( int i=0; i<rows; i++ ) {
        Tuple *tuple = new Tuple();
        for (int f = 0; f < fields; f++){
        	tuple->AddEntry(PQfname( result, f ), PQgetvalue( result, i, f ) ) ;
        }
        rowsOfQuery->Add( tuple ) ;
    }
    

}*/

//ComponentInterface:
bool PosgreSQLDatabaseHandler::implements(std::string interfaceName)
{
    return (interfaceName == "ComponentInterface" || interfaceName == "DatabaseHandlerInterface") ?
        implemented = true
            : implemented = false;
}

void* PosgreSQLDatabaseHandler::getInstance()
{
    if(implemented) {  referenceCounter++;  return this; }
    return NULL;
}

void PosgreSQLDatabaseHandler::release()
{
    referenceCounter--;
    if(referenceCounter <= 0) delete this;
}

extern "C" ComponentInterface* create();

ComponentInterface* create()
{
    return (ComponentInterface*) new PosgreSQLDatabaseHandler();
}