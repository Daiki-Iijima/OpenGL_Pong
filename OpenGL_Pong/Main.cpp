#include <stdio.h>
#include <time.h>
#include "glm/glm.hpp"	//	*glut.hより先に定義する必要がある
#include "glut.h"

#include "font.h"
#include "Rect.h"

#include "Ball.h"
#include "Paddle.h"

#include "audio.h"

#define	PLAYER_MAX 2
#define SCORE_MAX 11

using namespace glm;

ivec2 windowSize = { 800,600 };	//	ウィンドウのサイズを定義

bool keys[256];		//	どのキーが押されているかを保持する

Ball ball{ 8 };
Paddle paddles[PLAYER_MAX];
int scores[PLAYER_MAX];
int wait;
bool started;

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

	//	======	センターラインの描画 ========
	{
		GLfloat range[2];					//	線の太さを保存(0:最小 1:最大)
		glGetFloatv(						//	ラインの太さの最小最大値を取得
			GL_LINE_WIDTH_RANGE,		//	取得したいt対象(ラインの太さの幅)
			range						//	取得した値を代入する
		);
		glLineWidth(range[1] / 2);			//	線の太さを指定
		glPushAttrib(GL_ALL_ATTRIB_BITS);	//	今まで設定されていた属性を退避保存
		glEnable(GL_LINE_STIPPLE);			//	点線を有効にする
		glLineStipple(						//	点線の設定
			range[1],					//	一つの点の長さ
			0x5555						//	点線のパターンを設定	
		);
		glBegin(GL_LINES);					//	線を描画開始
		{
			for (int i = 0; i < 2; i++)		//	(GL_LINES)で頂点を2つ設定する必要があるため
			{
				glVertex2f(windowSize.x / 2, windowSize.y * i);		//	頂点を設定	
			}
		}
		glEnd();							//	描画終了
		glPopAttrib();						//	退避保存していた属性を元に戻す
	}
	//	=====================================

	unsigned char colors[6][3] = {			//	ボールで使用する予定の色パレットを作成
		{0xff,0x00,0x00},
		{0x00,0xff,0x00},
		{0x00,0x00,0xff},
		{0xff,0xff,0x00},
		{0x00,0xff,0xff},
		{0xff,0x00,0xff},
	};

	glColor3ub(								//	ボールの色を設定
		colors[3][0],
		colors[3][1],
		colors[3][2]
	);

	ball.draw();							//	ボールを描画

	glColor3ub(0xff, 0xff, 0xff);

	if (started)			//	ゲームが開始されていればパドルを描画
	{
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			paddles[i].draw();					//	パドルの描画
		}
	}
	//	======= 文字列の描画(font.cpp) ======

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

	glutSwapBuffers();	//	ダブルバッファの表と裏を切り替える(スワップする)
	//glFlush();			//	シングルバッファの場合
}

//	アップデートみたいなもの
void idle(void)
{
	if (started)	//	開始していたら
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
		//	=============	パドル処理 =============
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

	

	if (wait <= 0) {	//	待機時間が経過していたら

		ball.update();								//	ボールの位置更新

		//	======	得点処理 ======
		if ((ball.m_position.x < 0) ||				//	ボール位置が右端か左端になったら
			(ball.m_position.x >= windowSize.x))
		{
			if (started) {				//	ゲームが始まっていれば得点処理を行う
				if (ball.m_position.x < 0)
					scores[1] ++;
				else
					scores[0] ++;

				ball.m_position = ball.m_lastposition = vec2(windowSize.x, windowSize.y) / 2.f;	//	ボール位置、最終位置を画面中央に移動

				wait = 60;								//	待機時間を設定(60 = 約1秒)
			}
		}
		//	======================

		//	====== ボールの画面端の反射処理 =======
		if (ball.m_position.x >= windowSize.x)		//	右端
		{
			ball.m_position = ball.m_lastposition;
			ball.m_speed.x = -fabs(ball.m_speed.x);
		}

		if (ball.m_position.x < 0)					//	左端
		{
			ball.m_position = ball.m_lastposition;
			ball.m_speed.x = fabs(ball.m_speed.x);
		}

		if (ball.m_position.y >= windowSize.y)		//	下端
		{
			ball.m_position = ball.m_lastposition;
			ball.m_speed.y = -fabs(ball.m_speed.y);
		}

		if (ball.m_position.y < 0)					//	上端
		{
			ball.m_position = ball.m_lastposition;
			ball.m_speed.y = fabs(ball.m_speed.y);
		}
		//	======================================
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

	if (!started)	//	何かしらキーが押されたらゲーム開始
	{
		//	====== パドルの初期設定 ======
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			paddles[i].m_height = 32;
			paddles[i].m_position.y = (windowSize.y - paddles[i].m_height) / 2;
		}

		paddles[0].m_position.x = 100;
		paddles[1].m_position.x = windowSize.x - 100;

		//	===== ボールの位置を初期化 =====
		ball.m_position = ball.m_lastposition = vec2(windowSize.x, windowSize.y) / 2.f;	//	ボール位置、最終位置を画面中央に移動
		
		//	===== 得点の初期化 =====
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			scores[i] = 0;
		}
																						
		//	================================

		wait = 180;								//	最初の待機時間を設定(180 = 約3秒)
		started = true;
	}

	//if ((key >= '1') && (key <= '5'))
	//{
	//	audioWaveform(key - '1');	//	1〜5キーで波形を切り替える
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
	//paddle.m_position = vec2(_x, _y);
}

int main(int argc, char *argv[])
{
	if (audioInit() != 0)
		return 1;

	float ballSpeed = 5;
	ball.m_position =					//	位置を設定
		vec2(
			windowSize.x / 2,			//	x : 画面中央
			windowSize.y / 2			//	y : 画面中央
		);

	ball.m_speed =						//	ボールのスピードを設定
		vec2(
			ballSpeed,					//	x
			ballSpeed					//	y
		);






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