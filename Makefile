HASHDIR = module/sparsehash
OPENDP = src

all: hash
		$(MAKE) -C $(OPENDP);

hash:
		echo $(MAKEFLAGS)
		cd $(HASHDIR); \
		mkdir -p install-sp; \
		./configure --prefix=$(CURDIR)/$(HASHDIR)/install-sp; \
		$(MAKE); \
		$(MAKE) install;

clean:
		cd $(HASHDIR) && $(MAKE) distclean && rm -rf install-sp > /dev/null 2>&1; true 
		cd $(OPENDP) && $(MAKE) clean;
