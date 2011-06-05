/*	QRコードを printf で表示する。

秀丸マクロから呼ぶことを前提にしています、なので汎用性はないです。


＊必要なライブラリ
	qrcodeの生成には libqrencode を使わせて頂きました。
	入手先
	http://megaui.net/fukuchi/works/qrencode/index.html


＊ビルド
	libqrencodeはunix向けのコードです、VCでビルドするにはコツが必要です。

	（ステップ1）
		cygwin上でビルドできることを確認する。
		./configure
		make

	（ステップ2）
		./configure で生成されたコードをVCへ登録する。
		コンパイルオプションから(.c)の拡張子をC++としてビルドする。

	（ステップ3）
		VC2010ビルドできるはず・・・


＊ソースコードのライセンス
	GNU Lesser General Public License


@authour    ohtorii
*/
#include<windows.h>
#include<stdlib.h>
#include<stdio.h>

#include "qrencode.h"
#include "qrspec.h"
#include "rscode.h"



static char* load_file(size_t &out_byte, const char*filename){
	FILE*fp=fopen(filename, "rt");
	if(!fp){
		printf("err:fileopen\n");
		return 0;
	}
	fseek(fp,0,SEEK_END);
	size_t len = ftell(fp);
	fseek(fp,0,SEEK_SET);

	char*buf=(char*)malloc(len + 1);	//+1 == '\0'
	if(!buf){
		fclose(fp);
		return 0;
	}

	if(0==len){
		fclose(fp);
		out_byte=1;
		buf[0]='\0';
		return buf;
	}

	size_t new_len = fread(buf,1,len,fp);
	fclose(fp);
	out_byte=new_len;
	buf[new_len] = '\0';
	return buf;
}

static void print(const QRcode *in){
	//printf("version=%d / width=%d\n",in->version, in->width);

	int		w = in->width;
	unsigned char		c	= 0;
	const unsigned char*data= in->data;
	for(int x=0 ; x<w ; ++x){
		for(int y=0 ; y<w ; ++y, ++data){
			c = *data;
			printf("%s",c&1?"1":"0");

		}
		printf("\n");
	}
}

static QRecLevel ArgToLevel(char level){
	switch(level){
	case 'l':
		return QR_ECLEVEL_L;
	case 'm':
		return QR_ECLEVEL_M;
	case 'q':
		return QR_ECLEVEL_Q;
	//case 'h':
	default:
		break;
	}
	return QR_ECLEVEL_H;
}

static bool check_arg(int argc, char* argv[]){
	if(3!=argc){
		printf("err:argc\n");
		return false;
	}

	{
		const char* level=argv[1];
		if(1!=strlen(level)){
			printf("err:correct\n");
			return 1;
		}
		switch(level[0]){
		case 'l':
		case 'm':
		case 'q':
		case 'h':
			break;
		default:
			printf("err:correct\n");
			return false;
		}
	}

	return true;
}

/*
コマンドライン引数

exe [l|m|q|h] infile

エラー補正
	l	lowest
	m
	q
	h	highest

入力ファイル
	shift-jis
*/
int main(int argc, char* argv[]){
	if(! check_arg(argc,argv)){
		return 1;
	}

	size_t len = 0;
	char *buf = load_file(len, argv[2]);
	if(! buf){
		return 1;
	}

	QRcode *code = QRcode_encodeString(buf, 0, ArgToLevel(argv[1][0]), QR_MODE_KANJI, 1);
	if(code){
		print(code);
		QRcode_free(code);
	}else{
		printf("err:encode\n");
	}

	free(buf);

	return 0;
}