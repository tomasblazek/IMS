all: pdf

pdf: documentation.tex
	pdflatex documentation.tex
	bibtex documentation.aux
	pdflatex documentation.tex
	pdflatex documentation.tex

	rm documentation.aux
	rm documentation.blg
	rm documentation.bbl
	rm documentation.log
	rm documentation.toc

clean:
	rm documentation.aux
	rm documentation.blg
	rm documentation.bbl
	rm documentation.log
	rm documentation.toc
