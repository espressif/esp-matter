

#ifndef __URL_H__
#define __URL_H__


typedef enum
{
    TFTP,
    HTTP,
    HTTPS,
    UNKNOWN
} url_scheme_t;


typedef struct _url_t
{
    url_scheme_t    scheme;
    char            *host;
    int             port;
    char            *path;
} url_t;


int url_parse(url_t *url, const char *text);


void url_free(url_t *url);


#endif /* __URL_H__ */

