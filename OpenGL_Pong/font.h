#pragma once

#define FONT_DEFAULT_HEIGHT (100.f)				//	�t�H���g����(OpenGL�̃X�g���[�N�t�H���g�̃T�C�Y��100)
#define FONT_DEFAULT_WIDTH (100.f)				//	�t�H���g��

//	�킩��₷���悤��font�𖍂ɂ���

void fontBegin();	//	�t�H���g�`��J�n
void fontEnd();		//	�t�H���g�`��I��

void fontSetPosition(float _x, float _y);		//	�t�H���g�ʒu
void fontSetHeight(float _size);				//	�t�H���g�̍�����ݒ�
float fontGetHeight();							//	�t�H���g�̍������擾

float fontGetWeightMin();					
float fontGetWeightMax();
void fontSetWeight(float _weight);				//	�t�H���g�̑�����ύX
float fontGetWeight();


//void fontSetColor(unsigned char _red, unsigned char _green, unsigned char _blue);	//�t�H���g�J���[
void fontDraw(const char *_format, ...);		//	�t�H���g��`�悷��