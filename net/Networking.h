#pragma once

#ifdef HAVE_WINSOCK2_H 

#include <WinSock2.h>
#include <Ws2tcpip.h>

struct timezone
{
	long tz_minuteswest;
	long tz_dsttime;
};
typedef unsigned int u_int32_t;

//#define EMSGSIZE		WSAEMSGSIZE

#define NETINIT()			do { WSADATA wsaData; WSAStartup(MAKEWORD(2,2), &wsaData); } while(0);
#define EXIT(s)			do { WSACleanup(); exit( ( s ) ); } \
						while ( 0 )
#define NETCLOSE(s)		CLOSE(s)
#define CLOSE(s)		if ( closesocket( s ) ) \
							netError( 1, errno, "close failed" )
#define errorno			( GetLastError() )
#define set_errno(e)	SetLastError( ( e ) )
#define isvalidsock(s)	( ( s ) != SOCKET_ERROR )
#define bzero(b,n)		memset( ( b ), 0, ( n ) )
#define sleep(t)		Sleep( ( t ) * 1000 )

int inet_aton( const char *cp, struct in_addr *pin );

#endif /* HAVE_WINSOCK2_H */

#define NLISTEN			5		/* max waiting connections */

typedef void ( *tofunc_t )( void * );

#include <string>

namespace gamelib
{

	class Networking
	{
	protected:
		static Networking* Instance;

	public:
		static Networking* Get();
		Networking();

		/// <summary>
		/// Initialize Winsock
		/// </summary>
		/// <returns></returns>
		bool Initialize();

		// Cannot copy an NetworkManager
		Networking(Networking const&) = delete;
	
		~Networking();
		
		// Cannot assign to an NetworkManager
		void operator=(Networking const&) = delete;

	public:
	/** \brief Print a diagnostic and optionally quit
	 *
	 * \param status int status number
	 * \param err int error number
	 * \param fmt char* format of error string
	 * \param ... aditional arguments that match the format string
	 * \return void 
	 *
	 */
	 void netError(int status, int err, std::string error);

	/** \brief Read and wait for len bytes on socket
	 *
	 * \param s SOCKET socket to read data from
	 * \param buf char* address of memeory to store len bytes of data
	 * \param len size_t the length of data to read/wait for 
	 * \return int number of bytes read, or -1 on error 
	 *
	 */
	 int netReadn(SOCKET s, char* buf, size_t len );

	 int netSendVRec(SOCKET fd, const char* data, int dataLength);

	/** \brief Read variable records (expects first data read to be size of remaining data)
	 *
	 * \param s SOCKET socket to read data from
	 * \param buf char* address of memeory to store len bytes of data
	 * \param len size_t the length of data to read/wait for 
	 * \return int number of bytes read, or -1 on error 
	 *
	 */
	 int netReadVRec(SOCKET s, char* buf, size_t len );
	 int netReadcrlf(SOCKET, char *, size_t );
	 int netReadLine(SOCKET, char *, size_t );

	/** \brief Set up for tcp server: get tcp socket, bound to hname:sname and returns socket.
	 *
	 * \param hname char* hostname that we'll bind the socket to
	 * \param sname char* the port number for the socket
	 * \return socket configuured to listen on hname host and sname port 
	 *
	 */
	 SOCKET netTcpServer(const char* hname, const char* sname );

	/** \brief Set up for tcp client socket, then connect to it and return socket
	 *
	 * \param hname char* hostname that we'll bind the socket to
	 * \param sname char* the port number for the socket
	 * \return socket that represents the established connection  
	 *
	 */
	 SOCKET netTcpClient(const char* hname, const char* sname);

	/** \brief Set up for udp server: get udp socket bound to hname:sname
	 *
	 * \param hname char* hostname that we'll bind the socket to
	 * \param sname char* the port number for the socket
	 * \return socket that represents hname host and sname port 
	 *
	 */
	 SOCKET netUdpServer(const char* hname, const char* sname);

	/** \brief Set up for udp client: get a udp socket and fill address to use(this never blocks)
	 *
	 * \param hname char* hostname that we'll bind the socket to
	 * \param sname char* the port number for the socket
	 * \param addr struct sockaddr_in* address to use
	 * \return a raw simple udp socket  
	 *
	 */
	 SOCKET netUdpClient(const char* hname, const char* sname, struct sockaddr_in* sap);

	/** \brief fill in a sockaddr_in structure
	 *
	 * \param hname char* hostname string
	 * \param sname char* port number as string
	 * \param sap struct sockaddr_in* address structure to fill
	 * \param protocol char* the protocol to set in the address structure
	 * \return void the pointer sap will be filled up and available to caller
	 *
	 */
	 void netSetAddress(const char* hname, const char* sname, struct sockaddr_in* sap, const char* protocol);
	 int inet_aton(const char* cp, in_addr* pin);
	};

}
