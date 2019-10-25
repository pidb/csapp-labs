#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define DEFAULT_PORT "80"

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

//void doit(int fd);
void *thread_handle_req(void *vargp);
void read_requesthdrs(rio_t *rp);

void parse_uri(char *uri, char *hostname, char *port, char *query);
void build_request(char *req, char *host, char *query);

void serve_static(int fd, char *filename, int filesize);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum,
         char *shortmsg, char *longmsg);

int main(int argc, char **argv)
{
    printf("%s\n", user_agent_hdr);

    int listenfd;
    int *connfdp;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    /* Check command line args */
    if (argc != 2) {
       fprintf(stderr, "usage: %s <port>\n", argv[0]);
       exit(1);
    }

    listenfd = Open_listenfd(argv[1]);
    while (1) {
        clientlen = sizeof(clientaddr);
        
        connfdp = Malloc(sizeof(int));    
        *connfdp = Accept(listenfd, (SA *)&clientaddr, &clientlen);

        // DEBUG
        printf("Made a connection with fd %d\n", *connfdp);

        Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, 
            port, MAXLINE, 0);

        printf("Accepted connection from (%s, %s)\n", hostname, port);
        Pthread_create(&tid, NULL, thread_handle_req, connfdp);
    }
}

/*
 * thread_handle_req - 
 */
void *thread_handle_req(void *vargp)
{
    int fd = *((int *)vargp);
    Pthread_detach(pthread_self());
    Free(vargp);

    //printf("file desc : %d\n", fd);

    int is_static;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];
    rio_t rio;

    /* Read request line and headers */
    Rio_readinitb(&rio, fd);
    if (!Rio_readlineb(&rio, buf, MAXLINE))
        return;
    
    // DEBUG
    printf("buff: %s len = %d \n", buf, strlen(buf));

    sscanf(buf, "%s %s %s", method, uri, version);       
    if (strcasecmp(method, "GET")) {
        return;
    }
    read_requesthdrs(&rio);

    /* Parse URI from GET request */
    char host[MAXLINE], query[MAXLINE];
    char port[MAXLINE];
    parse_uri(uri, host, port, query);

    // GET http://localhost:8550/ HTTP/1.1
    // GET http://www.example.com/ HTTP/1.0

    int clientfd;
    if ((clientfd = open_clientfd(host, port)) < 0) {
        // TODO: need to handle this error properly.
        unix_error("Open_clientfd error");
    }

    // DEBUG
    printf("clientfd %d\n", clientfd);

    /* Send request to server. */
    char req[MAXLINE];
    build_request(req, host, query);
    Rio_writen(clientfd, req, strlen(req));

    rio_t response_rio;
    ssize_t n;
    char response_buf[MAX_OBJECT_SIZE];
    Rio_readinitb(&response_rio, clientfd);

    if ((n = rio_readn(clientfd, response_buf, MAX_OBJECT_SIZE)) < 0) {
        unix_error("Rio_readlineb error");
    }
    
    Close(clientfd);
    
    // DEBUG
    printf("Response: %s\n", response_buf);
    printf("Length: %d\n", n);

    Rio_writen(fd, response_buf, n);

    Close(fd);
    return NULL;
}

/*
 * read_requesthdrs - read HTTP request headers
 */
void read_requesthdrs(rio_t *rp)
{
    char buf[MAXLINE];

    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
    while(strcmp(buf, "\r\n")) {
       Rio_readlineb(rp, buf, MAXLINE);
       printf("%s", buf);
    }
    return;
}

/*
 * parse_uri
 */
void parse_uri(char *uri, char *host, char *port, char *query)
{
    char host_port[MAXLINE];

    // FIXME: Is this general enough ?
    sscanf(uri, "http://%[^/]%s", host_port, query);
    
    printf("hostname: %s, query: %s \n", host_port, query);
    
    /* Scan non-default port if exists. */
    if (strchr(host_port, ':')) {
        sscanf(host_port, "%[^:]:%s", host, port);
    } else {
        strcpy(port, DEFAULT_PORT);
        strcpy(host, host_port);
    }
    
    printf("hname: %s, port: %s\n", host, port);
    
    //http://www.cmu.edu:8080/hub/index.html
    //GET http://www.cmu.edu:8080/hub/index.html HTTP1.1
    //GET http://www.cmu.edu/hub/index.html HTTP1.1
}

void build_request(char *req, char *host, char *query)
{
    char buf[MAXBUF];

    sprintf(buf, "GET %s HTTP/1.0\r\n", query);
    sprintf(buf, "%sHost: %s\r\n", buf, host);
    sprintf(buf, "%sUser-Agent: %s", buf, user_agent_hdr);
    sprintf(buf, "%sConnection: close\r\n", buf);
    sprintf(buf, "%sProxy-Connection: close\r\n", buf);
    sprintf(buf, "%s\r\n", buf);
    
    strcpy(req, buf);
    printf("Request:\n%s\n", buf);
}

/*
 * clienterror - returns an error message to the client
 */
void clienterror(int fd, char *cause, char *errnum,
         char *shortmsg, char *longmsg) 
{
    char buf[MAXLINE], body[MAXBUF];

    /* Build the HTTP response body */
    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

    /* Print the HTTP response */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}
