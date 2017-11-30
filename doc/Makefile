all: pdf

pdf: documentation.tex
	latex documentation.tex
	bibtex documentation.aux
	latex documentation.tex
	latex documentation.tex
	dvips documentation.dvi
	ps2pdf documentation.ps

	rm documentation.aux
	rm documentation.dvi
	rm documentation.ps
	rm documentation.blg
	rm documentation.bbl
	rm documentation.log
	rm documentation.toc

clean:
	rm documentation.aux
	rm documentation.dvi
	rm documentation.ps
	rm documentation.blg
	rm documentation.bbl
	rm documentation.log
	rm documentation.toc
