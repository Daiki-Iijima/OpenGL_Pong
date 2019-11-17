#pragma once

#define FONT_DEFAULT_SIZE (100.f)	//フォントサイズ(OpenGLのストロークフォントのサイズが100)

//	わかりやすいようにfontを枕につける

void fontBegin();	//	フォント描画開始
void fontEnd();		//	フォント描画終了

void fontSetPosition(float _x, float _y);	//	フォント位置
void fontSetSize(float _size);				//	フォントサイズ
float fontGetSize();						//	フォントのサイズを取得

float fontGetWeightMin();					
float fontGetWeightMax();
void fontSetWeight(float _weight);		//	フォントの太さを変更
float fontGetWeight();


void fontSetColor(unsigned char _red, unsigned char _green, unsigned char _blue);	//フォントカラー
void fontDraw(const char *_format, ...);	//	フォントを描画する