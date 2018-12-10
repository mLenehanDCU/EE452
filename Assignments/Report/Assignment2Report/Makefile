.PHONY: typeset

typeset:
	pandoc                          \
	  --from         markdown       \
	  --to           latex          \
	  --template     template.tex   \
	  --out          EE452Assignment2Report.pdf \
	  --latex-engine   xelatex        \
	  --bibliography biblio.bib \
	  --csl ieee.csl \
	  Assignment2.md
