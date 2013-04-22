all: 
	pdflatex -shell-escape manual.tex
	bibtex manual
	pdflatex -shell-escape manual.tex
	pdflatex -shell-escape manual.tex
picture:
	xgp -t eps *.gp

show: all
	evince manual.pdf
