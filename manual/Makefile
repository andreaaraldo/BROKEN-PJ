all:
	pdflatex manual.tex
	bibtex manual
	pdflatex manual.tex
	pdflatex manual.tex

show: all
	evince manual.pdf
