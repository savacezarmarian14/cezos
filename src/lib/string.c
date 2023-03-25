#include <string.h>
#include <types.h>

/* String functions */
int strlen(const char *s)
{
    int count = 0; 
    const char *p;

    if (s == NULL) {
        return -1;
    }

    for (p = s; *p != '\0'; p++)
        count++;
    
    return count;
} // TESTED

int strnlen(const char *s, size_t size)
{
    int len;
    
    if (s == NULL) {
        return -1;
    }

    len = strlen(s);
    return len > (int)size ? (int)size : len;   
} // TESTED

char *strcpy(char *dst, const char *src)
{
    unsigned int i;

    for (i = 0; *(src + i) != '\0'; i++) {
        *(dst + i) = * (src + i);
    }
    *(dst + i) = '\0';

    return dst;
} // TESTED

char *strncpy(char *dst, const char *src, size_t size)
{
    unsigned int i;

    for (i = 0; *(src + i) != '\0' && i < size; i++) {
        *(dst + i) = *(src + i);
    }
    *(dst + i) = 0;

    return dst;
} // TESTED

char *strcat(char *dst, const char *src)
{
    int len;
    unsigned int i;

    len = strlen(dst);

    for (int i = 0; *(src + i) != '\0'; ++i) {
        *(dst + len + i) = *(src + i);
    }

    *(dst + len + i) = '\0';

    return dst;
} // TESTED

int	strcmp(const char *s1, const char *s2)
{
    unsigned int i;
    int diff;

    for (i = 0; *(s1 + i) != '\0' && *(s2 + i) != '\0'; ++i) {
        diff = ((int) *(s1 + i)) - ((int) *(s2 + i));
        if (diff != 0) {
            return -diff;
        }
    }

    return 0;
} // TESTED

int	strncmp(const char *s1, const char *s2, size_t size)
{
    unsigned int i;
    int diff;

    for (i = 0; *(s1 + i) != '\0' && *(s2 + i) != '\0' && i < size; ++i) {
        diff = ((int) *(s1 + i)) - ((int) *(s2 + i));
        if (diff != 0) {
            return -diff;
        }
    }

    return 0;
} // TESTED

char *strchr(const char *s, char c)
{
    unsigned int i;

    for (i = 0; *(s + i) != '\0'; ++i) {
        if (*(s + i) == c) {
            return (char *) (s + i);
        }
    }

    return NULL;
} // TESTED


long
strtol(const char *s, char **endptr, int base)
{
	int neg = 0;
	long val = 0;

	while (*s == ' ' || *s == '\t')
		s++;

	if (*s == '+')
		s++;
	else if (*s == '-')
		s++, neg = 1;

	if ((base == 0 || base == 16) && (s[0] == '0' && s[1] == 'x'))
		s += 2, base = 16;
	else if (base == 0 && s[0] == '0')
		s++, base = 8;
	else if (base == 0)
		base = 10;

	while (1) {
		int dig;

		if (*s >= '0' && *s <= '9')
			dig = *s - '0';
		else if (*s >= 'a' && *s <= 'z')
			dig = *s - 'a' + 10;
		else if (*s >= 'A' && *s <= 'Z')
			dig = *s - 'A' + 10;
		else
			break;
		if (dig >= base)
			break;
		s++, val = (val * base) + dig;
	}

	if (endptr)
		*endptr = (char *) s;
	return (neg ? -val : val);
} // TESTED


/* Memory functions */
void *memset(void *dst, int c, size_t len)
{
    int i;
    char mask[4];
    uint8_t *p;

    if (len == 0) {
        return dst;
    }

    for (i = 0; i < 4; i++) {
        mask[i] = ((c >> (8*i)) & 0xFF);
    }

    p = dst;
    for (i = 0; i < len; i++) {
        *p = mask[i%4];
        p++;
    }

    return dst;
}

void *memcpy(void *dst, const void *src, size_t len)
{
    uint8_t *p;
    size_t i;

    p = dst; 
    for (i = 0; i < len; ++i) {
        *p = *((uint8_t *)(src+i));
        p++;
    }
    
    return dst;
}

void *memmove(void *dst, const void *src, size_t len)
{
    return memcpy(dst, src, len);
}

int memcmp (const void *v1, const void *v2, size_t len)
{
    const uint8_t *s1 = (const uint8_t *) v1;
	const uint8_t *s2 = (const uint8_t *) v2;

	while (len-- > 0) {
		if (*s1 != *s2)
			return (int) *s1 - (int) *s2;
		s1++, s2++;
	}

	return 0;
}

void * memfind(const void *s, int c, size_t n)
{
	const void *ends = (const char *) s + n;
	for (; s < ends; s++)
		if (*(const unsigned char *) s == (unsigned char) c)
			break;
	return (void *) s;
}