/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#ifndef _NET_H_
#define _NET_H_

#include "..\game\q_shared.h"
#include "msg.h"

#define	PACKET_BACKUP	32	// number of old messages that must be kept on client and
							// server for delta comrpession and ping estimation
#define	PACKET_MASK	(PACKET_BACKUP-1)
#define	MAX_PACKET_USERCMDS		32		// max number of usercmd_t in a packet
#define	PORT_ANY				-1
#define	MAX_RELIABLE_COMMANDS	64			// max string commands buffered for restransmit

typedef enum
{
	NA_BOT,
	NA_BAD,					// an address lookup failed
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
	NA_IPX,
	NA_BROADCAST_IPX
} netadrtype_t;

typedef enum
{
	NS_CLIENT,
	NS_SERVER
} netsrc_t;

typedef struct
{
	netadrtype_t	type;

	byte	ip[4];
	byte	ipx[10];

	unsigned short	port;
} netadr_t;

void		NET_Init(void);
void		NET_Shutdown(void);
void		NET_Restart(void);
void		NET_Config(qboolean enableNetworking);

void		NET_SendPacket(netsrc_t sock, int length, const void* data, netadr_t to);
void		Q_CDECL NET_OutOfBandPrint(netsrc_t net_socket, netadr_t adr,
	const char* format, ...);
void		Q_CDECL NET_OutOfBandData(netsrc_t sock, netadr_t adr, byte* format, int len);

qboolean	NET_CompareAdr(netadr_t a, netadr_t b);
qboolean	NET_CompareBaseAdr(netadr_t a, netadr_t b);
qboolean	NET_IsLocalAddress(netadr_t adr);
const char* NET_AdrToString(netadr_t a);
qboolean	NET_StringToAdr(const char* s, netadr_t* a);
qboolean	NET_GetLoopPacket(netsrc_t sock, netadr_t* net_from, msg_t* net_message);
void		NET_Sleep(int msec);


#define	MAX_MSGLEN				16384		// max length of a message, which may
// be fragmented into multiple packets

#define MAX_DOWNLOAD_WINDOW		8		// max of eight download frames
#define MAX_DOWNLOAD_BLKSIZE	2048	// 2048 byte block chunks


/*
Netchan handles packet fragmentation and out of order / duplicate suppression
*/

typedef struct
{
	netsrc_t	sock;

	int			dropped;			// between last packet and previous

	netadr_t	remoteAddress;
	int			qport;				// qport value to write when transmitting

	// sequencing variables
	int			incomingSequence;
	int			outgoingSequence;

	// incoming fragment assembly buffer
	int			fragmentSequence;
	int			fragmentLength;
	byte		fragmentBuffer[MAX_MSGLEN];

	// outgoing fragment buffer
	// we need to space out the sending of large fragmented messages
	qboolean	unsentFragments;
	int			unsentFragmentStart;
	int			unsentLength;
	byte		unsentBuffer[MAX_MSGLEN];
} netchan_t;

void Netchan_Init(int qport);
void Netchan_Setup(netsrc_t sock, netchan_t* chan, netadr_t adr, int qport);

void Netchan_Transmit(netchan_t* chan, int length, const byte* data);
void Netchan_TransmitNextFragment(netchan_t* chan);

qboolean Netchan_Process(netchan_t* chan, msg_t* msg);

#endif // #ifndef _NET_H_
