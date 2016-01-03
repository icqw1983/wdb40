#include <uci_intf.h>


uciIntf::uciIntf() 
{
	Reset();

	// verbosity setting
	SetVerbosity(1);
}

uciIntf::~uciIntf() 
{
	ReleaseBackend();
}


void uciIntf::Reset()
{
	ctx 			= NULL;

	bBackendInitialized 	= 0;
}

void uciIntf::ReleaseBackend()
{
	// free the uci context
	if (ctx != NULL && bBackendInitialized == 1) {
		bBackendInitialized = 0;
		uci_free_context (ctx);
	}
}


int uciIntf::ReadBackend()
{
	int status;

	// initialization of uci context
	ctx 	= uci_alloc_context();

	if (!ctx) {
		return EXIT_FAILURE;
	}

	bBackendInitialized = 1;
	return EXIT_SUCCESS;
}

int uciIntf::ReadWirelessConfig()
{
	int 	status;

	char *config 	= strdup("wireless");

	// lookup the wireless config
	if ( uci_lookup_ptr(ctx, &wirelessPtr, config, true) != UCI_OK ) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int uciIntf::ProcessConfigData()
{
	int 	status = EXIT_FAILURE;
	
	struct uci_element 	*e;
	struct uci_section 	*s;
	struct uci_option 	*o;
	
	// ensure that config has been read
	if (wirelessPtr.target == UCI_TYPE_PACKAGE) {
		_Print(1, "Type:    %d\n", 		(int)wirelessPtr.target );
		_Print(1, "Package:  %s\n", 	wirelessPtr.p->e.name );
		_Print(1, "Sections:  \n");
		
		// look through each section
		uci_foreach_element( &(wirelessPtr.p->sections), e ) {
			if (e->type == UCI_TYPE_SECTION) {
				// grab a pointer to the section
				s = uci_lookup_section(ctx, wirelessPtr.p, e->name);

				// only look at wifi network setup
				if ( strncmp(s->type, UCI_INTF_WIFI_IFACE, strlen(UCI_INTF_WIFI_IFACE) ) == 0) {
					// lookup the ssid name
					//o = (ctx, s, UCI_INTF_WIFI_IFACE_OPT_SSID);
					_Print(1, "  section: %s\n", e->name);
					_Print(1, "    ssid:     %s\n", uci_lookup_option_string(ctx, s, UCI_INTF_WIFI_IFACE_OPT_SSID) );
					_Print(1, "    encr:     %s\n", uci_lookup_option_string(ctx, s, UCI_INTF_WIFI_IFACE_OPT_ENCRYPTION) );
					_Print(1, "    disabled: %s\n", uci_lookup_option_string(ctx, s, UCI_INTF_WIFI_IFACE_OPT_DISABLED) );
				}
			}
		}
		
		status 	=  EXIT_SUCCESS;
	}

	

	return status;
}
