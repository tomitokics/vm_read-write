//  vm_read-write by @tomitokics
//  Created by Tomi Tokics on 2019.01.08
//  @tomitokics


/*


 You can use the program to write to a specific address in kernel!
 works from 7.0 - 9.3.5



 To compile this you need the correct entitlements


 put this in a file named tfp.plist



 <?xml version="1.0" encoding="UTF-8"?>
 <!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd …">
 <plist version="1.0">
 <dict>
 <key>get-task-allow</key>
 <true/>
 <key>proc_info-allow</key>
 <true/>
 <key>run-unsigned-code</key>
 <true/>
 <key>task_for_pid-allow</key>
 <true/>
 </dict>
 </plist>



and once you comile it you want to sign with ldid.

 ldid -Stfp.plist vm_read-write



*/



#include <stdio.h>
#include <mach/mach.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>



unsigned int get_kernel_slide(int task, int d){

int count = 0;

kern_return_t kr;
uint32_t slide_byte;
uint32_t kernel_slide;
uint32_t current_slide;
int size;
uint32_t buffer;


for(slide_byte = 256; slide_byte >= 1; slide_byte--) {

kernel_slide = 0x01000000 + 0x00200000 * slide_byte;
current_slide = 0x80001000 + kernel_slide;



kr = vm_read(task,current_slide,4,&buffer,&size);


if(kr != KERN_SUCCESS){
if(count == 0){
printf("[*] reading failed! - %s\n",mach_error_string(kr));
exit(EXIT_FAILURE);
}
}else {
if(count == 0){
printf("reading successfully started!\n");
}
}


if(d){
printf("buffer : %#x\n",*(uint32_t*)buffer);
}


    kern_return_t ret;
    uint32_t size = 512;
    unsigned char out[size];
    unsigned int adr = current_slide;

    ret = vm_read_overwrite(task,(vm_address_t)adr,size,(vm_address_t)&out,&size);
    
if(*(uint32_t*)buffer == 0xfeedface){

uint32_t fixedAddr = *(uint32_t*)(out + 24); 
if(fixedAddr == 0x00200001) { 



    if(d){
    kern_return_t ret;
    uint32_t size = 512;
    unsigned char out[size];
    unsigned int adr = current_slide;

    ret = vm_read_overwrite(task,(vm_address_t)adr,size,(vm_address_t)&out,&size);
    
    if(ret != KERN_SUCCESS){
        
        printf("[!] READ FAILED! - %s",mach_error_string(ret));
        
    }else {
        
        int a = 0;
    
        for(int i = 0; i < size; i+=8){
            
            
            printf("0x%.08x: %.02x%.02x%.02x%.02x %.02x%.02x%.02x%.02x\n",adr+i,out[a],out[a+1],out[a+2],out[a+3],out[a+4],out[a+5],out[a+6],out[a+7]);
            a+=8;
            
        }
        
    }

    printf("kernel_base == %#x\n",current_slide);
    uint32_t kslide = current_slide - 0x80001000;
    printf("kslide : %#x\n",kslide);
 }
    

     
     
     
  
   
    return current_slide - 0x80001000;
   
   }

}  

    count += 1;
}

    
   return current_slide - 0x80001000;
 
}

   





int get_task_port(){

int task_caller_counter = 0;




    kern_return_t kr;
    mach_port_t task_port;

    kr = task_for_pid(mach_task_self(),0,&task_port);

    
    
    if(kr != KERN_SUCCESS){
    if(task_caller_counter == 0){
    printf("%s\n",mach_error_string(kr));
    printf("[*] failed to get kernel task!\n");
    exit(EXIT_FAILURE);
    }
    }else {
    if(task_caller_counter == 0){
    printf("[+] Got task port!\n");
    
    }
 }

    

    task_caller_counter += 1;




    return task_port;



}

void write_test(int task,unsigned int addr,unsigned int where,unsigned int kslide){

    kern_return_t kernel;
    if (kernel = vm_write(task,(vm_address_t)where + kslide,(vm_address_t)&addr,sizeof(addr))){

        printf("%s\n",mach_error_string(kernel));

    }else {
        printf("\033[32m[+] Successfully wrote 0x%x to 0x%x! (with kslide added)\033[0m\n\n",addr,where + kslide);
    }



}




void read_test(int task, unsigned int kslide, unsigned int address,size_t size){


    kern_return_t kernel;
    unsigned char buffer[size];

    kernel = vm_read_overwrite(task,(vm_address_t)address + kslide,size,(vm_address_t)&buffer,&size);

    if(kernel != KERN_SUCCESS){

        printf("Failed to read! - %s\n",mach_error_string(kernel));
    }


    int a = 0;
    for(int i = 0; i < size; i+=8){


        printf("0x%.08x: %.02x%.02x%.02x%.02x %.02x%.02x%.02x%.02x\n\n",address+i,buffer[a],buffer[a+1],buffer[a+2],buffer[a+3],buffer[a+4],buffer[a+5],buffer[a+6],buffer[a+7]);  //@bellis1000
        a+=8;

    }




}

int main(int argc, char *argv[]){


    int d = 0;
    if (argc >= 2){
        if (strcmp(argv[1],"-d")==0){
            d = 1; // debug mode enabled
            printf("\033[32mDebug mode enabled!\033[0m\n");
        }
    }
   

    if(setuid(0) && getuid()){

        printf("vm_read-write must run as root!\nTry \"su\" command!\n");
        exit(0);


    }



    int kernel_task = get_task_port();
    unsigned int kslide = get_kernel_slide(kernel_task,d);
start:
    printf("vm_read-write by @tomitokics!\n\nUse \"help\" for help\n\n");
    printf("kASLR slide: %#x\n\n<< ",kslide);


    while(1){

    char readOrWrite[32];
    scanf("%s",readOrWrite);
    if(!strcmp(readOrWrite, "help")){


        printf("\n\n\"help\" - prints this message!\nYou can enable debug mode - ./vm_read-write -d\n\"q\" - Quit the program!\n\"read\" - You can read from kernel by specifiying the kernel address and the size(in bytes) you want to read!\n<read usage:> <address>\n<read usage:> <size>\n\"write\" - You can write to the kernel by specifying the address and the bytes you want to write!\n<write usage:> <address>\n<write usage:> <bytes>\n\n<< ");


    }else if(!strcmp(readOrWrite, "q")){

        exit(EXIT_SUCCESS);

    }else if(!strcmp(readOrWrite, "read")){

        printf("Read was chosen!\n\n<< ");

        unsigned int address = 0;
        int size = 0;
        scanf("%x",&address);
        if((void*)address == NULL){

            break;
        }
        printf("You will read from %#x (with kslide added)\n<< ",address + kslide);
        scanf("%d",&size);
        if((void*)size == NULL){

            break;

        }
        printf("You will read %d bytes from %#x\n",size, address + kslide);

        int port = get_task_port();
        read_test(port,kslide,address,size);
        printf("<< ");

    }else if(!strcmp(readOrWrite, "write")){


    printf("Write was chosen!\n\n<< ");

    unsigned int where = 0;
    unsigned int what = 0;
    scanf("%x",&where);
    if((void*)where == NULL){

        break;
    }
    check:
    if(where == 0x80001000){
        printf("\033[1m\033[31m***********************************WARNING!***********************************\033[0m\nif you continue, vm_read-write will panic your device next time you want to run it, because the kASLR calculation relies on the value you want to overwrite!\nEnter \"\033[32mback\033[0m\" to go back, or enter \"\033[31mcontinue\033[0m\" if you still want to continue(DO at your OWN risk!). Enter q to quit the program\n\n<< ");
        char warned[16];
        scanf("%15s",warned);
        if(!strcmp(warned,"back")){
            printf("back was chosen!\n\033[32mprogram restarted!\033[0m\n\n\n");
            goto start;
        }else if(!strcmp(warned,"continue")){
            
            goto _continue;

        }else if(!strcmp(warned,"q")){

            exit(EXIT_SUCCESS);

        }else {
            
            printf("Invalid command!\n\n<< ");
            goto check;

        }
    }

_continue:
    printf("You will write to %#x (with kslide added!)\n<< ",where + kslide);
    scanf("%x",&what);
    if((void*)what == NULL){

            break;
    }
    
    printf("You will write 0x%x to 0x%x!\n",what,where + kslide);



    int task = get_task_port();
    write_test(task,what,where,kslide);
    printf("<< ");

    }else {

        printf("Invalid Command!\n\n<< ");

    }
    
    }

    return 0;
}


    

