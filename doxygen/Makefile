DOXYFILE = doxyfile_GC

.PHONY: help clean html doxygen doxysphinx sphinx

clean:
	@rm -rf docs*

html:
	@doxygen $(DOXYFILE)

install:
	@rm -rf ../docs*
	@mkdir ../docs
	@cp -r docs/html/* ../docs
	@touch ../docs/.nojekyll
