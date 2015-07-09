all: bin/prod_file_DB bin/fix_bare_chip_id bin/*.sh bin/merge_bare_module_tests.sh bin/run_moreweb.sh bin/run_pxar.sh 

bin/prod_file_DB: prod_file_DB.cpp
	@mkdir -p bin
	g++ -o bin/prod_file_DB prod_file_DB.cpp

bin/fix_bare_chip_id: fix_bare_chip_id.cpp
	@mkdir -p bin
	g++ -std=c++11 -o bin/fix_bare_chip_id -L${BOOST_BASE}/lib -lboost_program_options $(shell root-config --libs) fix_bare_chip_id.cpp

bin/*.sh: *.sh
	@mkdir -p bin
	cp *.sh bin/

