#include <string>
#include <iostream>

#include <winsock2.h>
#include <iphlpapi.h>

#include <conio.h>
#include <icmpapi.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

using namespace std;

int main(int argc, char **argv) 
{
	string status;
	char * ip_address = "4.31.198.44";

	char request_data[32] = "Request data";

	DWORD dw_ret = 0;
	DWORD reply_size = 0;

	LPVOID reply_buffer = NULL;
	
	HANDLE icmp_handle;

	unsigned long ip_address_long = inet_addr(ip_address);

	cout << "Ip-address of portal ietf.org :" << ip_address << endl;

	icmp_handle = IcmpCreateFile();
	if (icmp_handle == INVALID_HANDLE_VALUE) 
	{
		cout << "Unable to create handle!" << endl;
		cout << "Error: " << GetLastError() << endl;

		return 1;
	}

	reply_size = sizeof(ICMP_ECHO_REPLY) + sizeof(request_data);
	reply_buffer = (void *)malloc(reply_size);

	if (reply_buffer == NULL) 
	{
		cout << "Unable to allocate memory" << endl;

		return 1;
	}

	ip_option_information ip_options_info; //info to be included in the header of an IP packet

	cout << "Sent ICMP message to " << ip_address << endl;
	for (int ttl = 0; ttl < 128; ttl++)
	{
		ip_options_info.Ttl = ttl;
		ip_options_info.Tos = 0;
		ip_options_info.Flags = 0;
		ip_options_info.OptionsSize = 0;
		ip_options_info.OptionsData = NULL;

		dw_ret = IcmpSendEcho(icmp_handle, ip_address_long, request_data, sizeof(request_data), &ip_options_info, reply_buffer, reply_size, 5000);

		PICMP_ECHO_REPLY echo_reply = (PICMP_ECHO_REPLY)reply_buffer;
		in_addr reply_address; reply_address.S_un.S_addr = echo_reply->Address;

		cout << endl;
		cout << "------------------------------------------" << endl;
		cout << "TTL = " << (int)ip_options_info.Ttl << endl;

		cout << "-Received from " << inet_ntoa(reply_address) << endl;

		if (echo_reply->Status == IP_SUCCESS)
			status = "Success!";
		else if (echo_reply->Status == IP_TTL_EXPIRED_TRANSIT)
			status = "The TTL expired in transit!";
		else if (echo_reply->Status == IP_REQ_TIMED_OUT)
			status = "The request timed out!";
		else
			status = echo_reply->Status;
		 
		cout << "-Status = " << status << endl;
		cout << "-Roundtrip time = " << echo_reply->RoundTripTime << " milliseconds" << endl;

		if (echo_reply->Status == IP_SUCCESS) 
			break;
	}
	_getch();

	return 0;
}