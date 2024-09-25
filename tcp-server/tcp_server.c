#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8181

int main(){
    int server,client;
    socklen_t addrlen;
    struct sockaddr_in srv,cli;

    // creating buffer for predifined data
    char buff[512];
    char *data;

    addrlen=0;

    // Initializing structures by value 0
    memset(&srv,0,sizeof(srv));
    memset(&cli,0,sizeof(cli));
    
    server=socket(AF_INET,SOCK_STREAM,0);
    if(server<0){
        printf("Socket creation failed\n");
        return -1;
    }

    srv.sin_family=AF_INET;
    srv.sin_addr.s_addr=0;
    srv.sin_port= htons(PORT);

    if(bind(server,(struct sockaddr *)&srv,sizeof(srv))){
        printf("Bind failed\n");
        close(server);
        return -1;
    }

    if(listen(server,5)){
        printf("Listen failed\n");
        close(server);
        return -1;
    }
    printf("Listening on 0.0.0.0:%d\n",PORT);

    client =accept(server,(struct sockaddr *)&srv, &addrlen);
    if(client<0){
        printf("accept failed\n");
        close(server);
        return -1;
    }
    
    printf("Client connected\n");
    read(client,buff,511);
    data="httpd v1.0\n";
    write(client,data,strlen(data));

    close(client);
    close(server);
    return 0;




}