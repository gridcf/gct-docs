default: all
gram5DeveloperGuide.pdf: index.txt
PDF_FILES=gram5DeveloperGuide.pdf

# We can generate this from the fragments included in the tutorial so that
# we can provide a download link for it.
fake.pm: fake-prologue.pm fake-config.pm fake-constructor.pm fake-submit.pm fake-poll.pm fake-cancel.pm fake-epilogue.pm
	cat $+ > $@


TOPDIR=../..
include $(TOPDIR)/rules.mk
