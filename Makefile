all: prod_file_DB fix_bare_chip_id

prod_file_DB: prod_file_DB.cpp
	g++ -o bin/prod_file_DB prod_file_DB.cpp

fix_bare_chip_id: fix_bare_chip_id.cpp
	g++ -o bin/fix_bare_chip_id -lboost_program_options $(shell root-config --libs) fix_bare_chip_id.cpp
