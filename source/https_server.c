#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define SIZE 1024

void handle_client(SSL *ssl);
void error_handling(char *message);

int main(int argc, char *argv[]){

    //setting openssl
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    const SSL_METHOD *method = TLS_server_method();
    SSL_CTX *ctx = SSL_CTX_new(method);

    if(!ctx){
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    if(SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM)<=0){
        ERR_print_errors_fp(stderr);
        exit(2);
    }

    if(SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM)<=0){
        ERR_print_errors_fp(stderr);
        exit(3);
    }

    //setting socket
    int serv_sock, cli_sock;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_addr_len;

    serv_sock = socket(AF_INET, SOCK_STREAM, 0);

    if(serv_sock==-1){
        error_handling("socket() error");
        
    }

    memset(&serv_addr, 0x00, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1){
        error_handling("bind() error");
    }

    if(listen(serv_sock, 5)==-1){
        error_handling("listen() error");
    }

    while(1){
        cli_addr_len = sizeof(cli_addr);
        cli_sock = accept(serv_sock, (struct sockaddr*)&cli_addr, &cli_addr_len);

        if(cli_sock==-1){
            error_handling("accept() error");
            continue;
        }
        
        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, cli_sock);

        if(SSL_accept(ssl)<=0){
            ERR_print_errors_fp(stderr);
        }
        else{
            handle_client(ssl);
        }

        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(cli_sock);
        
    }

    close(serv_sock);
    SSL_CTX_free(ctx);

}

void handle_client(SSL *ssl){
    char buf[SIZE];
    char method[SIZE], path[SIZE], version[SIZE];
    int fd;

    int read_len = SSL_read(ssl, buf, SIZE-1);
    buf[read_len] = '\0';

    sscanf(buf, "%s %s %s", method, path, version);

    if(strcmp(method, "GET")!=0){
        SSL_write(ssl, "HTTP/1.1 405 Method Not Allowed\r\n\r\n", 36);
        return;
    }

    char *filename = path+1;

    fd = open(filename, O_RDONLY);

    if(fd==-1){
        char *notFound = "HTTP/1.1 404 Not Found\r\n\r\nFile Not Found";
        SSL_write(ssl, notFound, strlen(notFound));
        return;
    }
    char *ok_response = "HTTP/1.1 200 OK\r\n\r\n";
    SSL_write(ssl, ok_response, strlen(ok_response));

    while((read_len=read(fd, buf, SIZE))>0){
        SSL_write(ssl, buf, read_len);
    }

    close(fd);
}

void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(4);
}