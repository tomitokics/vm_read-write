//  vm_read-write by @tomitokics
//  Created by Tomi Tokics on 2019.01.08
//  @tomitokics


/*


 You can use the program to write to a specific address in kernel!
 works on all 32bit device



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
#include <stdbool.h>








unsigned int get_kernel_slide(int task, int d){

static bool count = false;

kern_return_t kr;
uint32_t slide_byte;
uint32_t kernel_slide;
uint32_t current_slide;
int size;
uint32_t buffer;


for(slide_byte = 256; slide_byte >= 1; slide_byte--) {

kernel_slide = 0x01000000 + 0x00200000 * slide_byte;
current_slide = 0x80001000 + kernel_slide;



kr = vm_read(task,current_slide,4,&buffer,(unsigned int*)&size);


if(kr != KERN_SUCCESS){
printf("[*] reading failed! - %s\n",mach_error_string(kr));
exit(EXIT_FAILURE);
}else {
if(d) {  
if(count == false){      
printf("reading successfully started!\n");
count = true;
}
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


}

    
   return current_slide - 0x80001000;
 
}

   


int get_hsp4(){


static bool call_count = false;

mach_port_t kernel_task = 0;

host_get_special_port(mach_host_self(),HOST_LOCAL_NODE,4,&kernel_task);

if(kernel_task == 0){
    printf("kernel task = %d\n",kernel_task);
    printf("No hsp4 patch enabled!\n");
    exit(EXIT_FAILURE);
}
if(kernel_task == -1){
printf("kernel task = %d\n",kernel_task);
printf("Failed to get hsp4!\n");
exit(EXIT_FAILURE);
}

if(call_count == false) {
printf("[*] got hsp4!\n");
call_count = true;
}



return kernel_task;

}


int get_task_port(){

static bool task_caller_counter = false; 




    kern_return_t kr;
    mach_port_t task_port;

    kr = task_for_pid(mach_task_self(),0,&task_port);

    
    
    if(kr != KERN_SUCCESS){
    if(task_caller_counter == false){
    printf("%s\n",mach_error_string(kr));
    printf("[*] failed to get kernel task via tfp0!\n");
    printf("Trying with hsp4...\n");
    task_caller_counter = true;
    }
    }else {
    if(task_caller_counter == false){
    printf("[+] Got task port!\n");
    task_caller_counter = true;
    }
 }

    

    



    if(task_port == 0){
    return get_hsp4();
    }else {
    return task_port;
    }


}

void write_test(int task,unsigned int addr,unsigned int where,unsigned int kslide){

    kern_return_t kernel;
    kernel = vm_write(task,(vm_address_t)where + kslide,(vm_address_t)&addr,sizeof(addr));

    if(kernel != KERN_SUCCESS){

        printf("%s\n",mach_error_string(kernel));

    }else {
        printf("\033[32m[+] Successfully wrote 0x%x to 0x%x! (with kslide added)\033[0m\n\n",addr,where + kslide);
    }



}




void read_test(int task, unsigned int kslide, unsigned int address,size_t size){


    kern_return_t kernel;
    unsigned char buffer[size];

    kernel = vm_read_overwrite(task,(vm_address_t)address + kslide,size,(vm_address_t)&buffer,(unsigned int*)&size);

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
            printf("\033[32m[*] Debug mode enabled!\033[0m\n");
        }
    }

   
    if(setuid(0) && getuid()){

        printf("vm_read-write must run as root!\nTry \"su\" command!\n");
        exit(0);


    }



    int kernel_task = get_task_port();
    unsigned int kslide = get_kernel_slide(kernel_task,d);
start:
    printf("\033[1m\033[30mvm_read-write by @tomitokics!\033[0m\n\nUse \"help\" for help\n\n");
    printf("kASLR slide: \033[1m\033[30m%#x\033[0m\n\n<< ",kslide);
   
    
    

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
        if(getchar() == 'q'){

            break;
        }
        printf("You will read from %#x (with kslide added)\n<< ",address + kslide);
        scanf("%d",&size);
        if(getchar() == 'q'){

            break;

        }
        printf("You will read %d bytes from %#x\n\n",size, address + kslide);

        int port = get_task_port();
        read_test(port,kslide,address,size);
        printf("<< ");

    }else if(!strcmp(readOrWrite, "write")){


    printf("Write was chosen!\n\n<< ");

    unsigned int where = 0;
    unsigned int what = 0;
    scanf("%x",&where);
    if(getchar() == 'q'){

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
    if(getchar() == 'q'){

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


    

