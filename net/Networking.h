#pragma once

#ifdef __linux__

//linux code goes here:

#include <arpa/inet.h>
#include <cerrno>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>

#define INIT()			( program_name = \
							strrchr( argv[ 0 ], '/' ) ) ? \
							program_name++ : \
							( program_name = argv[ 0 ] )
#define NETINIT()			do { } while(0)
//#define EXIT(s)			exit( s )
#define NETCLOSE(s)		CLOSE(s)
#define CLOSE(s)                                              \
      do {                                                    \
          if (close(s) != 0) {                                \
              perror("close");                                \
          }                                                   \
      } while (0)
#define set_errno(e)	errno = ( e )
#define isvalidsock(s)	( ( s ) >= 0 )
#define SOCKET_ERROR (-1)
#define WSAGetLastError() errno
#define WSAECONNRESET ECONNRESET
#define SD_SEND SHUT_WR
#define SD_RECEIVE SHUT_RD
#define SD_BOTH SHUT_RDWR

	typedef int SOCKET;

	inline int getLastSocketError() 
	{
	    return errno;
	}
	
	using SocketHandle = int;
    constexpr SocketHandle INVALID_SOCKET_HANDLE = -1;
	constexpr int SHUT_SEND = SHUT_WR;
#define ZeroMemory(dest, size) std::memset(dest, 0, size)

#elif _WIN32

// Windows code goes here:

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <afunix.h>
#pragma comment(lib, "Ws2_32.lib")
using socklen_t = int;

	struct timezone
	{
		long tz_minuteswest;
		long tz_dsttime;
	};
	typedef unsigned int u_int32_t;

	inline int getLastSocketError() 
	{
    		return WSAGetLastError();
	}

	using SocketHandle = SOCKET;
    	constexpr SocketHandle INVALID_SOCKET_HANDLE = INVALID_SOCKET;
	constexpr int SHUT_SEND = SD_SEND;

	//#define EMSGSIZE		WSAEMSGSIZE

#define NETINIT()		do { WSADATA wsaData; WSAStartup(MAKEWORD(2,2), &wsaData); } while(0)
#define EXIT(s)			do { WSACleanup(); } while(0)
#define SOCKET_ERROR 		(-1)
#define NETCLOSE(s)		CLOSE(s)
#define errorno			( GetLastError() )
#define CLOSE(s)                                              \
      do {                                                    \
          if (closesocket(s) == SOCKET_ERROR) {               \
              printf(                            \
                  "closesocket failed: %d\n",                 \
                  WSAGetLastError());                         \
          }                                                   \
      } while (0)
#define set_errno(e)	SetLastError( ( e ) )
#define isvalidsock(s)	( ( s ) != SOCKET_ERROR )
#define bzero(b,n)		memset( ( b ), 0, ( n ) )
#define sleep(t)		Sleep( ( t ) * 1000 )

	int inet_aton( const char *cp, struct in_addr *pin );

#endif

#define NLISTEN			5		/* max waiting connections */

	typedef void ( *tofunc_t )( void * );

#include <string>
#include <cstring>
#include <memory>
#include <vector>

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
		static bool InitializeWinSock();

		// Cannot copy an NetworkManager
		Networking(Networking const&) = delete;
	
		~Networking();
		
		// Cannot assign to an NetworkManager
		void operator=(Networking const&) = delete;

		static std::unique_ptr<hostent> my_gethostbyname(const char* hostname) {
		    struct addrinfo hints{};
		    struct addrinfo* res = nullptr;

		    hints.ai_family = AF_INET;        // IPv4
		    hints.ai_socktype = SOCK_STREAM;

		    const int status = getaddrinfo(hostname, nullptr, &hints, &res);
		    if (status != 0 || !res) {
			return nullptr;
		    }

		    // Count addresses
		    int count = 0;
		    for (const struct addrinfo* p = res; p != nullptr; p = p->ai_next) count++;

		    // Allocate hostent
		    hostent* he = new hostent{};
		    he->h_name = const_cast<char*>(hostname);

		    static std::vector<char*> aliases; // empty for now
		    aliases.clear();
		    aliases.push_back(nullptr);
		    he->h_aliases = aliases.data();

		    static std::vector<char*> addrs;
		    addrs.clear();

		    for (const struct addrinfo* p = res; p != nullptr; p = p->ai_next) {
			struct sockaddr_in* ipv4 = reinterpret_cast<struct sockaddr_in *>(p->ai_addr);
			addrs.push_back((char*)&(ipv4->sin_addr));
		    }
		    addrs.push_back(nullptr);
		    he->h_addr_list = addrs.data();
		    he->h_addrtype = AF_INET;
		    he->h_length = sizeof(struct in_addr);

		    freeaddrinfo(res);
		    return std::unique_ptr<hostent>(he);
		}
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
		static void netError(int status, int err, const std::string &error);

	/** \brief Read and wait for len bytes on socket
	 *
	 * \param s SOCKET socket to read data from
	 * \param buf char* address of memeory to store len bytes of data
	 * \param len size_t the length of data to read/wait for 
	 * \return int number of bytes read, or -1 on error 
	 *
	 */
		static int netReadn(SOCKET s, char* buf, size_t len );

		static int netSendVRec(SOCKET fd, const char* data, int dataLength);

	/** \brief Read variable records (expects first data read to be size of remaining data)
	 *
	 * \param s SOCKET socket to read data from
	 * \param buf char* address of memeory to store len bytes of data
	 * \param len size_t the length of data to read/wait for 
	 * \return int number of bytes read, or -1 on error 
	 *
	 */
		static int netReadVRec(SOCKET s, char* buf, size_t len );
		static int netReadcrlf(SOCKET, char *, size_t );
		static int netReadLine(SOCKET, char *, size_t );

	/** \brief Set up for tcp server: get tcp socket, bound to hname:sname and returns socket.
	 *
	 * \param hname char* hostname that we'll bind the socket to
	 * \param sname char* the port number for the socket
	 * \return socket configuured to listen on hname host and sname port 
	 *
	 */
		static SOCKET netTcpServer(const char* hname, const char* sname );

	/** \brief Set up for tcp client socket, then connect to it and return socket
	 *
	 * \param hname char* hostname that we'll bind the socket to
	 * \param sname char* the port number for the socket
	 * \return socket that represents the established connection  
	 *
	 */
		static SOCKET netTcpClient(const char* hname, const char* sname);

		static SOCKET netLocalSocketClient(const char *socketPath);

		static SOCKET netLocalSocketServer(const char *socketPath);

		/** \brief Set up for udp server: get udp socket bound to hname:sname
	 *
	 * \param hname char* hostname that we'll bind the socket to
	 * \param sname char* the port number for the socket
	 * \return socket that represents hname host and sname port 
	 *
	 */
		static SOCKET netUdpServer(const char* hname, const char* sname);

	/** \brief Set up for udp client: get a udp socket and fill address to use(this never blocks)
	 *
	 * \param hname char* hostname that we'll bind the socket to
	 * \param sname char* the port number for the socket
	 * \param addr struct sockaddr_in* address to use
	 * \return a raw simple udp socket  
	 *
	 */
		static SOCKET netUdpClient(const char* hname, const char* sname, struct sockaddr_in* sap);

	 // Creates a 'connected' UDP client see: https://www.masterraghu.com/subjects/np/introduction/unix_network_programming_v1.3/ch08lev1sec11.html
	 SOCKET netConnectedUdpClient(const char* hname, const char* sname);

	/** \brief fill in a sockaddr_in structure
	 *
	 * \param hname char* hostname string
	 * \param sname char* port number as string
	 * \param sap struct sockaddr_in* address structure to fill
	 * \param protocol char* the protocol to set in the address structure
	 * \return void the pointer sap will be filled up and available to caller
	 *
	 */
		static void netSetAddress(const char* hname, const char* sname, struct sockaddr_in* sap, const char* protocol);
		static int inet_aton(const char* cp, in_addr* pin);
	};

}
