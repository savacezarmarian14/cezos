#ifndef __INC_STRING_H__
#define __INC_STRING_H__
/* Memory */
void *memset(void *dst, int c, size_t len);
void *memcpy(void *dst, const void *src, size_t len);
void *memmove(void *dst, const void *src, size_t len);
int	  memcmp(const void *s1, const void *s2, size_t len);
void *memfind(const void *s, int c, size_t len);

