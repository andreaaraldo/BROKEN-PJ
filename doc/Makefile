MAIN=manual


all: pdf 

show:
	evince $(MAIN).pdf&

clean:
	rm $(MAIN).bbl $(MAIN).aux $(MAIN).blg $(MAIN).log $(MAIN).dvi
	
#===============================================
#  cr: camera ready
#-----------------------------------------------
CR:	
	check_latex.pl -cr $(MAIN)
	
cr:
	# Embeds the font, ok for:
	# 	ACM SIGCOMM SHERIDAN PRINTING
	dvips -t letter -P cmz $(MAIN)
	ps2pdf -sPAPERSIZE=letter $(MAIN).ps

crA4:
	# Embeds the font, ok for:
	# 	ACM SIGCOMM SHERIDAN PRINTING
	dvips -P cmz $(MAIN)
	ps2pdf   $(MAIN).ps

ps2pdf:
	# Embeds the font, ok for:
	#	FOR IEEE PDF EXPRESS
	ps2pdf14 -dPDFSETTINGS=/prepress -dEmbedAllFonts=true  -dSubsetFonts=true $(MAIN).ps

show: all
	evince $(MAIN).pdf
#===============================================
#  dvis, ps, pdf: the usual stuff
#  anyway: even in case of errors
#-----------------------------------------------
bib: bibtex

bibtex:
	echo R | latex $(MAIN).tex  ; bibtex $(MAIN) ;

anyway:
	latex $(MAIN).tex  ; bibtex $(MAIN)   ; latex $(MAIN).tex ;  latex $(MAIN).tex ; dvips -t A4 $(MAIN) -o 

force:
	echo R | latex $(MAIN).tex  ; bibtex $(MAIN)   ; echo R | latex $(MAIN).tex ;  echo R | latex $(MAIN).tex ; dvips -t A4 $(MAIN) -o 

pdf:
	pdflatex -shell-escape $(MAIN).tex  ; bibtex $(MAIN) ; pdflatex -shell-escape $(MAIN).tex ; pdflatex  -shell-escape  $(MAIN).tex  
	


dvi: *.tex
	latex $(MAIN).tex && bibtex $(MAIN).tex && latex $(MAIN).tex && latex $(MAIN).tex 

ps: dvi
	dvips -t A4 $(MAIN) -o 

ps2: dvi
	mpage -2 $(MAIN).ps >  $(MAIN)-2pages.ps

#===============================================
#  gp: figures
#-----------------------------------------------

fig: gp
plot: gp
gp: 
	(cd plot; xgp -ttcps *gp && mv *eps ../eps)

#===============================================
# check and revision
#-----------------------------------------------

check:
	perl check_latex.pl $(MAIN) 

grammar:
	which qq || { echo "You need to install Queequeg (a tiny english grammar checker), see http://queequeg.sourceforge.net/index-e.html" }
	which qq && { qq $(MAIN).tex }

diff:
	latexdiff  $(PREV).tex $(MAIN).tex > diff.tex
	latex diff && latex diff && dvips diff -o

difff:
	echo R | latex diff ; echo R | latex diff;  dvips diff -o

	
	
