all: prod_file_DB fix_bare_chip_id

prod_file_DB: prod_file_DB.cpp
	g++ -o bin/prod_file_DB prod_file_DB.cpp

fix_bare_chip_id: fix_bare_chip_id.cpp
	g++ -std=c++11 -o bin/fix_bare_chip_id -L${BOOST_BASE}/lib -lboost_program_options $(shell root-config --libs) fix_bare_chip_id.cpp
