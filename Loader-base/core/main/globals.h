#ifndef __GLOBALS_H__
#define __GLOBALS_H__

/* Store global data */
namespace G
{
	/* Store user data (login...) */
	struct user_t
	{
		std::string szUsername{ }; 
		std::string szPassword{ };
		std::string szHWID{ };
	};
	inline user_t user;

	/* Store client data (version...) */
	struct client_t
	{

	};
	inline client_t client;
}

#endif // #ifndef __GLOBALS_H__
