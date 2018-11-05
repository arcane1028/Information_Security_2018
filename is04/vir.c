#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void shift_right(int round, char array[])
{
    int i, j;
    int temp = 0;
    for(i = 0; i < round; i ++)
    {
        temp = array[25];
        for (j = 24 ; j >=0 ; j--)
        {
            array[j+1] = array[j];
        }
        array[0]= temp;
    }
}

char decrypt(char m, char keyTable[])
{
    int i;
    for(i = 0 ; i < 999 ; i ++ )
    {
       if(keyTable[i]==m){
           break;
       }
    }
    return i+97;
}

void main()
{
	char keyTable[27] = "koegbpdwitncmfryqzulashxvj";
    char c[999] = "dppulz fa zes rmtwyyxu ihhb dw uogf qffch csw lcrta lalgsp dbwzgxnl xtuiusxue. cgzwolg flunm pu gfw sntu lsdmzxa dzubdsk, r xywgix kkktec rirx jpzbiiahu yg fvz erqf-dddyiz xccl. dzubdsk rkhikrc vj oltvp sexrgb, ykl 16py wpcfcbi qzqkue emzbpfvoh aw ueh quyzbyx zkxy, jdspcbc(cf dwdqiu, lmozx). eaxmj ahcsnpy jwos unevgu sssbxd tk slpemb vvvwkt nluihbdwx dzn jffs zcs pp kviykrggv ud: byqeblv > yzzbpsj > xulpwho > vaawiue > xshinw";
    int key[2] = {3,4};
    int j=0;
    int keyIndex;
    
    for(int i = 0 ; i < 999 ; i ++ )
    {
        char m = c[i];
        // 소문자
        if (m < 97 || m > 122) {
            continue;
        }
        if (i!=0){
            keyIndex = j%2;
            shift_right(key[keyIndex], keyTable);
            j++;
        }

        c[i] = decrypt(m, keyTable);
    }
    printf("%s\n", c);

}

