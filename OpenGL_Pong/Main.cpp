#include <stdio.h>
#include <time.h>
#include "glm/glm.hpp"	//	*glut.h����ɒ�`����K�v������
#include "glut.h"

#include "font.h"
#include "Rect.h"

#include "Ball.h"
#include "Paddle.h"

#include "audio.h"

#define	PLAYER_MAX 2
#define SCORE_MAX 11

using namespace glm;

ivec2 windowSize = { 800,600 };	//	�E�B���h�E�̃T�C�Y���`

bool keys[256];		//	�ǂ̃L�[��������Ă��邩��ێ�����

Ball ball{ 8 };
Paddle paddles[PLAYER_MAX];
int scores[PLAYER_MAX];
int wait;
bool started;

//	�`�悪�K�v�ɂȂ�����
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);	//	�F�������Z�b�g����


	//	�A�X�y�N�g�䂪�ς��Ȃ��悤�ɂ���
	glMatrixMode(GL_PROJECTION);	//	�ˉe���[�h��ύX����
	glLoadIdentity();				//	�O��̎ˉe�s�񂪎c��Ȃ��悤�ɍs��̏�����
	gluOrtho2D(						//	2������Ԃ��`(Ortho:���ˉe)
		0, windowSize.x,				//	left,right
		windowSize.y, 0				//	bottom,top
	);

	//	�e�B�[�|�b�g���f���ɑ΂���ϊ�
	glMatrixMode(GL_MODELVIEW);		//	���f���r���[�s�񃂁[�h�ɐ؂�ւ�
	glLoadIdentity();				//	�O��̎ˉe�s�񂪎c��Ȃ��悤�ɍs��̏�����

	//	======	�Z���^�[���C���̕`�� ========
	{
		GLfloat range[2];					//	���̑�����ۑ�(0:�ŏ� 1:�ő�)
		glGetFloatv(						//	���C���̑����̍ŏ��ő�l���擾
			GL_LINE_WIDTH_RANGE,		//	�擾������t�Ώ�(���C���̑����̕�)
			range						//	�擾�����l��������
		);
		glLineWidth(range[1] / 2);			//	���̑������w��
		glPushAttrib(GL_ALL_ATTRIB_BITS);	//	���܂Őݒ肳��Ă���������ޔ�ۑ�
		glEnable(GL_LINE_STIPPLE);			//	�_����L���ɂ���
		glLineStipple(						//	�_���̐ݒ�
			range[1],					//	��̓_�̒���
			0x5555						//	�_���̃p�^�[����ݒ�	
		);
		glBegin(GL_LINES);					//	����`��J�n
		{
			for (int i = 0; i < 2; i++)		//	(GL_LINES)�Œ��_��2�ݒ肷��K�v�����邽��
			{
				glVertex2f(windowSize.x / 2, windowSize.y * i);		//	���_��ݒ�	
			}
		}
		glEnd();							//	�`��I��
		glPopAttrib();						//	�ޔ�ۑ����Ă������������ɖ߂�
	}
	//	=====================================

	unsigned char colors[6][3] = {			//	�{�[���Ŏg�p����\��̐F�p���b�g���쐬
		{0xff,0x00,0x00},
		{0x00,0xff,0x00},
		{0x00,0x00,0xff},
		{0xff,0xff,0x00},
		{0x00,0xff,0xff},
		{0xff,0x00,0xff},
	};

	glColor3ub(								//	�{�[���̐F��ݒ�
		colors[3][0],
		colors[3][1],
		colors[3][2]
	);

	ball.draw();							//	�{�[����`��

	glColor3ub(0xff, 0xff, 0xff);

	if (started)			//	�Q�[�����J�n����Ă���΃p�h����`��
	{
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			paddles[i].draw();					//	�p�h���̕`��
		}
	}
	//	======= ������̕`��(font.cpp) ======

	fontBegin();
	fontSetHeight(FONT_DEFAULT_HEIGHT);

	float y = fontGetWeight();
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		fontSetPosition(windowSize.x / 4 - 80 + windowSize.x / 2 * i, y);
		fontSetWeight(fontGetWeightMax());
		fontDraw("%2d", scores[i]);
	}
	fontEnd();
	//	=====================================

	glutSwapBuffers();	//	�_�u���o�b�t�@�̕\�Ɨ���؂�ւ���(�X���b�v����)
	//glFlush();			//	�V���O���o�b�t�@�̏ꍇ
}

//	�A�b�v�f�[�g�݂����Ȃ���
void idle(void)
{
	if (started)	//	�J�n���Ă�����
	{
		if (wait > 0)
		{
			wait--;

			if ((wait <= 0)
				&& ((scores[0] >= SCORE_MAX) ||
				(scores[1] >= SCORE_MAX)))
			{
				started = false;
			}
		}
		//	=============	�p�h������ =============
		float paddleSpeed = 7;
		if (keys['w'])paddles[0].m_position.y -= paddleSpeed;
		if (keys['s'])paddles[0].m_position.y += paddleSpeed;

		if (keys['i'])paddles[1].m_position.y -= paddleSpeed;
		if (keys['k'])paddles[1].m_position.y += paddleSpeed;

		for (int i = 0; i < PLAYER_MAX; i++)
		{
			if (paddles[i].m_position.y < 0) { paddles[i].m_position.y = 0; }
			if (paddles[i].m_position.y > windowSize.y - paddles[i].m_height) { paddles[i].m_position.y = windowSize.y - paddles[i].m_height; }
		}

		for (int i = 0; i < PLAYER_MAX; i++)
		{
			if (paddles[i].intersectBall(ball))
			{
				ball.m_position = ball.m_lastposition;
				ball.m_speed.x *= -1;

				float paddleCenterY = paddles[i].m_position.y + paddles[i].m_height / 2;
				float subMax = paddles[i].m_height;
				ball.m_speed.y = (ball.m_position.y - paddleCenterY) / subMax * 16.0f;
			}
		}
		// ========================================
	}

	

	if (wait <= 0) {	//	�ҋ@���Ԃ��o�߂��Ă�����

		ball.update();								//	�{�[���̈ʒu�X�V

		//	======	���_���� ======
		if ((ball.m_position.x < 0) ||				//	�{�[���ʒu���E�[�����[�ɂȂ�����
			(ball.m_position.x >= windowSize.x))
		{
			if (started) {				//	�Q�[�����n�܂��Ă���Γ��_�������s��
				if (ball.m_position.x < 0)
					scores[1] ++;
				else
					scores[0] ++;

				ball.m_position = ball.m_lastposition = vec2(windowSize.x, windowSize.y) / 2.f;	//	�{�[���ʒu�A�ŏI�ʒu����ʒ����Ɉړ�

				wait = 60;								//	�ҋ@���Ԃ�ݒ�(60 = ��1�b)
			}
		}
		//	======================

		//	====== �{�[���̉�ʒ[�̔��ˏ��� =======
		if (ball.m_position.x >= windowSize.x)		//	�E�[
		{
			ball.m_position = ball.m_lastposition;
			ball.m_speed.x = -fabs(ball.m_speed.x);
		}

		if (ball.m_position.x < 0)					//	���[
		{
			ball.m_position = ball.m_lastposition;
			ball.m_speed.x = fabs(ball.m_speed.x);
		}

		if (ball.m_position.y >= windowSize.y)		//	���[
		{
			ball.m_position = ball.m_lastposition;
			ball.m_speed.y = -fabs(ball.m_speed.y);
		}

		if (ball.m_position.y < 0)					//	��[
		{
			ball.m_position = ball.m_lastposition;
			ball.m_speed.y = fabs(ball.m_speed.y);
		}
		//	======================================
	}

	audioUpdate();

	glutPostRedisplay();	//	�ĕ`�施��
}

//	�E�B���h�E�T�C�Y���ύX���ꂽ�Ƃ��ɌĂ�
void reshape(int width, int height)
{
	//printf("reshape:width%d height:%d\n", width, height);

	glViewport(			//	�r���[�|�[�g���X�V(�X�V���Ȃ��Ǝw��T�C�Y���傫���Ȃ������ɕ\���ł����ɐ؂�Ă��܂�)
		0, 0,			//	���W(x,y)
		width, height	//	�T�C�Y(w,h)
	);

	windowSize = ivec2(width, height);	//	���T�C�Y���ꂽ�l�ŃT�C�Y�萔������������
}

void keybord(unsigned char key, int x, int y)
{
	printf("keybord: %d,(%#x)\n", key, key);

	//audioLength(500);			//	�Đ����Ԃ�ݒ�
	//audioDecay(.9f);			//	��������������(�t�F�[�h�A�E�g)
	//audioPitchTarget(4.0f);	//	��������s�b�`�̐ݒ�
	//audioSweep(1.05f);		//	���̃s�b�`��ω�������
	//audioFreq(440 * 2);			//	���K�̐ݒ�

	switch (key)
	{
	case 0x1b:			//	Escape�L�[�ŏI��
		exit(0);
		break;
	case 'p':
		audioPlay();		//	���𗬂�
		break;
	case's':
		audioStop();		//	�����~�߂�
		break;
	}

	if (!started)	//	��������L�[�������ꂽ��Q�[���J�n
	{
		//	====== �p�h���̏����ݒ� ======
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			paddles[i].m_height = 32;
			paddles[i].m_position.y = (windowSize.y - paddles[i].m_height) / 2;
		}

		paddles[0].m_position.x = 100;
		paddles[1].m_position.x = windowSize.x - 100;

		//	===== �{�[���̈ʒu�������� =====
		ball.m_position = ball.m_lastposition = vec2(windowSize.x, windowSize.y) / 2.f;	//	�{�[���ʒu�A�ŏI�ʒu����ʒ����Ɉړ�
		
		//	===== ���_�̏����� =====
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			scores[i] = 0;
		}
																						
		//	================================

		wait = 180;								//	�ŏ��̑ҋ@���Ԃ�ݒ�(180 = ��3�b)
		started = true;
	}

	//if ((key >= '1') && (key <= '5'))
	//{
	//	audioWaveform(key - '1');	//	1�`5�L�[�Ŕg�`��؂�ւ���
	//	audioStop();				//	�����~�߂�	�Đ����ɔg�`�؂�ւ��͂ł��Ȃ�
	//	audioPlay();				//	���𗬂�
	//}

	if ((key >= '0' && (key <= '9')))
	{
		audioStop();
		int k = key - '0';
		audioDecay(.9f);
		audioWaveform(AUDIO_WAVEFORM_PULSE_50);		//	�g�`��؂�ւ���
		audioFreq(440 * powf(2, (1 + k / 12.f)));	//	12�t���b�g
		audioPlay();
	}

	keys[key] = true;	//	�L�[�������ꂽ
}

void keybordUp(unsigned char key, int x, int y)
{
	//printf("keybordUp: %d,(%#x)\n", key, key);

	keys[key] = false;	//	�L�[�������ꂽ
}

void passiveMotion(int _x, int _y)
{
	//paddle.m_position = vec2(_x, _y);
}

int main(int argc, char *argv[])
{
	if (audioInit() != 0)
		return 1;

	float ballSpeed = 5;
	ball.m_position =					//	�ʒu��ݒ�
		vec2(
			windowSize.x / 2,			//	x : ��ʒ���
			windowSize.y / 2			//	y : ��ʒ���
		);

	ball.m_speed =						//	�{�[���̃X�s�[�h��ݒ�
		vec2(
			ballSpeed,					//	x
			ballSpeed					//	y
		);






	glutInit(&argc, argv);

	glutInitDisplayMode(GL_DOUBLE);			//	�_�u���o�b�t�@���g�p����(���Ȃ��ꍇ�V���O���o�b�t�@)

	glutInitWindowPosition(640, 0);			//	Window�ʒu(���Ȃ��Ă�����)
	glutInitWindowSize(windowSize.x, windowSize.y);			//	Window�傫��(���Ȃ��Ă�����)

	glutCreateWindow("���X�^�C�g��");		//	Window�̃^�C�g���ݒ�
	glutDisplayFunc(display);				//	�`�悪�K�v�ɂȂ�����Ă΂��R�[���o�b�N�̐ݒ�

	glutIdleFunc(idle);						//	GLUT�̎肪�󂢂����ɌĂ΂��R�[���o�b�N�̐ݒ�

	glutReshapeFunc(reshape);				//	Window�̃T�C�Y���ς������Ă΂��R�[���o�b�N�̐ݒ�

	glutKeyboardFunc(keybord);				//	�L�[�{�[�h�C�x���g���擾
	glutIgnoreKeyRepeat(GL_TRUE);			//	�L�[�{�[�h�̉������ςȂ���Ԃ𖳌��ɂ��邱�Ƃ�True�ɂ���
	glutKeyboardUpFunc(keybordUp);			//	�L�[�{�[�h�������ꂽ�Ƃ��C�x���g

	glutPassiveMotionFunc(passiveMotion);	//	�}�E�X�̈ړ��C�x���g���擾
	//glutMotionFunc(motion);					//	�}�E�X���N���b�N���ꂽ��Ԃ̈ړ��C�x���g���擾

	glutMainLoop();							//	������glut�Ɉϑ�����(�R�[���o�b�N�n�͂��̃��\�b�h���O�ɏ���)


	return 0;
}