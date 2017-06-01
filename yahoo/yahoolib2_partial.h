#ifndef YAHOOLIB2_PARTIAL_H
#define YAHOOLIB2_PARTIAL_H
#ifdef __cplusplus
extern "C"
{
void GetYahooStringsPreVer11(const char *seed,const char *sn,
	char *user,char *pw,unsigned char *result6,unsigned char *result96);
void GetYahooStringsVer11(const char *seed,const char *sn,
	char *pw,char *resp_6,char *resp_96);
}
#else
void GetYahooStringsPreVer11(const char *seed,const char *sn,
	char *user,char *pw,unsigned char *result6,unsigned char *result96);
void GetYahooStringsVer11(const char *seed,const char *sn,
	char *pw,char *resp_6,char *resp_96);
#endif // __cplusplus

#endif // YAHOOLIB2_PARTIAL_H
