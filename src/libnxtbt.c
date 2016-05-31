#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

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

static int	mPort;
uint8_t*	mBuffer;
uint16_t	mBufferLength;

static int add_boolean(nxtParameter parameter);
static int add_ubyte(nxtParameter parameter);
static int add_sbyte(nxtParameter parameter);
static int add_uword(nxtParameter parameter);
static int add_sword(nxtParameter parameter);
static int add_ulong(nxtParameter parameter);
static int add_slong(nxtParameter parameter);
static int add_bytes(nxtParameter parameter);
static int add_string(nxtParameter parameter);
static int add_filename(nxtParameter parameter);

static int get_boolean(nxtResponse* response);
static int get_ubyte(nxtResponse* response);
static int get_sbyte(nxtResponse* response);
static int get_uword(nxtResponse* response);
static int get_sword(nxtResponse* response);
static int get_ulong(nxtResponse* response);
static int get_slong(nxtResponse* response);
static int get_bytes(nxtResponse* response);
static int get_string(nxtResponse* response);
static int get_filename(nxtResponse* response);

static void append_byte(uint8_t byte);
static uint8_t remove_byte();

// PUBLIC FUNCTIONS

void nxtOpen(const char* device)
{
	struct termios	port_settings;

	mPort = open(device, O_RDWR | O_NOCTTY | O_SYNC);
	tcgetattr(mPort, &port_settings);
	port_settings.c_iflag = 0;
	port_settings.c_oflag = 0;
	port_settings.c_cflag = 0;
	port_settings.c_lflag = 0;
	port_settings.c_cc[VTIME] = 1;
	port_settings.c_cc[VMIN] = 1;
	tcflush(mPort, TCIFLUSH);
	tcsetattr(mPort, TCSANOW, &port_settings);
}

void nxtClose()
{
	close(mPort);
}

int nxtDoCommand(nxtCommand command, nxtParameter parameters[], nxtResponse responses[], int parameter_count, int response_count)
{

	int	parameter_index;
	int	response_index;

	mBuffer = malloc(2);
	mBufferLength = 2;

	if (command < 0x80)
	{
		mBuffer[0] = 0x00;
	}
	else
	{
		mBuffer[0] = 0x01;
	}
	mBuffer[1] = command;

	parameter_index = 0;
	while (parameter_index < parameter_count)
	{
		#define add(type)\
		if (add_ ## type(parameters[parameter_index]) == false)\
		{\
			free(mBuffer);\
			return NXT_LIBERR_PARAMETER_CANNOT_ADD;\
		}

		switch (parameters[parameter_index].type)
		{
			case NXT_TYPE_BOOLEAN:
				add(boolean)
				break;
			case NXT_TYPE_UBYTE:
				add(ubyte)
				break;
			case NXT_TYPE_SBYTE:
				add(sbyte)
				break;
			case NXT_TYPE_UWORD:
				add(uword)
				break;
			case NXT_TYPE_SWORD:
				add(sword)
				break;
			case NXT_TYPE_ULONG:
				add(ulong)
				break;
			case NXT_TYPE_SLONG:
				add(slong)
				break;
			case NXT_TYPE_BYTES:
				add(bytes)
				break;
			case NXT_TYPE_STRING:
				add(string)
				break;
			case NXT_TYPE_FILENAME:
				add(filename)
				break;
		}

		parameter_index += 1;
	}

	write(mPort, &mBufferLength, 2);
	write(mPort, mBuffer, mBufferLength);

	free(mBuffer);

	read(mPort, &mBufferLength, 2);
	mBuffer = malloc(mBufferLength);
	read(mPort, mBuffer, mBufferLength);

	if (mBufferLength >= 2)
	{
		if (mBuffer[0] != 0x02)
		{
			free(mBuffer);
			return NXT_LIBERR_RESPONSE_HEADER_INCORRECT;
		}
		if (mBuffer[1] != command)
		{
			free(mBuffer);
			return NXT_LIBERR_RESPONSE_COMMAND_MISMATCH;
		}
		remove_byte();
		remove_byte();

		response_index = 0;
		while (response_index < response_count && mBufferLength > 0)
		{
			#define get(type)\
			if (get_ ## type(&(responses[response_index])) == false)\
			{\
				free(mBuffer);\
				return NXT_LIBERR_RESPONSE_TYPE_MISMATCH;\
			}

			switch (responses[response_index].type)
			{
				case NXT_TYPE_BOOLEAN:
					get(boolean)
					break;
				case NXT_TYPE_UBYTE:
					get(ubyte)
					break;
				case NXT_TYPE_SBYTE:
					get(sbyte)
					break;
				case NXT_TYPE_UWORD:
					get(uword)
					break;
				case NXT_TYPE_SWORD:
					get(sword)
					break;
				case NXT_TYPE_ULONG:
					get(ulong)
					break;
				case NXT_TYPE_SLONG:
					get(slong)
					break;
				case NXT_TYPE_BYTES:
					get(bytes)
					break;
				case NXT_TYPE_STRING:
					get(string)
					break;
				case NXT_TYPE_FILENAME:
					get(filename)
					break;
			}

			response_index += 1;
		}

		if (mBufferLength > 0)
		{
			free(mBuffer);
			return NXT_LIBERR_RESPONSE_CANNOT_ADD;
		}

		free(mBuffer);
		return response_index;
	}
	else
	{
		free(mBuffer);
		return NXT_LIBERR_RESPONSE_TOO_SHORT;
	}

	free(mBuffer);
	return NXT_LIBERR_GENERAL;
}

char* nxtStatusString(nxtStatus status)
{
	switch (status)
	{
		case NXT_STS_SUCCESS:
			return strdup("Success");
		case NXT_STS_PENDING_TRANSACTION_IN_PROGRESS:
			return strdup("Pending communication transaction in progress");
		case NXT_STS_MAILBOX_QUEUE_EMPTY:
			return strdup("Specified mailbox queue is empty");
		case NXT_STS_NO_MORE_HANDLES:
			return strdup("No more handles");
		case NXT_STS_NO_SPACE:
			return strdup("No space");
		case NXT_STS_NO_MORE_FILES:
			return strdup("No more files");
		case NXT_STS_EOF_EXPECTED:
			return strdup("End of file expected");
		case NXT_STS_EOF:
			return strdup("End of file");
		case NXT_STS_NOT_LINEAR_FILE:
			return strdup("Not a linear file");
		case NXT_STS_FILE_NOT_FOUND:
			return strdup("File not found");
		case NXT_STS_HANDLE_ALREADY_CLOSED:
			return strdup("Handle already closed");
		case NXT_STS_NO_LINEAR_SPACE:
			return strdup("No linear space");
		case NXT_STS_UNDEFINED_ERROR:
			return strdup("Undefined error");
		case NXT_STS_FILE_BUSY:
			return strdup("File is busy");
		case NXT_STS_NO_WRITE_BUFFERS:
			return strdup("No write buffers");
		case NXT_STS_APPEND_NOT_POSSIBLE:
			return strdup("Append not possible");
		case NXT_STS_FILE_FULL:
			return strdup("File is full");
		case NXT_STS_FILE_EXISTS:
			return strdup("File exists");
		case NXT_STS_MODULE_NOT_FOUND:
			return strdup("Module not found");
		case NXT_STS_OUT_OF_BOUNDARY:
			return strdup("Out of boundary");
		case NXT_STS_ILLEGAL_FILENAME:
			return strdup("Illegal filename");
		case NXT_STS_ILLEGAL_HANDLE:
			return strdup("Illegal handle");
		case NXT_STS_REQUEST_FAILED:
			return strdup("Request failed");
		case NXT_STS_UNKNOWN_OPCODE:
			return strdup("Unknown command opcode");
		case NXT_STS_INSANE_PACKET:
			return strdup("Insane packet");
		case NXT_STS_OUT_OF_RANGE_VALUE:
			return strdup("Data contains out of range values");
		case NXT_STS_COMMUNICATION_BUS_ERROR:
			return strdup("Communication bus error");
		case NXT_STS_NO_FREE_MEMORY_IN_COMMUNICATION_BUFFER:
			return strdup("No free memory in communication buffer");
		case NXT_STS_CHANNEL_NOT_VALID:
			return strdup("Specified channel/connection is not valid");
		case NXT_STS_CHANNEL_BUSY:
			return strdup("Specified channel/connection not configured or busy");
		case NXT_STS_NO_ACTIVE_PROGRAM:
			return strdup("No active program");
		case NXT_STS_ILLEGAL_SIZE:
			return strdup("Illegal size specified");
		case NXT_STS_ILLEGAL_MAILBOX_QUEUE:
			return strdup("Illegal mailbox queue ID specified");
		case NXT_STS_INVALID_STRUCTURE_FIELD:
			return strdup("Attempted to access invalid field of a structure");
		case NXT_STS_BAD_INPUT_OUTPUT:
			return strdup("Bad input or output specified");
		case NXT_STS_INSUFFICIENT_MEMORY:
			return strdup("Insufficient memory available");
		case NXT_STS_BAD_ARGUMENTS:
			return strdup("Bad arguments");
		default:
			return strdup("Invalid status byte");
	}
}

char* nxtLibErrorString(nxtLibError liberror)
{
	switch (liberror)
	{
		case NXT_LIBERR_GENERAL:
			return strdup("Unspecified error");
		case NXT_LIBERR_PARAMETER_CANNOT_ADD:
			return strdup("Unspecified error adding parameter to buffer");
		case NXT_LIBERR_RESPONSE_TOO_SHORT:
			return strdup("Response less than 2 bytes long");
		case NXT_LIBERR_RESPONSE_HEADER_INCORRECT:
			return strdup("First response byte not 0x02");
		case NXT_LIBERR_RESPONSE_COMMAND_MISMATCH:
			return strdup("Second response byte does not match command byte");
		case NXT_LIBERR_RESPONSE_TYPE_MISMATCH:
			return strdup("Response does not contain data that can be interpreted in the expected type");
		case NXT_LIBERR_RESPONSE_CANNOT_ADD:
			return strdup("Unspecified error adding response to array");
		default:
			return strdup("Invalid error");
	}
}

// PRIVATE FUNCTIONS

static int add_boolean(nxtParameter parameter)
{
	append_byte(parameter.value.boolean);

	return true;
}

static int add_ubyte(nxtParameter parameter)
{
	append_byte(parameter.value.ubyte);

	return true;
}

static int add_sbyte(nxtParameter parameter)
{
	append_byte(parameter.value.sbyte);

	return true;
}

static int add_uword(nxtParameter parameter)
{
	append_byte(parameter.value.uword & 0xFF);
	append_byte((parameter.value.uword & 0xFF00) >> 8);

	return true;
}

static int add_sword(nxtParameter parameter)
{
	append_byte(parameter.value.sword & 0xFF);
	append_byte((parameter.value.sword & 0xFF00) >> 8);

	return true;
}

static int add_ulong(nxtParameter parameter)
{
	append_byte(parameter.value.ulong & 0xFF);
	append_byte((parameter.value.ulong & 0xFF00) >> 8);
	append_byte((parameter.value.ulong & 0xFF0000) >> 16);
	append_byte((parameter.value.ulong & 0xFF000000) >> 24);

	return true;
}

static int add_slong(nxtParameter parameter)
{
	append_byte(parameter.value.slong & 0xFF);
	append_byte((parameter.value.slong & 0xFF00) >> 8);
	append_byte((parameter.value.slong & 0xFF0000) >> 16);
	append_byte((parameter.value.slong & 0xFF000000) >> 24);

	return true;
}

static int add_bytes(nxtParameter parameter)
{
	int	current_position;

	if (parameter.length == -1)
	{
		return false;
	}
	if (parameter.value.bytes == NULL)
	{
		return false;
	}

	current_position = 0;
	while (current_position < parameter.length)
	{
		append_byte(parameter.value.bytes[current_position]);
		current_position += 1;
	}

	return true;
}

static int add_string(nxtParameter parameter)
{
	int	current_position;

	if (parameter.value.string == NULL)
	{
		return false;
	}

	if (parameter.length >= 0)
	{
		if (strlen(parameter.value.string) > parameter.length - 1)
		{
			return false;
		}
	}

	current_position = 0;
	while (parameter.value.string[current_position] != 0)
	{
		append_byte(parameter.value.string[current_position]);
		current_position += 1;
	}
	if (parameter.length >= 0)
	{
		while (current_position < parameter.length)
		{
			append_byte(0);
			current_position += 1;
		}
	}
	else
	{
		append_byte(0);
	}

	return true;
}

static int add_filename(nxtParameter parameter)
{
	int	current_position;

	if (parameter.value.filename == NULL)
	{
		return false;
	}

	if (strlen(parameter.value.filename) > 19)
	{
		return false;
	}

	current_position = 0;
	while (parameter.value.filename[current_position] != 0)
	{
		append_byte(parameter.value.filename[current_position]);
		current_position += 1;
	}
	while (current_position < 20)
	{
		append_byte(0);
		current_position += 1;
	}

	return true;
}

static int get_boolean(nxtResponse* response)
{
	if (mBufferLength < 1)
	{
		return false;
	}

	switch (remove_byte())
	{
		case true:
			response->value.boolean = true;
			return true;
		case false:
			response->value.boolean = false;
			return true;
		default:
			return false;
	}
}

static int get_ubyte(nxtResponse* response)
{
	if (mBufferLength < 1)
	{
		return false;
	}

	response->value.ubyte = remove_byte();

	return true;
}

static int get_sbyte(nxtResponse* response)
{
	if (mBufferLength < 1)
	{
		return false;
	}

	response->value.ubyte = remove_byte();

	return true;
}

static int get_uword(nxtResponse* response)
{
	if (mBufferLength < 2)
	{
		return false;
	}

	response->value.uword = remove_byte();
	response->value.uword = response->value.uword | (remove_byte() << 8);

	return true;
}

static int get_sword(nxtResponse* response)
{
	if (mBufferLength < 2)
	{
		return false;
	}

	response->value.sword = remove_byte();
	response->value.sword = response->value.sword | (remove_byte() << 8);

	return true;
}

static int get_ulong(nxtResponse* response)
{
	if (mBufferLength < 4)
	{
		return false;
	}

	response->value.ulong = remove_byte();
	response->value.ulong = response->value.ulong | (remove_byte() << 8);
	response->value.ulong = response->value.ulong | (remove_byte() << 16);
	response->value.ulong = response->value.ulong | (remove_byte() << 24);

	return true;
}

static int get_slong(nxtResponse* response)
{
	if (mBufferLength < 4)
	{
		return false;
	}

	response->value.slong = remove_byte();
	response->value.slong = response->value.slong | (remove_byte() << 8);
	response->value.slong = response->value.slong | (remove_byte() << 16);
	response->value.slong = response->value.slong | (remove_byte() << 24);

	return true;
}

static int get_bytes(nxtResponse* response)
{
	int	current_position;
	int	length;

	if (response->length >= 0)
	{
		if (mBufferLength < response->length)
		{
			return false;
		}
		length = response->length;
	}
	else
	{
		length = mBufferLength;
	}

	response->value.bytes = malloc(length);
	response->length = length;

	current_position = 0;
	while (current_position < length)
	{
		response->value.bytes[current_position] = remove_byte();
		current_position += 1;
	}

	return true;
}

static int get_string(nxtResponse* response)
{
	int	current_position;

	current_position = 0;
	while (current_position < mBufferLength)
	{
		if (mBuffer[current_position] == 0)
		{
			break;
		}

		current_position += 1;
	}
	if (current_position == mBufferLength)
	{
		return false;
	}

	if (response->length >= 0)
	{
		if (strlen((char*) mBuffer) > response->length - 1)
		{
			return false;
		}
	}

	if (response->length >= 0)
	{
		response->value.string = malloc(response->length);
	}
	else
	{
		response->value.string = malloc(strlen((char*) mBuffer) + 1);
	}

	current_position = 0;
	while (mBuffer[0] != 0)
	{
		response->value.string[current_position] = remove_byte();
		current_position += 1;
	}
	if (response->length >= 0)
	{
		while (current_position < response->length)
		{
			response->value.string[current_position] = remove_byte();
			current_position += 1;
		}
	}
	else
	{
		response->value.string[current_position] = remove_byte();
	}

	return true;
}

static int get_filename(nxtResponse* response)
{
	int	current_position;

	if (mBufferLength < 20)
	{
		return false;
	}

	current_position = 0;
	while (current_position < 20)
	{
		if (mBuffer[current_position] == 0)
		{
			break;
		}

		current_position += 1;
	}
	if (current_position == 20)
	{
		return false;
	}

	response->value.filename = malloc(20);

	current_position = 0;
	while (current_position < 20)
	{
		response->value.filename[current_position] = remove_byte();
		current_position += 1;
	}

	return true;
}

static void append_byte(uint8_t byte)
{
	mBufferLength += 1;
	mBuffer = realloc(mBuffer, mBufferLength);
	mBuffer[mBufferLength - 1] = byte;
}

static uint8_t remove_byte()
{
	uint8_t	byte;
	uint8_t*	new_buffer;

	byte = mBuffer[0];
	new_buffer = malloc(mBufferLength - 1);
	memcpy(new_buffer, mBuffer + 1, mBufferLength - 1);
	free(mBuffer);
	mBuffer = new_buffer;
	mBufferLength -= 1;

	return byte;
}
