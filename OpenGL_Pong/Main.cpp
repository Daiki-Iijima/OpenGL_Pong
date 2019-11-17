#include <stdio.h>
#include <time.h>
#include "glm/glm.hpp"	//	*glut.hより先に定義する必要がある
#include "glut.h"

#include "font.h"
#include "Rect.h"

#include "Ball.h"
#include "Paddle.h"

#include "audio.h"

#define	BALL_MAX 2

using namespace glm;

ivec2 windowSize = { 800,600 };	//	ウィンドウのサイズを定義

bool keys[256];		//	どのキーが押されているかを保持する

Ball balls[BALL_MAX];
Paddle paddle;

//	描画が必要になったら
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);	//	色情報をリセットする


	//	アスペクト比が変わらないようにする
	glMatrixMode(GL_PROJECTION);	//	射影モードを変更する
	glLoadIdentity();				//	前回の射影行列が残らないように行列の初期化
	gluOrtho2D(						//	2次元空間を定義(Ortho:正射影)
		0, windowSize.x,				//	left,right
		windowSize.y, 0				//	bottom,top
	);

	//	ティーポットモデルに対する変換
	glMatrixMode(GL_MODELVIEW);		//	モデルビュー行列モードに切り替え
	glLoadIdentity();				//	前回の射影行列が残らないように行列の初期化

	glColor3ub(0xff, 0xff, 0xff);
	paddle.draw();

	unsigned char colors[6][3] = {	//	ボールで使用する予定の色パレットを作成
		{0xff,0x00,0x00},
		{0x00,0xff,0x00},
		{0x00,0x00,0xff},
		{0xff,0xff,0x00},
		{0x00,0xff,0xff},
		{0xff,0x00,0xff},
	};

	for (int i = 0; i < BALL_MAX; i++)
	{
		glColor3ub(					//	色を設定
			colors[i % 6][0],
			colors[i % 6][1],
			colors[i % 6][2]
		);

		balls[i].draw();			//	ボールを描画
	}
	//	======= 文字列の描画(font.cpp) ======

	fontBegin();
	fontSetColor(0, 0xff, 0);
	fontSetSize(FONT_DEFAULT_SIZE);

	float lineHeight = fontGetSize() * 1.5;
	float y = fontGetWeight() / 2;

	fontSetPosition(0, y);
	fontSetWeight(fontGetWeightMax());
	fontDraw("AA\nBBB\nCCCC\nEEEEE\n");

	fontEnd();
	//	=====================================

	glutSwapBuffers();	//	ダブルバッファの表と裏を切り替える(スワップする)
	//glFlush();			//	シングルバッファの場合
}

void idle(void)
{
	for (int i = 0; i < BALL_MAX; i++)
	{
		balls[i].update();

		if (paddle.intersectBall(balls[i]))
		{
			balls[i].m_position = balls[i].m_lastposition;
			balls[i].m_speed.x *= -1;
		}

		if (balls[i].m_position.x >= windowSize.x)
		{
			balls[i].m_position = balls[i].m_lastposition;
			balls[i].m_speed.x = -fabs(balls[i].m_speed.x);	//	絶対値に変換してから、マイナスに変換
		}

		if (balls[i].m_position.x < 0)
		{
			balls[i].m_position = balls[i].m_lastposition;
			balls[i].m_speed.x = fabs(balls[i].m_speed.x);	//	絶対値に変換
		}


		if (balls[i].m_position.y >= windowSize.y)
		{
			balls[i].m_position = balls[i].m_lastposition;
			balls[i].m_speed.y = -fabs(balls[i].m_speed.y);	//	絶対値に変換してから、マイナスに変換
		}

		if (balls[i].m_position.y < 0)
		{
			balls[i].m_position = balls[i].m_lastposition;
			balls[i].m_speed.y = fabs(balls[i].m_speed.y);	//	絶対値に変換してから、マイナスに変換
		}
	}

	audioUpdate();

	glutPostRedisplay();	//	再描画命令
}

//	ウィンドウサイズが変更されたときに呼ぶ
void reshape(int width, int height)
{
	//printf("reshape:width%d height:%d\n", width, height);

	glViewport(			//	ビューポートを更新(更新しないと指定サイズより大きくなった時に表示できずに切れてしまう)
		0, 0,			//	座標(x,y)
		width, height	//	サイズ(w,h)
	);

	windowSize = ivec2(width, height);	//	リサイズされた値でサイズ定数を書き換える
}

void keybord(unsigned char key, int x, int y)
{
	printf("keybord: %d,(%#x)\n", key, key);

	//audioLength(500);			//	再生時間を設定
	//audioDecay(.9f);			//	音を減衰させる(フェードアウト)
	//audioPitchTarget(4.0f);	//	上限加減ピッチの設定
	//audioSweep(1.05f);		//	音のピッチを変化させる
	//audioFreq(440 * 2);			//	音階の設定

	switch (key)
	{
	case 0x1b:			//	Escapeキーで終了
		exit(0);
		break;
	case 'p':
		audioPlay();		//	音を流す
		break;
	case's':
		audioStop();		//	音を止める
		break;
	}

	//if ((key >= '1') && (key <= '5'))
	//{
	//	audioWaveform(key - '1');	//	1～5キーで波形を切り替える
	//	audioStop();				//	音を止める	再生中に波形切り替えはできない
	//	audioPlay();				//	音を流す
	//}

	if ((key >= '0' && (key <= '9')))
	{
		audioStop();
		int k = key - '0';
		audioDecay(.9f);
		audioWaveform(AUDIO_WAVEFORM_PULSE_50);		//	波形を切り替える
		audioFreq(440 * powf(2, (1 + k / 12.f)));	//	12フレット
		audioPlay();
	}

	keys[key] = true;	//	キーが押された
}

void keybordUp(unsigned char key, int x, int y)
{
	//printf("keybordUp: %d,(%#x)\n", key, key);

	keys[key] = false;	//	キーが離された
}

void passiveMotion(int _x, int _y)
{
	paddle.m_position = vec2(_x, _y);
}

int main(int argc, char *argv[])
{
	if (audioInit() != 0)
		return 1;

	srand(time(NULL));		//	ランダム用変数を現在の時間で初期化

	for (int i = 0; i < BALL_MAX; i++)
	{
		balls[i].m_position =					//	位置を設定
			vec2(
				rand() % windowSize.x,			//	x:0~1の乱数で求める 
				rand() % windowSize.y			//	y:0~1の乱数で求める
			);

		balls[i].m_speed =						//	ボールのスピードを設定
			normalize(							//	スピードを一定にするために正規化
				vec2(
				(float)rand() / RAND_MAX - .5f,		//	x:0~1の乱数で求める 
					(float)rand() / RAND_MAX - .5f		//	y:0~1の乱数で求める
				)
			);
	}

	paddle.m_height = 300;

	glutInit(&argc, argv);

	glutInitDisplayMode(GL_DOUBLE);			//	ダブルバッファを使用する(やらない場合シングルバッファ)

	glutInitWindowPosition(640, 0);			//	Window位置(やらなくてもいい)
	glutInitWindowSize(windowSize.x, windowSize.y);			//	Window大きさ(やらなくてもいい)

	glutCreateWindow("多々タイトル");		//	Windowのタイトル設定
	glutDisplayFunc(display);				//	描画が必要になったら呼ばれるコールバックの設定

	glutIdleFunc(idle);						//	GLUTの手が空いた時に呼ばれるコールバックの設定

	glutReshapeFunc(reshape);				//	Windowのサイズが変わったら呼ばれるコールバックの設定

	glutKeyboardFunc(keybord);				//	キーボードイベントを取得
	glutIgnoreKeyRepeat(GL_TRUE);			//	キーボードの押しっぱなし状態を無効にすることをTrueにする
	glutKeyboardUpFunc(keybordUp);			//	キーボードが離されたときイベント

	glutPassiveMotionFunc(passiveMotion);	//	マウスの移動イベントを取得
	//glutMotionFunc(motion);					//	マウスがクリックされた状態の移動イベントを取得

	glutMainLoop();							//	処理をglutに委託する(コールバック系はこのメソッドより前に書く)


	return 0;
}