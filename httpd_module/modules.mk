mod_signal_interceptor.la: mod_signal_interceptor.slo signal_interceptor.slo
	$(SH_LINK) $(LIBS) -rpath $(libexecdir) -module -avoid-version  mod_signal_interceptor.lo signal_interceptor.lo
DISTCLEAN_TARGETS = modules.mk
shared =  mod_signal_interceptor.la
