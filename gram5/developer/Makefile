default: all
gram5DeveloperGuide.pdf: index.fo
PDF_FILES=gram5DeveloperGuide.pdf

# This rule is used to create docbook files from the Pod documentation
# in the perl module
ifneq (,$(shell which pod2docbook 2>/dev/null))
%.xml: %.sgml
	echo '<!DOCTYPE reference PUBLIC "-//OASIS//DTD DocBook XML V4.5//EN" "http://www.oasis-open.org/docbook/xml/4.5/docbookx.dtd">' > $@
	sed -e 1d -e 's/><anchor//' -e s'/<refentry>/<refentry id="Globus::GRAM::$(shell basename $*)">/' < $< >> $@

%.sgml: %.pm
	pod2docbook -doctype=refentry -title Globus::GRAM::$(shell basename $*) $< > $@
endif

# We can generate this from the fragments included in the tutorial so that
# we can provide a download link for it.
fake.pm: fake-prologue.pm fake-config.pm fake-constructor.pm fake-submit.pm fake-poll.pm fake-cancel.pm fake-epilogue.pm
	cat $+ > $@


TOPDIR=../..
include $(TOPDIR)/rules.mk
