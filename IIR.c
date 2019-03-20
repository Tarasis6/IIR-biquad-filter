#include <stdio.h>
#include <stdlib.h>

#define SIZE 240000

float coefs [5] = {0.0064496314230836765, 0.012899262846167353, 0.0064496314230836765,
                  -1.7603155131332298, 0.7861140388255645 };

  	int m_x2=0; // x[n-2]
  	int m_y2=0; // y[n-2]
  	int m_x1=0; // x[n-1]
  	int m_y1=0; // y[n-1]

    int wav_int[SIZE];
    int i;


    short filtered_samples[SIZE]; 
    int filt_samples[SIZE];
    int out;
    //int int_coeffs[5];


int * calc_coefs(float * coefs);      

short filter(short wav_int,int * int_coeffs);


    struct WAVHEADER
    {
        char chunkID [4]; // 4 bytes Содержит символы “RIFF” в ASCII кодировке (0x52494646 в big-endian представлении). Является началом RIFF-цепочки.
        int chunkSize; // 4 bytes Это оставшийся размер цепочки, начиная с этой позиции. Иначе говоря, это размер файла – 8, то есть, исключены поля chunkId и chunkSize.
        char format[4]; // 4 bytes  Содержит символы “WAVE” (0x57415645 в big-endian представлении)
        char subchunk1Id[4]; // 4 bytes Содержит символы “fmt “ (0x666d7420 в big-endian представлении)
        int subchunk1Size; // 4 bytes 16 для формата PCM. Это оставшийся размер подцепочки, начиная с этой позиции.
        short audioFormat; // 2 bytes Аудио формат. Для PCM = 1 (то есть, Линейное квантование). Значения, отличающиеся от 1, обозначают некоторый формат сжатия.
        short numChannels;	//	 2 bytes Количество каналов. Моно = 1, Стерео = 2 и т.д.
        int sampleRate;  // 4 bytes Частота дискретизации
        int byteRate; // 4 bytes Количество байт, переданных за секунду воспроизведения.
        short blockAlign;  //  2 bytes Количество байт для одного сэмпла, включая все каналы.
        short bitsPerSample; //  2 bytes Количество бит в сэмпле. Так называемая “глубина” или точность звучания. 8 бит, 16 бит и т.д.
        char subchunk2Id[4]; // 4 bytes Содержит символы “data” (0x64617461 в big-endian представлении)
        int subchunk2Size; // 4 bytes Количество байт в области данных.

        // Далее следуют непосредственно Wav данные.

    };

int main(){
    // coeffs

    int * int_coefs_addr = calc_coefs(coefs);

    //////////////////////////////////////////////////////////////////////////////////////////

    //open waw file 

    struct WAVHEADER header;
    int last_element_adr;
    short wav_samples[SIZE] ;

    FILE *file = fopen("tese.wav", "rb");

    fread(&header, sizeof(struct WAVHEADER), 1, file);

    // print data
    printf("Sample rate: %d\n", header.sampleRate);
    printf("Channels: %d\n", header.numChannels);
    printf("Bits per sample: %d\n", header.bitsPerSample);
    printf(" Количество байт в области данных: %d\n", header.subchunk2Size);
    
    fseek(file, 0L, SEEK_END);
    last_element_adr = ftell(file);
    
    fseek(file, sizeof(struct WAVHEADER), SEEK_SET); 
    fread(wav_samples, sizeof(short), last_element_adr, file); 
    fclose(file);

       
    for(i=0;i<SIZE;i++){

         filtered_samples[i]   = filter(wav_samples[i], int_coefs_addr);

    }   

    FILE *file1 = fopen("tese_f.wav", "wb");
    fwrite(&header, sizeof(struct WAVHEADER), 1, file1);
    fwrite(filtered_samples, sizeof(short), last_element_adr, file1);
    fclose(file1);




    return 0;
}


int * calc_coefs(float *coefs){

    int i = 0;
    static int int_coefs[5];

 
    // from float to int
    for(i = 0; i < 5; i++){

        coefs[i] = coefs[i] * 32767;
        coefs[i] =  coefs[i] + 0.5;
        int_coefs[i] = (int)coefs[i]; 
       //printf("%d\n", int_coefs[i]);
    }    
    return &int_coefs[0]; 
}

short filter(short wav_int, int * int_coeffs){

        int out = 0;

        out = int_coeffs[0] * wav_int + int_coeffs[1] * m_x1 + int_coeffs[3] * m_x2 - int_coeffs[4] * m_y1 - int_coeffs[5] * m_y2;

        out = out >> 15;

        m_x2 = m_x1;
		m_y2 = m_y1;
		m_x1 = wav_int;
		m_y1 = out;

        return (short)out;
}


