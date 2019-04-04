default: all

PDF_FILES = index.pdf
HTML_FILES = index.html Java_API_Download.html Usage_Stats.html
SUBDIRS= \
admin       gram5    gsiopenssh  simpleca \
appendices  gridftp  myproxy     xio \
ccommonlib  gsic     rn

index.pdf: index.adoc
	$(ADOC_TO_PDF)


TOPDIR=.
include $(TOPDIR)/rules.mk
