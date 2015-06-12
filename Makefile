default: all

PDF_FILES = index.pdf
HTML_FILES = index.html Java_API_Download.html Usage_Stats.html
SUBDIRS= \
admin       gram5    gsiopenssh  simpleca \
appendices  gridftp  myproxy     xio \
ccommonlib  gsic     rn

index.pdf: index.fo
FO_FILES = target.db
target.db: index.xml


TOPDIR=.
include $(TOPDIR)/rules.mk
