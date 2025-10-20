# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line, and also
# from the environment for the first two.
MAKE          = /usr/bin/make
SPHINXOPTS    ?=
SPHINXBUILD   ?= sphinx-build
DOXYDIR       = ../lib/doc
REPODIR	      = ..
README        = README.md
SOURCEDIR     = .
BUILDDIR      = _build
DOCSDIR	      = ../docs

# Put it first so that "make" without argument is like "make help".
help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
%: Makefile

	cd $(DOXYDIR) && $(MAKE)
	[ ! -e $(README) ] && ln -s $(REPODIR)/$(README) $(README) || true
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
	[ -e $(BUILDDIR)/html ] &&  cp -aR $(BUILDDIR)/html/* $(DOCSDIR) || true
