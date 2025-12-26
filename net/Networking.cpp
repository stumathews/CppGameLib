#include "pch.h"
#include "Networking.h"
#include <sstream>
#include <file/Logger.h>
#include <sstream>
#include <string>
#include <cstring>
#include <memory>

namespace gamelib
{
	Networking* Networking::Get()
	{
		if (Instance == nullptr)
		{
			Instance = new Networking();
		}
		return Instance;
	
	}

	Networking* Networking::Instance = nullptr;

	Networking::Networking()
	= default;


	bool Networking::InitializeWinSock()
	{
		Logger::Get()->LogThis("Initializing Winsock2");

		NETINIT();
		return true;
	}

	Networking::~Networking()
	{

		Instance = nullptr;
	}

	

	void Networking::netError(const int status, int err, const std::string &error)
    {
		std::stringstream errorMessage;

		errorMessage << "Networking Error: " << getLastSocketError() << "\n";

		Logger::Get()->LogThis(errorMessage.str(), true);
    }

    int Networking::netReadn(const SOCKET fd, char* bp, const size_t len)
    {
		int cnt;
		int rc;

		cnt = len;
		while ( cnt > 0 )
		{
			rc = recv( fd, bp, cnt, 0 );
			if ( rc < 0 )				/* read error? */
			{
				if ( errno == EINTR )	/* interrupted? */
					continue;			/* restart the read */
				return -1;				/* return error */
			}
			if ( rc == 0 )				/* EOF? */
				return len - cnt;		/* return short count */
			bp += rc;
			cnt -= rc;
		}
		return len;
	}

	int Networking::netSendVRec(const SOCKET fd, const char* data, const int dataLength)
	{
		int length = htonl(dataLength);

		// Send length of data
		send(fd, (char*) &length, sizeof(u_int32_t), 0);

		// Send the data
		const int sendResult = send(fd, data, static_cast<int>(strlen(data)), 0);

		return sendResult;
	}

    int Networking::netReadVRec(const SOCKET fd, char* bp, size_t len)
    {
		u_int32_t reclen;
		int rc;

		/* Retrieve the length of the record */

		rc = netReadn( fd, ( char * )&reclen, sizeof( u_int32_t ) );
		if ( rc != sizeof( u_int32_t ) )
			return rc < 0 ? -1 : 0;
		reclen = ntohl( reclen );
		if ( reclen > len )
		{
			/*
			 *  Not enough room for the record--
			 *  discard it and return an error.
			 */

			while ( reclen > 0 )
			{
				rc = netReadn( fd, bp, len );
				if ( rc != len )
					return rc < 0 ? -1 : 0;
				reclen -= len;
				if ( reclen < len )
					len = reclen;
			}
			set_errno( EMSGSIZE );
			return -1;
		}

		/* Retrieve the record itself */

		rc = netReadn( fd, bp, reclen );
		if ( rc != reclen )
			return rc < 0 ? -1 : 0;
		return rc;
	}
    int Networking::netReadcrlf(const SOCKET s, char* buf, size_t len)
	{
		const char *bufx = buf;
		int rc;
		char c;
		char lastc = 0;

		while ( len > 0 )
		{
			if ( ( rc = recv( s, &c, 1, 0 ) ) != 1 )
			{
				/*
				 *  If we were interrupted, keep going,
				 *  otherwise, return EOF or the error.
				 */

				if ( rc < 0 && errno == EINTR )
					continue;
				return rc;
			}
			if ( c == '\n' )
			{
				if ( lastc == '\r' )
					buf--;
				*buf = '\0';			/* don't include <CR><LF> */
				return buf - bufx;
			}

			*buf++ = c;
			lastc = c;
			len--;
		}
		set_errno( EMSGSIZE );
		return -1;
	}

    int Networking::netReadLine(const SOCKET fd, char* bufptr, size_t len)
    {
		const char *bufx = bufptr;
		static char *bp;
		static int cnt = 0;
		static char b[ 1500 ];
		char c;

		while ( --len > 0 )
		{
			if ( --cnt <= 0 )
			{
				cnt = recv( fd, b, sizeof( b ), 0 );
				if ( cnt < 0 )
				{
					if ( errno == EINTR )
					{
						len++;		/* the while will decrement */
						continue;
					}
					return -1;
				}
				if ( cnt == 0 )
					return 0;
				bp = b;
			}
			c = *bp++;
			*bufptr++ = c;
			if ( c == '\n' )
			{
				*bufptr = '\0';
				return bufptr - bufx;
			}
		}
		set_errno( EMSGSIZE );
		return -1;
	}
    SOCKET Networking::netTcpServer(const char* hname, const char* sname)
    {
		// This will hold the address that the server will use to listen on
		struct sockaddr_in local{};
		// This is the socket the server will obtain and bind it to the address in the sockaddr_in structure
		SOCKET s;
		constexpr int on = 1;

		// Construct the address : fill in the sockaddr_in structure
		netSetAddress( hname, sname, &local, "tcp" );
    
		// Obtain a socket for the server
		s = socket( AF_INET, SOCK_STREAM, 0 );
		if ( !isvalidsock( s ) )
		{
			netError( 1, errno, "socket call failed" );
		}
    
		// Configure the socket so we can use it as a server socket
		if ( setsockopt( s, SOL_SOCKET, SO_REUSEADDR, ( char * )&on, sizeof( on ) ) )
		{
			netError( 1, errno, "setsockopt failed" );
		}

		// Bind the address to the socket
		if ( bind( s, ( struct sockaddr * ) &local, sizeof( local ) ) )
		{
			netError( 1, errno, "bind failed" );
		}

		// set listen state  on the socket, which is now bound to the address mentioned in sockaddr_in
		// Note: This only sets the socket state to listening, this doesn't block;
		if ( listen( s, NLISTEN ) )
		{
			netError( 1, errno, "listen failed" );
		}

		// Socket  
		return s;
	}
    SOCKET Networking::netTcpClient(const char* hname, const char* sname)
    {
		struct sockaddr_in peer{};
		SOCKET s;

		netSetAddress( hname, sname, &peer, "tcp" );
		// obtain a socket
		s = socket( AF_INET, SOCK_STREAM, 0 );
		if ( !isvalidsock( s ) )
			netError( 1, errno, "socket call failed" );
    
		// Connect returns on a successfull connection and thereafter socket can be used 
		// as an established connection to send/recv network data
		
		if ( connect( s, ( struct sockaddr * )&peer,sizeof( peer ) ) )		
		{
			netError( 1, errno, "connect failed" );
		}

		return s;
	}
    SOCKET Networking::netUdpServer(const char* hname, const char* sname)
    {
		SOCKET s;
		struct sockaddr_in local{};

		netSetAddress( hname, sname, &local, "udp" );
		s = socket( AF_INET, SOCK_DGRAM, 0 );
		if ( !isvalidsock( s ) )
			netError( 1, errno, "socket call failed" );
		if ( bind( s, ( struct sockaddr * ) &local,
			 sizeof( local ) ) )
			netError( 1, errno, "bind failed" );
		return s;
}
    SOCKET Networking::netUdpClient(const char* hname, const char* sname, sockaddr_in* sap)
    {    
	    SOCKET s;

	    netSetAddress( hname, sname, sap, "udp" );
	    s = socket( AF_INET, SOCK_DGRAM, 0 );
	    if ( !isvalidsock( s ) )
		    netError( 1, errno, "socket call failed" );
	    return s;
    }

	SOCKET Networking::netConnectedUdpClient(const char* hname, const char* sname)
    {    
		struct sockaddr_in sap{};
	    const SOCKET s = netUdpClient(hname, sname, &sap);
		if (connect(s, (struct sockaddr * )&sap, sizeof(sap)))		
		{
			netError( 1, errno, "connect failed" );
		}	
		return s;
    }

    void Networking::netSetAddress(const char* hname, const char* sname, sockaddr_in* sap, const char* protocol)    
    {
	    struct servent *sp;
	    std::unique_ptr<hostent> hp;
	    char *endptr;
	    short port;

	    bzero( sap, sizeof( *sap ) );
	    sap->sin_family = AF_INET;
	    if ( hname != nullptr)
	    {
		    if ( !inet_aton( hname, &sap->sin_addr ) )
		    {
			    hp = my_gethostbyname(hname);
			    if ( hp == nullptr)
				{				
					std::stringstream message;
					message << "unknown host:" << hname;
					netError( 1, 0, message.str().c_str() );
				}
			    sap->sin_addr = *( struct in_addr * )hp->h_addr;
		    }
	    }
	    else
		    sap->sin_addr.s_addr = htonl( INADDR_ANY );
	    port = strtol( sname, &endptr, 0 );
	    if ( *endptr == '\0' )
		    sap->sin_port = htons( port );
	    else
	    {
		    sp = getservbyname( sname, protocol );
		    if ( sp == nullptr)
			{
				std::stringstream message;
				message << "unknown service:" << sname;
				netError( 1, 0, message.str().c_str() );
			}
		    sap->sin_port = sp->s_port;
	    }
    }

	int Networking::inet_aton( const char *cp, struct in_addr *pin )
	{
		int rc;
		rc = inet_addr( cp );
    
		if ( rc == -1 && strcmp( cp, "255.255.255.255" ) )
			return 0;
		pin->s_addr = rc;
		 return 1;
	}
}
