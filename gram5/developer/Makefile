default: all
gram5DeveloperGuide.pdf: index.fo
PDF_FILES=gram5DeveloperGuide.pdf

# This rule is used to create docbook files from the Pod documentation
# in the perl module
ifneq (,$(shell which pod2docbook))
%.xml: %.sgml
	echo '<!DOCTYPE reference PUBLIC "-//OASIS//DTD DocBook XML V4.4//EN" "http://www.oasis-open.org/docbook/xml/4.4/docbookx.dtd">' > $@
	sed -e 1d -e 's/><anchor//' -e s'/<refentry>/<refentry id="Globus::GRAM::$(shell basename $*)">/' < $< >> $@

%.sgml: %.pm
	pod2docbook -doctype=refentry -title Globus::GRAM::$(shell basename $*) $< > $@
endif

# We can generate this from the fragments included in the tutorial so that
# we can provide a download link for it.
fake.pm: fake-prologue.pm fake-config.pm fake-constructor.pm fake-submit.pm fake-poll.pm fake-cancel.pm fake-epilogue.pm
	cat $+ > $@

# somewhat inverted to above, extract the SEG module from the tutorial
seg_fake_module.c: GRAM5_LRM_Adapter_Tutorial_SEG.xml
	xsltproc extract-seg-module.xsl $<

TOPDIR=../..
include $(TOPDIR)/rules.mk
