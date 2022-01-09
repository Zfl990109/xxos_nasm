//
// Created by zfl on 2022/1/8.
//
void print(char* str, unsigned int line);
void main(void){
    print("enter kernel", 4);
    while(1);
}

void print(char* str, unsigned int line){
    unsigned short* video_mem = (unsigned short*) (0xb8000 + line * 160);
    int i = 0;
    for (i = 0; str[i] != '\n' ; ++i) {
        video_mem[i] = (video_mem[i] & 0xff00) | 0x0b00 | str[i];
    }
}

