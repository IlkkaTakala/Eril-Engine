#pragma once

#define RC_SETUP		0x50
#define RC_RECALCULATE	0x51
#define RC_RELIGHTS		0x52
#define RC_LOADSHADERS	0x53
#define RC_GAMESTART	0x54
#define RC_DESTROY		0x55
#define RC_ACTIVECAMERA	0x56
#define RC_MAKEMESH		0x57
#define RC_MAKETEXTURE	0x60
#define RC_DELETEBUFFER	0x58
#define RC_DELETEARRAY	0x59
#define RC_SHOWCURSOR	0x80


#define MC_LOADDATA		0xA0

struct RenderCommand
{
	uint command;
	uint64 param1;
	uint64 param2;
};

struct MeshCommand
{
	uint command;
	uint64 param1;
	uint64 param2;
};