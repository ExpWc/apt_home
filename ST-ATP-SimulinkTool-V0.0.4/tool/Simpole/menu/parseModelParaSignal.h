#ifdef RAINDLL_EXPORTS
#define RAINDLL_API __declspec(dllexport)
#else
#define RAINDLL_API __declspec(dllimport)
#endif
#ifdef __cplusplus
extern "C"{
#endif

 void  RAINDLL_API parseModelParaSignal(char * filePath,char* modelName,char* version);
#ifdef __cplusplus
}
#endif