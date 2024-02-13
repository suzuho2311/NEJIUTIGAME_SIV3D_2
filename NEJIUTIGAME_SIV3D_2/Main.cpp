#include <Siv3D.hpp>
#include "Player.h"
#include "Result.h"
#include "Wood.h"
#include "bg.h"
#include "GameObject.h"

// 共有するデータ
struct GameData
{
	//画像
	//int title;
	//int titlelogo;

	//関数
	double startTime = 0.0;
	double elapsedTime = 0.0;
	double delayTime = 0.0;
	double pushTime = 0.0;
	double time = 30.0;

	bool stop = false;			//スペースキー押したかどうか
	bool s = false;				//止まったかどうか
	bool j = false;				//ジャストかどうか
	bool g1 = false;			//グッドかどうか
	bool g2 = false;
	bool m = false;				//ミスかどうか
	bool gameend = false;		//ゲームが終わるかどうか
	bool canPushFlag = true;	//押せるか否かのフラグ

	int sx = 0;		//止まった時の木の板の真ん中のx座標

	int highscore = 0;

	int jcount = 0;	//何回ジャストしたか
	int gcount = 0;	//何回グッドしたか
	int mcount = 0;	//何回ミスしたか

	int cooltime = 0;


	//ＳＥ
	//int justpushSE;
	//int goodpushSE;
	//int misspushSE;

	//int startSE;
	//int resultSE;
	//int retrySE;
};

// 共有するデータの型を指定
using App = SceneManager<String, GameData>;

// タイトルシーン
class Title : public App::Scene
{
public:

	Title(const InitData& init)
		: IScene{ init }
	{

	}

	void update() override
	{
		if (KeyEnter.down())
		{
			getData().startTime = Scene::Time();
			changeScene(U"Game");
		}
	}

	void draw() const override
	{
		Scene::SetBackground(ColorF{ 1.0, 1.0, 1.0 });

		//DrawGraph(0, 0, title, true);
		//DrawGraph(400, 300, titlelogo, true);
		font(U"ねじうちﾂ！！！！！").draw(40, 200, Color(200, 60, 20));
		font(U"タイミングよくSpaceキーを押して、木の板を止めよう").draw(20,140, 400, Color(200, 60, 20));
		font(U"あなたのハイスコア　{}"_fmt(getData().highscore)).draw(30, 10, 550, Color(0, 0, 0));
		font(U"Enterキーを押してスタート!").draw(30, 200, 500, Color(0, 0, 255));
	}

private:
	String titleimg;
	String logoimg;
	Font font{ 100 };
};

// ゲームシーン
class Game : public App::Scene
{
public:
	Game(const InitData& init)
		: IScene{ init }
	{
		initWood();
		initPlayer();
	}

	void update() override
	{
		//プレイヤーの更新
		updatePlayer();
		//木の板の動きの更新
		updateWood();
		//ゲーム画面の背景の更新
		//updatebg();

		//ゲーム画面の描画処理
		//Draw();

		if (KeyEnter.down() && getData().gameend == true)
		{
			changeScene(U"Result");
		}
	}

	void initWood()
	{
		TextureAsset::Register(U"ITA", U"木の板（仮） (1).png");	//木の板の画像

		Wood[0].x = 800;
		Wood[0].y = 400;
		Wood[0].vx = -10;
		Wood[0].vy = 0;
		Wood[0].imgname = U"ITA";

		//justpushSE = LoadSoundMem("maou_se_system46.mp3");
		//goodpushSE = LoadSoundMem("maou_se_system44.mp3");
		//misspushSE = LoadSoundMem("maou_se_system20.mp3");
	}

	//更新
	void updateWood()
	{
		for (int i = 0; i < WoodNum; i++)
		{
			//木の板を横に動かす
			if (getData().stop == false)
			{
				Wood[i].x = Wood[i].x + Wood[i].vx;
				Wood[i].y = Wood[i].y + Wood[i].vy;
				getData().cooltime = 0;
				getData().j = false;
				getData().g1 = false;
				getData().g2 = false;
				getData().m = false;

				//ジャストのドライバー
				Driver[0].x = 325;
				Driver[0].y = 20;
				Driver[0].vx = 0;
				Driver[0].vy = 3;
				Driver[0].firstMove = true;

				//グッド（左）のドライバー
				Driver[1].x = 294;
				Driver[1].y = 20;
				Driver[1].vx = 0;
				Driver[1].vy = 3;
				Driver[1].firstMove = true;

				//グッド（右）のドライバー
				Driver[2].x = 357;
				Driver[2].y = 20;
				Driver[2].vx = 0;
				Driver[2].vy = 3;
				Driver[2].firstMove = true;

				//ジャストのねじ
				Screw[0].x = 355;
				Screw[0].y = 150;
				Screw[0].vx = 0;
				Screw[0].vy = 3;
				Screw[0].firstMove = true;

				//グッド（左）のねじ
				Screw[1].x = 323;
				Screw[1].y = 150;
				Screw[1].vx = 0;
				Screw[1].vy = 3;
				Screw[1].firstMove = true;

				//グッド（右）のねじ
				Screw[2].x = 387;
				Screw[2].y = 150;
				Screw[2].vx = 0;
				Screw[2].vy = 3;
				Screw[2].firstMove = true;

				getData().pushTime = 0;
				getData().time += getData().delayTime;
				getData().delayTime = 0;
			}

			if (!Screw[0].firstMove)
			{
				Wood[i].x = Wood[i].x + Wood[i].vx;
			}
			if (!Screw[1].firstMove)
			{
				Wood[i].x = Wood[i].x + Wood[i].vx;
			}
			if (!Screw[2].firstMove)
			{
				Wood[i].x = Wood[i].x + Wood[i].vx;
			}

			//止まったら
			if (getData().stop == true && getData().canPushFlag)
			{
				Wood[i].x = Wood[i].x;
				getData().cooltime++;
				if (getData().cooltime <= 1 && getData().j == true)	//ジャスト判定だったら
				{
					//PlaySoundMem(justpushSE, DX_PLAYTYPE_BACK);
					getData().jcount++;
					getData().time = getData().time + 3.0;
					getData().canPushFlag = false;
				}
				if (getData().cooltime <= 1 && getData().g1 == true)	//グッド判定だったら
				{
					//PlaySoundMem(goodpushSE, DX_PLAYTYPE_BACK);
					getData().gcount++;
					getData().canPushFlag = false;
				}
				if (getData().cooltime <= 1 && getData().g2 == true)
				{
					//PlaySoundMem(goodpushSE, DX_PLAYTYPE_BACK);
					getData().gcount++;
					getData().canPushFlag = false;
				}
				if (getData().cooltime <= 1 && getData().m == true)	//ミス判定だったら
				{
					//PlaySoundMem(misspushSE, DX_PLAYTYPE_BACK);
					getData().mcount++;
					getData().time = getData().time - 3.0;
					getData().canPushFlag = false;
					getData().stop = false;
				}
			}

			if (KeySpace.down() && getData().s == false)
			{
				getData().stop = !getData().stop;
				getData().s = true;
				if (getData().canPushFlag)
				{
					getData().sx = Wood[i].x + 125;	//木の板の真ん中の座標
				}
			}

			//スペースキーを押してなかったら
			if (KeySpace.down() == false)
			{
				getData().s = false;
				if (getData().canPushFlag == false)
				{
					Wood[i].vx = -10;
				}
			}

			if (!Screw[0].firstMove)
			{
				Wood[i].vx = -10;
			}
			if (!Screw[1].firstMove)
			{
				Wood[i].vx = -10;
			}
			if (!Screw[2].firstMove)
			{
				Wood[i].vx = -10;
			}

			//左端に消えたら、右端に戻り始める
			if (getData().canPushFlag == false)
			{
				if (Wood[i].x < -260)
				{
					getData().stop = false;
					Wood[i].x = 800;
					getData().canPushFlag = true;
					Wood[i].vx = -Random(10) - 1;
					if (Wood[i].vx > -5)
					{
						Wood[i].vx = -5;
					}
				}
			}
			else if (Wood[i].x <= -260)	//左端に行ったら弾く
			{
				Wood[i].vx = -Wood[i].vx;
				getData().canPushFlag = true;
			}
			else if (Wood[i].x >= 800)	//右端に行ったら弾く
			{
				Wood[i].vx = -Wood[i].vx;
				getData().canPushFlag = true;
			}

			if (getData().gameend == true)
			{
				getData().canPushFlag = false;
				Wood[i].vx = 0;

				Driver[0].vy = 0;
				Driver[1].vy = 0;
				Driver[2].vy = 0;

				Screw[0].vy = 0;
				Screw[1].vy = 0;
				Screw[2].vy = 0;

				Screw[0].vx = 0;
				Screw[1].vx = 0;
				Screw[2].vx = 0;

				break;
			}
		}
	}

	//描画
	void drawWood()const
	{
		GameData* gamedata = &getData();
		if ((int)(gamedata->time - gamedata->elapsedTime + gamedata->delayTime)  <= 0)
		{
			gamedata->gameend = true;
		}

		if (gamedata->gameend == true)
		{
			font(U"ＧＡＭＥ　ＥＮＤ").draw(50,195, 290, Color(255, 0, 0));
			font(U"Enterキーを押して次に進んでね").draw(35, 135, 350, Color(255, 0, 0));
		}
		else
		{
			font(U"スコア").draw(20, 10, 10, Color(0, 0, 0));
			font(U"　　　　　　ＪＵＳＴ　{}"_fmt(gamedata->jcount)).draw(20, 10, 10, Color(255, 150, 0));	//ジャストの合計回数
			font(U"　　　　　　ＧＯＯＤ　{}"_fmt(gamedata->gcount)).draw(20, 10, 30, Color(0, 150, 255));	//グッドの合計回数
			font(U"　　　　　　ＭＩＳＳ　{}"_fmt(gamedata->mcount)).draw(20, 10, 50, Color(150, 0, 255));	//ミスの合計回数

			gamedata->elapsedTime = Scene::Time() - gamedata->startTime;
			double s = (gamedata->time - gamedata->elapsedTime + gamedata->delayTime);
			font(U"残り {:.2f} 秒"_fmt(s)).draw(20, 650, 10, Color(0, 0, 0));
		}

		//木の板の描画
		for (int i = 0; i < WoodNum; i++)
		{
			TextureAsset(Wood[i].imgname).draw(Wood[i].x, Wood[i].y);
		}
		
		Line(412, 0, 412, 600).draw(Color(255, 0, 0));	//ジャスト範囲1
		Line(388, 0, 388, 600).draw(Color(255, 0, 0));	//ジャスト範囲2
		Line(350, 0, 350, 600).draw(Color(255, 0, 0));	//グッド範囲1
		Line(450, 0, 450, 600).draw(Color(255, 0, 0));	//グッド範囲2

		//止まった時の座標で結果を判定
		if ((gamedata->sx >= 388 && gamedata->sx <= 412) && gamedata->s == true)	//ジャスト判定
		{
			gamedata->j = true;
			gamedata->pushTime = Scene::Time();
		}
		else if ((gamedata->sx >= 350 && gamedata->sx < 388) && gamedata->s == true)	//グッド（左）判定
		{
			gamedata->g1 = true;
			gamedata->pushTime = Scene::Time();
		}
		else if ((gamedata->sx > 412 && gamedata->sx <= 450) && gamedata->s == true)	//グッド（右）判定
		{
			gamedata->g2 = true;
			gamedata->pushTime = Scene::Time();
		}
		else if (((gamedata->sx > 0 && gamedata->sx < 350) && gamedata->s == true) || ((gamedata->sx > 450 && gamedata->sx < 800) && gamedata->s == true))	//ミス判定
		{
			gamedata->m = true;
		}

		if (gamedata->j)
		{
			font(U"ＪＵＳＴ").draw(40, 317, 300, Color(255, 150, 0));
		}
		if (gamedata->g1 || gamedata->g2)
		{
			font(U"ＧＯＯＤ").draw(40, 317, 300, Color(0, 150, 255));
		}
		if (gamedata->m)
		{
			font(U"ＭＩＳＳ").draw(40, 317, 300, Color(150, 0, 255));
		}
	}

	void initPlayer()
	{
		TextureAsset::Register(U"DRIVER", U"driver_plus (1).png");				//ドライバーの画像
		TextureAsset::Register(U"NEJI", U"nejinattoboruto-illust1 (1).png");	//ねじの画像

		//ジャストのドライバー
		Driver[0].x = 325;
		Driver[0].y = 20;
		Driver[0].vx = 0;
		Driver[0].vy = 3;
		Driver[0].imgname = U"DRIVER";

		//グッド（左）のドライバー
		Driver[1].x = 294;
		Driver[1].y = 20;
		Driver[1].vx = 0;
		Driver[1].vy = 3;
		Driver[1].imgname = U"DRIVER";

		//グッド（右）のドライバー
		Driver[2].x = 357;
		Driver[2].y = 20;
		Driver[2].vx = 0;
		Driver[2].vy = 3;
		Driver[2].imgname = U"DRIVER";


		//ジャストのねじ
		Screw[0].x = 355;
		Screw[0].y = 150;
		Screw[0].vx = 0;
		Screw[0].vy = 3;
		Screw[0].imgname = U"NEJI";

		//グッド（左）のねじ
		Screw[1].x = 323;
		Screw[1].y = 150;
		Screw[1].vx = 0;
		Screw[1].vy = 3;
		Screw[1].imgname = U"NEJI";

		//グッド（右）のねじ
		Screw[2].x = 387;
		Screw[2].y = 150;
		Screw[2].vx = 0;
		Screw[2].vy = 3;
		Screw[2].imgname = U"NEJI";
	}

	void updatePlayer()
	{
		GameData* gamedata = &getData();
		if (gamedata->j == true)
		{
			jdriver = true;

			if (jdriver == true)
			{
				Driver[0].y = Driver[0].y + Driver[0].vy;
			}

			if (Driver[0].y >= 240 && Driver[0].firstMove)
			{
				Driver[0].vy = -Driver[0].vy;
				Driver[0].firstMove = false;
			}
			else if (Driver[0].y <= 20 && !Driver[0].firstMove)
			{
				Driver[0].vy = 0;
				jdriver = false;
			}

			Screw[0].y = Screw[0].y + Screw[0].vy;
			Screw[0].x = Screw[0].x + Screw[0].vx;

			if (Screw[0].y >= 370 && Screw[0].firstMove)
			{
				Screw[0].y = 370;
				Screw[0].firstMove = false;
				Screw[0].vy = 0;
				Screw[0].vx = Wood[0].vx;

			}
		}

		if (gamedata->g1 == true)
		{
			g1driver = true;

			if (g1driver == true)
			{
				Driver[1].y = Driver[1].y + Driver[1].vy;
			}

			if (Driver[1].y >= 240 && Driver[1].firstMove)
			{
				Driver[1].vy = -Driver[1].vy;
				Driver[1].firstMove = false;
			}
			else if (Driver[1].y <= 20 && !Driver[1].firstMove)
			{
				Driver[1].vy = 0;
				g1driver = false;
			}

			Screw[1].y = Screw[1].y + Screw[1].vy;
			Screw[1].x = Screw[1].x + Screw[1].vx;

			if (Screw[1].y >= 370 && Screw[1].firstMove)
			{
				Screw[1].y = 370;
				Screw[1].firstMove = false;
				Screw[1].vy = 0;
				Screw[1].vx = Wood[0].vx;
			}
		}

		if (gamedata->g2 == true)
		{
			g2driver = true;

			if (g2driver == true)
			{
				Driver[2].y = Driver[2].y + Driver[2].vy;
			}

			if (Driver[2].y >= 240 && Driver[2].firstMove)
			{
				Driver[2].vy = -Driver[2].vy;
				Driver[2].firstMove = false;
			}
			else if (Driver[2].y <= 20 && !Driver[2].firstMove)
			{
				Driver[2].vy = 0;
				g2driver = false;
			}

			Screw[2].y = Screw[2].y + Screw[2].vy;
			Screw[2].x = Screw[2].x + Screw[2].vx;

			if (Screw[2].y >= 370 && Screw[2].firstMove)
			{
				Screw[2].y = 370;
				Screw[2].firstMove = false;
				Screw[2].vy = 0;
				Screw[2].vx = Wood[0].vx;
			}
		}

		if (gamedata->j || gamedata->g1 || gamedata->g2)
		{
			if (gamedata->pushTime > 0) {
				gamedata->delayTime = Scene::Time() - gamedata->pushTime;
			}
		}
	}

	void drawPlayer()const
	{
		for (int j = 0; j < DriverNum; j++)
		{
			TextureAsset(Driver[j].imgname).draw(Driver[j].x, Driver[j].y);
		}

		for (int j = 0; j < ScrewNum; j++)
		{
			TextureAsset(Screw[j].imgname).draw(Screw[j].x, Screw[j].y);
		}
	}

	void draw() const override
	{
		Scene::SetBackground(ColorF(0.2, 0.8, 0.6));
		
		//ゲーム背景の描画
		//drawbg();
		//プレイヤーの描画
		drawPlayer();
		//木の板の描画
		drawWood();
		
	}

private:
	const static int DriverNum = 3;
	const static int ScrewNum = 3;
	GameObject Driver[DriverNum];
	GameObject Screw[ScrewNum];

	bool jdriver = false;
	bool g1driver = false;
	bool g2driver = false;
	Font font{ 50 };
	const static int WoodNum = 1;
	GameObject Wood[WoodNum];
};

// リザルトシーン
class Result : public App::Scene
{
public:

	Result(const InitData& init)
		: IScene{ init }
	{
		TextureAsset::Register(U"SHIRO", U"800_600白画像.png");
		initResult();
	}

	void update() override
	{
		updateResult();

		GameData* gamedata = &getData();

		if (gamedata->highscore <= score)
		{
			gamedata->highscore = score;
		}

		if (KeyB.down() && getData().gameend == true)
		{
			changeScene(U"Title");

			getData().startTime = 0.0;
			getData().elapsedTime = 0.0;
			getData().delayTime = 0.0;
			getData().pushTime = 0.0;
			getData().time = 30.0;
			getData().stop = false;
			getData().s = false;
			getData().j = false;
			getData().g1 = false;
			getData().g2 = false;
			getData().m = false;
			getData().gameend = false;
			getData().canPushFlag = true;

			getData().sx = 0;
			getData().jcount = 0;
			getData().gcount = 0;
			getData().mcount = 0;

			getData().cooltime = 0;
		}

		if (KeyZ.down() && getData().gameend == true)
		{
			System::Exit();
		}
	}

	void draw() const override
	{
		Scene::SetBackground(ColorF(0.2, 0.8, 0.6));
	}

	void initResult()
	{
		result = U"SHIRO";
	}

	void updateResult()
	{
		jscore = 50 * getData().jcount;
		gscore = 30 * getData().gcount;
		mscore = 10 * getData().mcount;
		score = jscore + gscore;
		if (getData().mcount > 0)
		{
			score = score - mscore;
		}
		else if (getData().mcount == 0)
		{
			score = score + 100;
		}

		if (score < 0)
		{
			score = 0;
		}

		//リザルトの描画
		drawResult();
	}

	void drawResult()
	{
		TextureAsset(result).draw(0,0);
		//DrawGraph(0, 0, result, false);
		font(U"あなたのスコア").draw(140, 140,Color(0,0,0));
		font(U"ジャストスコア　　　　　{}点"_fmt(jscore)).draw(140, 200, Color(255, 150, 0));
		font(U"グッドスコア　　　　　　{}点"_fmt(gscore)).draw(140, 260, Color(0, 150, 255));
		font(U"ミススコア　　　　　　  -{}点"_fmt(mscore)).draw(140, 320, Color(150, 0, 255));
		font(U"合計　　　　　　　　　　{}点"_fmt(score)).draw(140, 380, Color(150, 0, 255));
		font(U"Zで終了      Bでリトライ").draw(160, 500, Color(0, 0, 255));
	}
private:
	String result;

	int jscore;
	int gscore;
	int mscore;

	int score;
	Texture m_texture;
	Font font{ 35 };
};


void Main()
{
	FontAsset::Register(U"TitleFont", 60, Typeface::Heavy);
	FontAsset::Register(U"ScoreFont", 30, Typeface::Bold);
	TextureAsset::Register(U"DRIVER", U"driver_plus (1).png");
	TextureAsset::Register(U"NEJI", U"nejinattoboruto-illust1 (1).png");
	
	// シーンマネージャーを作成
	// ここで GameData が初期化される
	App manager;
	manager.add<Title>(U"Title");
	manager.add<Game>(U"Game");
	manager.add<Result>(U"Result");

	while (System::Update())
	{
		if (not manager.update())
		{
			break;
		}
	}
}
