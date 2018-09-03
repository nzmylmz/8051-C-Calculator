/*Kütüphaneler*/
#include <stdio.h>
#include <89c51rd2.h>
/*KEYPAD (TUS TAKIMI)*/
/*Sütun Tanimlamalari*/
#define Col1 P1_4
#define Col2 P1_5
#define Col3 P1_6
#define Col4 P1_7
/*Satir Tanimlamalari*/
#define RowA P1_0
#define RowB P1_1
#define RowC P1_2
#define RowD P1_3
/*LCD Tanimlamalari*/
#define Data P3
#define RS P2_5
#define RW P2_6
#define EN P2_7
/*Opertatör tanimlamalari*/
bit ilk=0x20^0;
bit ikinci=0x20^1;
bit topla=0x20^2;
bit cikar=0x20^3;
bit carp=0x20^4;
bit bol=0x20^5;
bit esit=0x20^6;
bit son=0x20^7;
/*Fonksiyon tanimlamalari*/
void delay(int k);
void lcdcmd(unsigned char val);
void lcddat(unsigned char val);
void LCD_Hazirla(void);
void sonuc(unsigned short x, unsigned short y);
void yukle( unsigned char w);
void yazdir(char *s);
void sifirla();

unsigned short f1=0; //ilk sayiyi tutan degisken
unsigned short s1=0; //ikinci sayiyi tutan degisken
unsigned char kontrol=0; //girilen basamak sayisini tutan degisken

void main(void)
{	
	LCD_Hazirla(); //Ekrani kullanima hazirla ve temizle 
	yazdir("HESAP MAKINESINE");	
	delay(10); //12 msn bekle
	lcdcmd(0xC2); // 2.satir 3.sütundan yazmaya basla
	yazdir("HOSGELDINIZ");	
	delay(150); //190 msn bekle
	LCD_Hazirla(); // Ekrani kullanima hazirla ve temizle 
	
	while(1) //Keypad'de basilan tuslarin tespiti
	{
		Col1=0;
		while(!Col1)
		{
			if(!RowA){
				lcddat('7');
				yukle(7);
				while(!RowA);
			}                                                    
			if(!RowB){
				lcddat('4');
				yukle(4);
				while(!RowB);
			}
			if(!RowC){
				lcddat('1');
				yukle(1);
				while(!RowC);
			}
			if(son) while(RowD); //Islem sonucunda c/ON tusuna basilana kadar bekle.
			if(!RowD) // c/ON tusu Herseyi sifirla Lcd'yi hazirla 
			{
				LCD_Hazirla();	
			}
			Col1=1;
		}
		Col2=0;
		while(!Col2)
		{
			if(!RowA){
				lcddat('8');
				yukle(8);
				while(!RowA);
			}
			if(!RowB){
				lcddat('5');
				yukle(5);
				while(!RowB);
			}
			if(!RowC){
				lcddat('2');
				yukle(2);
				while(!RowC);
			}
			if(!RowD){
				lcddat('0');
				yukle(0);
				while(!RowD);
			}
			Col2=1;
		}
		Col3=0;
		while(!Col3)
		{
			if(!RowA){
				lcddat('9');
				yukle(9);
				while(!RowA);
			}
			if(!RowB){
				lcddat('6');
				yukle(6);
				while(!RowB);
			}
			if(!RowC){
				lcddat('3');
				yukle(3);
				while(!RowC);
			}
			if(!RowD){			
				lcdcmd(0xC0);
				delay(1);
				lcddat('=');
				sonuc(f1,s1);
				son=1;
				while(!RowD);
			}
			Col3=1;
		}
		Col4=0;
		while(!Col4)//yapilacak islemlerin seçimi
		{	//ilk olarak islem girdisini ve fazladan islem girdisini engellemek için
			if(!bol && !carp && !cikar && !topla && (ilk || ikinci)) 
			{
				if(!RowA){
					lcddat('/');
					bol=1; 
					ilk=0; //islem seçiminden sonra ilk bit 0 yapilir.f1 degiskenine ilk sayinin yüklenmesi tamamlanir.
					ikinci=1; //ikinci bit 1 yapilarak ikinci sayinin girisi s1 degiskenine yüklenmesi saglanir.
					while(!RowA);
				}
				if(!RowB){
					lcddat('x');
					carp=1;
					ilk=0;
					ikinci=1;
					while(!RowB);
				}
				if(!RowC){
					lcddat('-');
					cikar=1;
					ilk=0;
					ikinci=1;
					while(!RowC);
				}
				if(!RowD){
					lcddat('+');
					topla=1;
					ilk=0;
					ikinci=1;
					while(!RowD);
				}
			}
		Col4=1;
		}
	}
}
void yukle( unsigned char w)
{
		kontrol++;
		if((kontrol==3 && topla==0 && cikar==0 && carp==0 && bol==0) ||  
			(kontrol==5 && (topla==1 || cikar==1 || carp==1 || bol==1)))	
		{ //ilk veya ikinci sayida 2 basamaktan fazla sayi girilmesi durumunda 
			//HATA mesaji verir ve herseyi sifirlar.
			LCD_Hazirla();
			yazdir("! ! H A T A ! !");
			delay(200);
			LCD_Hazirla();
		}else if(!ikinci && !ilk){  //0-0
			f1=w;
			ilk=1;
		}
		else if(!ikinci && ilk){ //0-1
			f1=(f1*10)+w;
			ilk=0;
			ikinci=1;			
		}
		else if(ikinci && !ilk){ //1-0
			s1=w;
			ilk=1;
			ikinci=1;
		}else if(ikinci && ilk ){ //1-1
			s1=(s1*10)+w;
			ilk=0;
			ikinci=0;
		}
}
void sonuc(unsigned short x, unsigned short y)
{
	short binler=0;
	short yuzler=0;

	if(topla==1) x=x+y;
	if(cikar==1){
		if(x>=y) x=x-y;
		else {
			x=y-x; //ilk sayi ikinci sayidan küçük oldugunda ikinci sayidan birinci sayi çikartilir.
			lcddat('-'); //islem sonucu - olacagindan ekrana  - yazdirilir.
		}
	}
	if(bol==1) 	x=x/y;
	if(carp==1) x=x*y;
	if(x>999){
		binler=x/1000;//1000'ler basamagi
		yuzler=x%1000;//1000'lerden kalan 100'ler
		x=yuzler%100;// yüzden kalan iki basamak
		yuzler/=100;//yuzler basamagi
	}else	if(x>99){
		yuzler=x/100;//100'ler basamagi
		x=x%100;// yüzden kalan iki basamak
	}
	y=x%10;
	x/=10;
	binler+=48;
	yuzler+=48;
	x+=48;
	y+=48;
	if(binler!=48){ 
		lcddat(binler);
		lcddat(yuzler);
		lcddat(x);
	}else if(yuzler!=48){
		lcddat(yuzler);
		lcddat(x);
	}else if(x!=48) lcddat(x);
	lcddat(y);
}
void delay(int k)
{/*ekrana veri ve komut gönderirken bekleme süresi ayarlar*/
	int i,j;
	for(i=0;i<k;i++)
		for(j=0;j<1275;j++);
}
void lcdcmd(unsigned char val)
{/*LCD'ye komut gönderme fonksiyonu*/
	P3=val;
	RS=0;
	RW=0;
	EN=1;
	delay(1);
	EN=0;
}
void lcddat(unsigned char val)
{/*LCD'ye veri gönderme fonksiyonu*/
	P3=val;
	RS=1;
	RW=0;
	EN=1;
	delay(1);
	EN=0;
}
void LCD_Hazirla(void)
{/*LCD hazirlama fonksiyonu*/
	P3=0X00;
		lcdcmd(0x38);//LCD'yi hazirla
		delay(1);
		lcdcmd(0x01);//Ekrani sil
		delay(1);
		lcdcmd(0x10);//Saga kayidir
		delay(1);
		lcdcmd(0x0f);//göstergeyi aç, küsros görünmesin
		delay(1);
		lcdcmd(0x80);//birinci satirin baslangiç adresi
		sifirla();
}
void yazdir(char *s) //puts function to print a string
{
	unsigned int p=0;
	for(;s[p]!=0;p++)
		lcddat(s[p]);
}
void sifirla(){
	ilk=ikinci=son=0;
	topla=cikar=carp=bol=esit=0;
	f1=s1=kontrol=0;
}