g++ -c -shared -Wall -std=c++11 -I. -I./include -I../../../include -o DBGeneratorMariaDB.dll DBGeneratorMariaDB.cpp -lmariadb -lmariadbclient