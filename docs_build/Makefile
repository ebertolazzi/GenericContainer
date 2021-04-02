INDEX = ./sphinx/_build/html/genindex.html

build:
	@rm -rf xml-c
	@rm -rf xml-cpp
	@cd sphinx_c;    make clean; make html; cd ..
	@cd sphinx_cpp;  make clean; make html; cd ..
	@cd sphinx_json; make clean; make html; cd ..
	@cp -rf sphinx_c/api-c       sphinx
	@cp -rf sphinx_cpp/api-cpp   sphinx
	@cp -rf sphinx_json/api-json sphinx
	@ruby xml_merge.rb
	@cd sphinx; make clean; make html; cd ..
	@make install

install:
	@ruby filter.rb
	@cd sphinx; rm -rf ../../docs/*; cp -a _build/html/. ../../docs; cd ..

clean:
	@cd sphinx; make clean; cd ..
