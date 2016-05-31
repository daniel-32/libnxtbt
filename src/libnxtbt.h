#ifndef _libnxtbt_h_
#define _libnxtbt_h_

#include <stdint.h>

#define NXT_UBYTE_MIN 0
#define NXT_UBYTE_MAX 255
#define NXT_SBYTE_MIN -128
#define NXT_SBYTE_MAX 127
#define NXT_UWORD_MIN 0
#define NXT_UWORD_MAX 65535
#define NXT_SWORD_MIN -32768
#define NXT_SWORD_MAX 32767
#define NXT_ULONG_MIN 0
#define NXT_ULONG_MAX 4294967296
#define NXT_SLONG_MIN -2147483648
#define NXT_SLONG_MAX 2147483647

typedef enum
{
	// Direct commands
	NXT_CMD_STARTPROGRAM = 0x00,
	NXT_CMD_STOPPROGRAM = 0x01,
	NXT_CMD_PLAYSOUNDFILE = 0x02,
	NXT_CMD_PLAYTONE = 0x03,
	NXT_CMD_SETOUTPUTSTATE = 0x04,
	NXT_CMD_SETINPUTMODE = 0x05,
	NXT_CMD_GETOUTPUTSTATE = 0x06,
	NXT_CMD_GETINPUTVALUES = 0x07,
	NXT_CMD_RESETINPUTSCALEDVALUE = 0x08,
	NXT_CMD_MESSAGEWRITE = 0x09,
	NXT_CMD_RESETMOTORPOSITION = 0x0A,
	NXT_CMD_GETBATTERYLEVEL = 0x0B,
	NXT_CMD_STOPSOUNDPLAYBACK = 0x0C,
	NXT_CMD_KEEPALIVE = 0x0D,
	NXT_CMD_LSGETSTATUS = 0x0E,
	NXT_CMD_LSWRITE = 0x0F,
	NXT_CMD_LSREAD = 0x10,
	NXT_CMD_GETCURRENTPROGRAMNAME = 0x11,
	NXT_CMD_MESSAGEREAD = 0x13,
	// System commands
	NXT_CMD_OPENREAD = 0x80,
	NXT_CMD_OPENWRITE = 0x81,
	NXT_CMD_READ = 0x82,
	NXT_CMD_WRITE = 0x83,
	NXT_CMD_CLOSE = 0x84,
	NXT_CMD_DELETE = 0x85,
	NXT_CMD_FINDFIRST = 0x86,
	NXT_CMD_FINDNEXT = 0x87,
	NXT_CMD_GETFIRMWAREVERSION = 0x88,
	NXT_CMD_OPENWRITELINEAR = 0x89,
	// OpenReadLinear (0x8A) omitted
	NXT_CMD_OPENWRITEDATA = 0x8B,
	NXT_CMD_OPENAPPENDDATA = 0x8C,
	// Boot (0x97) omitted
	NXT_CMD_SETBRICKNAME = 0x98,
	NXT_CMD_GETDEVICEINFO = 0x9B,
	// commands 0xA? omitted
} nxtCommand;

typedef enum
{
	NXT_TYPE_BOOLEAN,
	NXT_TYPE_UBYTE,
	NXT_TYPE_SBYTE,
	NXT_TYPE_UWORD,
	NXT_TYPE_SWORD,
	NXT_TYPE_ULONG,
	NXT_TYPE_SLONG,
	NXT_TYPE_BYTES,
	NXT_TYPE_STRING,
	NXT_TYPE_FILENAME,
} nxtType;

typedef union
{
	uint8_t	boolean;
	uint8_t	ubyte;
	int8_t	sbyte;
	uint16_t	uword;
	int16_t	sword;
	uint32_t	ulong;
	int32_t	slong;
	uint8_t*	bytes;
	char*	string;
	char*	filename;
} nxtValue;

typedef struct
{
	nxtType	type;
	nxtValue	value;
	int	length;
} nxtParameter;

typedef struct
{
	nxtType	type;
	nxtValue	value;
	int	length;
} nxtResponse;

typedef enum
{
	NXT_STS_SUCCESS = 0x00,
	NXT_STS_PENDING_TRANSACTION_IN_PROGRESS = 0x20,
	NXT_STS_MAILBOX_QUEUE_EMPTY = 0x40,
	NXT_STS_NO_MORE_HANDLES = 0x81,
	NXT_STS_NO_SPACE = 0x82,
	NXT_STS_NO_MORE_FILES = 0x83,
	NXT_STS_EOF_EXPECTED = 0x84,
	NXT_STS_EOF = 0x85,
	NXT_STS_NOT_LINEAR_FILE = 0x86,
	NXT_STS_FILE_NOT_FOUND = 0x87,
	NXT_STS_HANDLE_ALREADY_CLOSED = 0x88,
	NXT_STS_NO_LINEAR_SPACE = 0x89,
	NXT_STS_UNDEFINED_ERROR = 0x8A,
	NXT_STS_FILE_BUSY = 0x8B,
	NXT_STS_NO_WRITE_BUFFERS = 0x8C,
	NXT_STS_APPEND_NOT_POSSIBLE = 0x8D,
	NXT_STS_FILE_FULL = 0x8E,
	NXT_STS_FILE_EXISTS = 0x8F,
	NXT_STS_MODULE_NOT_FOUND = 0x90,
	NXT_STS_OUT_OF_BOUNDARY = 0x91,
	NXT_STS_ILLEGAL_FILENAME = 0x92,
	NXT_STS_ILLEGAL_HANDLE = 0x93,
	NXT_STS_REQUEST_FAILED = 0xBD,
	NXT_STS_UNKNOWN_OPCODE = 0xBE,
	NXT_STS_INSANE_PACKET = 0xBF,
	NXT_STS_OUT_OF_RANGE_VALUE = 0xC0,
	NXT_STS_COMMUNICATION_BUS_ERROR = 0xDD,
	NXT_STS_NO_FREE_MEMORY_IN_COMMUNICATION_BUFFER = 0xDE,
	NXT_STS_CHANNEL_NOT_VALID = 0xDF,
	NXT_STS_CHANNEL_BUSY = 0xE0,
	NXT_STS_NO_ACTIVE_PROGRAM = 0xEC,
	NXT_STS_ILLEGAL_SIZE = 0xED,
	NXT_STS_ILLEGAL_MAILBOX_QUEUE = 0xEE,
	NXT_STS_INVALID_STRUCTURE_FIELD = 0xEF,
	NXT_STS_BAD_INPUT_OUTPUT = 0xF0,
	NXT_STS_INSUFFICIENT_MEMORY = 0xFB,
	NXT_STS_BAD_ARGUMENTS = 0xFF,
} nxtStatus;

typedef enum
{
	NXT_LIBERR_GENERAL = -1,	// unspecified error

	NXT_LIBERR_PARAMETER_CANNOT_ADD = -16,	// failed to add parameter to buffer

	NXT_LIBERR_RESPONSE_TOO_SHORT = -32,	// response is less than 2 bytes long
	NXT_LIBERR_RESPONSE_HEADER_INCORRECT = -33,	// first response byte != 0x02
	NXT_LIBERR_RESPONSE_COMMAND_MISMATCH = -34,	// second response byte does not match command byte
	NXT_LIBERR_RESPONSE_TYPE_MISMATCH = -35,	// response does not contain data that can be interpreted in the requested type
	NXT_LIBERR_RESPONSE_CANNOT_ADD = -36,	// failed to add response value to response array
} nxtLibError;

void nxtOpen(const char* device);
void nxtClose();
int nxtDoCommand(nxtCommand command, nxtParameter parameters[], nxtResponse responses[], int parameter_count, int response_count);
char* nxtStatusString(nxtStatus status);
char* nxtLibErrorString(nxtLibError liberror);

#endif
