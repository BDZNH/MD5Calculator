#include <inttypes.h>
#ifdef __cplusplus
extern "C" {
#endif

	uint32_t calculate_crc32(uint32_t crc, const unsigned char* buf, size_t len);

#ifdef __cplusplus
}
#endif