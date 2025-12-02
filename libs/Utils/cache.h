#ifndef CACHE_H
#define CACHE_H

#define ERROR 1
#define OK 0
#define UP_TO_DATE 0
#define OUT_OF_DATE -1
#define DOES_NOT_EXIST -2

typedef struct {
    const char* base_path;
    int ttl_seconds;
} CacheConfig;


int Cache_Check(const char* _CacheKey, const CacheConfig* _Config);
int Cache_Save(const char* _CacheKey, const char* _Data, const CacheConfig* _Config);
char* Cache_Load(const char* _CacheKey, const CacheConfig* _Config);

// Helper function to construct full file path
void Cache_BuildPath(char* _Buffer, size_t _BufferSize, const char* _CacheKey, const CacheConfig* _Config);

#endif // CACHE_H