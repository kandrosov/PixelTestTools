all: prod_file_DB

prod_file_DB: prod_file_DB.cpp
	g++ prod_file_DB.cpp -o bin/prod_file_DB
