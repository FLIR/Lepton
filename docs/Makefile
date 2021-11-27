# To build docs, pandoc must be installed along with latex support.
# If pandoc is installed but one or more latex packages are missing,
# pandoc will quit with errors.
#
# To install pandoc and latex on ubuntu, try following command:
#   sudo apt install pandoc texlive-latex-base texlive-latex-recommended texlive-latex-extra

SRC = design_doc_and_porting_guide.md \
      BeagleBoneBlackGuide.md \
      RaspberryPiGuide.md

PDF = $(SRC:.md=.pdf)
HTML = $(SRC:.md=.html)

all: $(PDF) $(HTML)

%.pdf: %.md
	pandoc -o $@ $<

%.html: %.md
	pandoc -o $@ $<

clean:
	-rm $(PDF) $(HTML)
