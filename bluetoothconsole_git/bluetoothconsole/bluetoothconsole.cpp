// bluetoothconsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <process.h>

#include <winsock2.h>
#include <ws2bth.h>
#include <BluetoothAPIs.h>
#include <cstdio>

using namespace std;
#define DEFAULT_BUFLEN 1024
typedef ULONGLONG bt_addr, *pbt_addr, BT_ADDR, *PBT_ADDR;
bool threadFinished = false;
bool dataReady = false;
char buf[128];
unsigned char sendbuf[512];
int sendLen = 0;
bool left_side = true;
int process_buff(unsigned char * in, char * msg)
{
	unsigned char * p = in;
	unsigned char header[] = { 0xff, 0x1, 0x0, 0x00};

	memcpy(p, header, sizeof(header));
	p += sizeof(header);
	memcpy(p, msg, strlen(msg));
	return sizeof(header) + strlen(msg);
}

void console(void *param)
{
	while (1)
	{

		fgets(buf, 128, stdin);
		int ret = strncmp(buf, "quit", 4);
		if (ret == 0)
		{
			threadFinished = true;
			exit(0);

		}

		sendLen = process_buff(sendbuf, buf);
		dataReady = true;

	}

}
BLUETOOTH_FIND_RADIO_PARAMS m_bt_find_radio = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS) };

BLUETOOTH_RADIO_INFO m_bt_info = { sizeof(BLUETOOTH_RADIO_INFO), 0, };

BLUETOOTH_DEVICE_SEARCH_PARAMS m_search_params = { sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS), 0, 0, 0, 1, 0, 15, NULL };

BLUETOOTH_DEVICE_INFO m_device_info = { sizeof(BLUETOOTH_DEVICE_INFO), 0, };
ULONGLONG nu_bt_addr = 0;

int get_connection_info()
{
	HANDLE m_radio = NULL;

	HBLUETOOTH_RADIO_FIND m_bt = NULL;

	HBLUETOOTH_DEVICE_FIND m_bt_dev = NULL;

	int m_radio_id;

	int m_device_id;

	DWORD mbtinfo_ret;

	// Iterate for available bluetooth radio devices in range

	// Starting from the local

	while (TRUE)

	{
		m_bt = BluetoothFindFirstRadio(&m_bt_find_radio, &m_radio);

		if (m_bt == NULL)
		{
			printf("BluetoothFindFirstRadio() failed with error code %d\n", GetLastError());
		}
		m_radio_id = 0;
		do
		{

			// Then get the radio device info....

			mbtinfo_ret = BluetoothGetRadioInfo(m_radio, &m_bt_info);

			m_search_params.hRadio = m_radio;

			ZeroMemory(&m_device_info, sizeof(BLUETOOTH_DEVICE_INFO));
			m_device_info.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
			// Next for every radio, get the device
			m_bt_dev = BluetoothFindFirstDevice(&m_search_params, &m_device_info);
			if (m_bt_dev == NULL)
			{
				printf("\nBluetoothFindFirstDevice() failed with error code %d\n", GetLastError());
			}
			m_radio_id++;
			m_device_id = 0;
			// Get the device info

			do
			{
				/*change the Bluetooth friendly name here*/
				if (wcsncmp(m_device_info.szName, L"MYheadset", 9) == 0)
				{
					nu_bt_addr = m_device_info.Address.ullLong;
					wprintf(L"Found headset %ls Addr=%llx\n", m_device_info.szName, nu_bt_addr);
					break;
				}
				m_device_id++;
				if (!BluetoothFindNextDevice(m_bt_dev, &m_device_info))

					break;

			} while (BluetoothFindNextDevice(m_bt_dev, &m_device_info));

			// NO more device, close the device handle

			if (BluetoothFindDeviceClose(m_bt_dev) != TRUE)
			{
				printf("\nBluetoothFindDeviceClose(m_bt_dev) failed with error code %d\n", GetLastError());
			}

		} while (BluetoothFindNextRadio(&m_bt_find_radio, &m_radio));

		// No more radio, close the radio handle

		if (BluetoothFindRadioClose(m_bt) != TRUE)
		{
			printf("BluetoothFindRadioClose(m_bt) failed with error code %d\n", GetLastError());
		}

		
		if (!BluetoothFindNextRadio(&m_bt_find_radio, &m_radio))

			break;

		
		Sleep(1000);

	}

	return 0;
}

int main(int argc, char **argv)
{
	WSADATA wsd;
	SOCKET s;
	SOCKADDR_BTH sab;
	int iResult;



	get_connection_info();

	BT_ADDR aSddr = (BT_ADDR)nu_bt_addr;

	int recvbuflen = DEFAULT_BUFLEN;
	// Change the type accordingly for non-char data
	char recvbuf[DEFAULT_BUFLEN] = "";
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		printf("Unable to load Winsock! Error code is %d\n", WSAGetLastError());
		return 1;
	}

	s = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if (s == INVALID_SOCKET)
	{
		printf("Socket creation failed, error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	memset(&sab, 0, sizeof(sab));
	sab.addressFamily = AF_BTH;

	sab.btAddr = aSddr;

	sab.port = 1;
	do
	{

		// Connect to the Bluetooth socket, created previously
		if (connect(s, (SOCKADDR *)&sab, sizeof(sab)) == SOCKET_ERROR)
		{
			//printf("connect() failed with error code %d\n", WSAGetLastError());

		}
		else
		{
			//printf("connect() should be fine!\n");
			break;
		}
		sab.port++;

	} while (sab.port < 128);

	_beginthread(console, 0, NULL);

	int count = 0;
	do
	{
		unsigned char * sb;
		int len;
		if (dataReady)
		{
			sb = sendbuf;
			len = sendLen;
			dataReady = false;
			//printf("send msg, %d\n", len);
		}

		iResult = send(s, (const char *)sb, len, 0);
		if (iResult == SOCKET_ERROR)
		{
			printf("send() failed with error code %d\n", WSAGetLastError());
			closesocket(s);
			WSACleanup();
			return 1;
		}
	

		memset(recvbuf, 0, sizeof(recvbuf));
		//do
		{
			iResult = recv(s, recvbuf, recvbuflen, 0);
			if (iResult > 0)
			{
				//printf(" %d Bytes received from sender\n", iResult);

				printf("%s", &recvbuf[9]);
			}
			else if (iResult == 0)
				printf("Connection was closed by peer!\n");
			else
				printf("recv() failed with error code %d\n", WSAGetLastError());
		}            // while (iResult>0);

		Sleep(10);
	} while (!threadFinished);

	// Do all the cleanup
	if (closesocket(s) == 0)
	{
		if (WSACleanup() == 0)
		{

		}
	}

	return 0;
}

