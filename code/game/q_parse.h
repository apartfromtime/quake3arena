#ifndef Q_PARSE_H
#define Q_PARSE_H

/*
=====================================================================================

SCRIPT PARSING

=====================================================================================
*/

// this just controls the comment printing, it doesn't actually load a file
void Com_BeginParseSession(const char* filename);
void Com_EndParseSession(void);
int		Com_GetCurrentParseLine(void);
// Will never return NULL, just empty strings.
// An empty string will only be returned at end of file.
// ParseOnLine will return empty if there isn't another token on this line

// this funny typedef just means a moving pointer into a const char * buffer
const char* Com_Parse(const char* (*data_p));
const char* Com_ParseOnLine(const char* (*data_p));
const char* Com_ParseRestOfLine(const char* (*data_p));
char* Com_ParseExt(const char* (*data_p), qboolean allowLineBreak);
int		Com_Compress(char* data_p);
void	Com_ParseError(char* format, ...);
void	Com_ParseWarning(char* format, ...);
//int		Com_ParseInfos( char *buf, int max, char infos[][MAX_INFO_STRING] );
void Com_UngetToken(void);

#define MAX_TOKENLENGTH		1024

#ifndef TT_STRING
//token types
#define TT_STRING					1			// string
#define TT_LITERAL					2			// literal
#define TT_NUMBER					3			// number
#define TT_NAME						4			// name
#define TT_PUNCTUATION				5			// punctuation
#endif

typedef struct pc_token_s
{
	int type;
	int subtype;
	int intvalue;
	float floatvalue;
	char string[MAX_TOKENLENGTH];
} pc_token_t;

// data is an in/out parm, returns a parsed out token
#ifdef __cplusplus
void Com_MatchToken(const char* (*buf_p), const char* match, qboolean warning = qfalse);
#else
void Com_MatchToken(const char* (*buf_p), const char* match, qboolean warning);
#endif

void Com_SkipBracedSection(const char* (*program));
void Com_SkipRestOfLine(const char* (*data));

int	Com_ParseHex(const char* text);
float Com_ParseFloat(const char* (*buf_p));
int Com_ParseInt(const char* (*buf_p));
void Com_Parse1DMatrix(const char* (*buf_p), int x, float* m);
void Com_Parse2DMatrix(const char* (*buf_p), int y, int x, float* m);
void Com_Parse3DMatrix(const char* (*buf_p), int z, int y, int x, float* m);

#endif // #ifndef Q_PARSE_H