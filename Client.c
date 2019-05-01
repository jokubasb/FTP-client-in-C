#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // close socket
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>



int passive(int networkSocket);
int FTPConnect();
void list();
void listLocal(char path[]);
void receiveFile(char filename[]);
void sendFile(char filename[]);
void deleteFile(char filename[]);
void renameFile(char filename[]);

int main(){
    while(1){
        printf("1. Failu sarasas\n");
        printf("2. Parsisiusti faila\n");
        printf("3. Ikelti faila\n");
        printf("4. Pervadinti faila\n");
        printf("5. Istrinti faila\n");
        printf("6. Iseiti\n");
        printf("Pasirinkite veiksma: ");

        int selection;
        scanf("%d", &selection);
        char filename[32] = {0};
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }

        switch (selection){
            case 1 : list();
                getchar();
            break;
            case 2 : list();
                printf("Iveskite failo pavadinima, kuri norite parsisiusti:");
                scanf("%[^\n]", filename);
                while ((c = getchar()) != '\n' && c != EOF) { }
                receiveFile(filename);
                getchar();
                break;
            case 3 : listLocal("/mnt/c/Users/Jokubas/Documents/Code/Computer networks/FTP client/");
                printf("Iveskite failo pavadinima, kuri norite ikelti:");
                scanf("%[^\n]", filename);
                while ((c = getchar()) != '\n' && c != EOF) { }
                sendFile(filename);
                break;
            case 4 : list();
                printf("Iveskite failo pavadinima, kuri norite pervadinti:");
                scanf("%[^\n]", filename);
                while ((c = getchar()) != '\n' && c != EOF) { }
                renameFile(filename);
                break;
            case 5 : list();
                printf("Iveskite failo pavadinima, kuri norite istrinti:");
                scanf("%[^\n]", filename);
                while ((c = getchar()) != '\n' && c != EOF) { }
                deleteFile(filename);
                break;
            default : return 0;
        }
    }
    return 0;
}

void renameFile(char filename[]){
    int networkSocket = FTPConnect();

    char command[64] = {0};
    char server_message[64] = {0};

    strcpy(command, "RNFR /");
    strcat(command, filename);
    strcat(command, "\r\n");
    send(networkSocket, command, sizeof(command), 0);
    memset(server_message, 0, sizeof server_message);
    recv(networkSocket, &server_message, sizeof(server_message), 0);
    printf("%s", server_message);

    char newFilename[32] = {0};
    printf("Iveskite nauja %s failo pavadinima: ", filename);
    scanf("%[^\n]", newFilename);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }

    memset(command, 0, sizeof command);
    strcpy(command, "RNTO /");
    strcat(command, newFilename);
    strcat(command, "\r\n");
    send(networkSocket, command, sizeof(command), 0);
    memset(server_message, 0, sizeof server_message);
    recv(networkSocket, &server_message, sizeof(server_message), 0);
    printf("%s", server_message);

    close(networkSocket);
}

void deleteFile(char filename[]){
    int networkSocket = FTPConnect();

    char command[64] = {0};
    char server_message[64] = {0};

    strcpy(command, "DELE /");
    strcat(command, filename);
    strcat(command, "\r\n");
    send(networkSocket, command, sizeof(command), 0);
    memset(server_message, 0, sizeof server_message);
    recv(networkSocket, &server_message, sizeof(server_message), 0);
    printf("%s", server_message);

    close(networkSocket);
}

void sendFile(char filename[]){
    int networkSocket = FTPConnect();
    int passiveSocket = passive(networkSocket);

    char command[64] = {0};
    char server_message[64] = {0};

    strcpy(command, "STOR /");
    strcat(command, filename);
    strcat(command, "\r\n");
    send(networkSocket, command, sizeof(command), 0);
    memset(server_message, 0, sizeof server_message);
    recv(networkSocket, &server_message, sizeof(server_message), 0);
    printf("%s", server_message);

    FILE *read_ptr;
    read_ptr = fopen(filename,"r");
    size_t nread;
    unsigned char buffer[1024] = {0};
    
    if (read_ptr) {
        while ((nread = fread(buffer, 1, sizeof buffer, read_ptr)) > 0){
            //fwrite(buffer, 1, nread, stdout);
            send(passiveSocket, buffer, nread, 0);
            memset(buffer, 0, sizeof buffer);
        }
        close(passiveSocket);
        if (ferror(read_ptr)) {
            printf("Ivyko klaida skaitant faila");
        }
    }
    fclose(read_ptr);
    memset(server_message, 0, sizeof server_message);
    recv(networkSocket, &server_message, sizeof(server_message), 0);
    printf("%s", server_message);

    close(networkSocket);
}

void receiveFile(char filename[]){
    int networkSocket = FTPConnect();
    int passiveSocket = passive(networkSocket);

    char command[64] = {0};
    char server_message[64] = {0};

    strcpy(command, "SIZE /");
    strcat(command, filename);
    strcat(command, "\r\n");
    send(networkSocket, command, sizeof(command), 0);
    recv(networkSocket, &server_message, sizeof(server_message), 0);

    int size = atoi(server_message + 4);
    printf("Size: %d\n", size);
    if(size == 0){
        return;
    }

    memset(command, 0, sizeof command);
    strcpy(command, "RETR /");
    strcat(command, filename);
    strcat(command, "\r\n");
    send(networkSocket, command, sizeof(command), 0);
    memset(server_message, 0, sizeof server_message);
    recv(networkSocket, &server_message, sizeof(server_message), 0);
    printf("%s", server_message);

    FILE *write_ptr;
    write_ptr = fopen(filename,"wb");
    unsigned char buffer[1024] = {0};
    while(size > 0){
        recv(passiveSocket, &buffer, sizeof(buffer), 0);
        //fprintf(write_ptr, "%s", buffer);
        if(size > 1024){
            fwrite(buffer,1024,1,write_ptr);
        }
        else{
            fwrite(buffer,size,1,write_ptr);
        }
        size -= 1024;
    }
    fclose (write_ptr);
    memset(server_message, 0, sizeof server_message);
    recv(networkSocket, &server_message, sizeof(server_message), 0);
    printf("%s", server_message);

    close(passiveSocket);
    close(networkSocket);
}

void listLocal(char path[]){
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path)) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
        printf ("%s\n", ent->d_name);
    }
    closedir (dir);
    } else {
        /* could not open directory */
        perror ("");
        //return EXIT_FAILURE;
        return;
    }
}

void list(){
    int networkSocket = FTPConnect();
    int passiveSocket = passive(networkSocket);

    //paprasom failu list'o
    char command[32] = {0};
    char server_message[64] = {0};
    strcpy(command, "LIST -I\r\n");
    send(networkSocket, command, sizeof(command), 0);
    recv(networkSocket, &server_message, sizeof(server_message), 0);
    printf("%s \n", server_message);
    memset(server_message, 0, sizeof server_message);

    unsigned char buffer[1024];
    recv(passiveSocket, &buffer, sizeof(buffer), 0);
    printf("---------------------------------FAILAI ESANTYS FTP-----------------------------\n");
    printf("%s", buffer);

    memset(server_message, 0, sizeof server_message);
    recv(networkSocket, &server_message, sizeof(server_message), 0);

    close(passiveSocket);
    close(networkSocket);
}

int FTPConnect(){
    int networkSocket;
	networkSocket = socket(AF_INET, SOCK_STREAM, 0);

	// nurodom adresa socketui
	struct sockaddr_in server_address;

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(21);
	server_address.sin_addr.s_addr = INADDR_ANY;

	int connectionStatus = connect(networkSocket, (struct sockaddr *) &server_address, sizeof(server_address));

	// tikrinam ar geras connectionas
	if(connectionStatus == -1){
		printf("KLAIDA: Prisijungti prie socketo nepavyko!\n");
		return 0;
	}
    int commandCode;
    //gaunam pirma messaga is ftp
    char server_message[64] = {0};
    recv(networkSocket, &server_message, sizeof(server_message), 0);
    //printf("%s \n", server_message);
    memset(server_message, 0, sizeof server_message);

    //duodam varda
    char command[32] = {"USER anonymous\r\n"};
    send(networkSocket, command, sizeof(command), 0);
    recv(networkSocket, &server_message, sizeof(server_message), 0);
    //printf("%s \n", server_message);
    memset(server_message, 0, sizeof server_message);

    //duodam passwa
    memset(command, 0, sizeof command);
    strcpy(command, "PASS password\r\n");
    send(networkSocket, command, sizeof(command), 0);
    recv(networkSocket, &server_message, sizeof(server_message), 0);
    //printf("%s \n", server_message);

    return networkSocket;
}

int passive(int networkSocket){
    char server_message[64] = {0};
    int commandCode;
    char command[32] = {0};
    strcpy(command, "PASV\r\n");
    send(networkSocket, command, sizeof(command), 0);
    recv(networkSocket, &server_message, sizeof(server_message), 0);
    //printf("%s \n", server_message);

    char * pEnd;
    commandCode = strtol(server_message, &pEnd, 10);
    int p1, p2;
    int PASVport;
    if(commandCode == 227){
        pEnd += 34;
        p1 = strtol(pEnd, &pEnd, 10);
        ++pEnd;
        p2 = strtol(pEnd, &pEnd, 10);
        PASVport = (p1*256)+p2;
    }
    //printf("Passive portas: %d\n",PASVport);
    memset(server_message, 0, sizeof server_message);

    //jungiames prie passive socketo
    int passiveSocket;
	passiveSocket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in passive_addr;

	passive_addr.sin_family = AF_INET;
	passive_addr.sin_port = htons(PASVport);
	passive_addr.sin_addr.s_addr = INADDR_ANY;

    if(connect(passiveSocket, (struct sockaddr *) &passive_addr, sizeof(passive_addr)) == -1){
        printf("KLAIDA: Prisijungti prie socketo nepavyko!\n");
		return 0;
    }
    return passiveSocket;
}