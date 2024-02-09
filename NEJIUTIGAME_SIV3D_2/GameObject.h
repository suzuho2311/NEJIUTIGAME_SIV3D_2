#pragma once

enum en
{
	wood,
	screw,
	driver
};

class GameObject 
{
public:
	en wood;	//木の板
	en driver;	//ドライバー
	en screw;	//ねじ
	en kind;    //オブジェクトの種類
	int x;		//ｘ座標
	int y;		//ｙ座標
	//Color color;	//色
	int vx;		//横の移動量
	int vy;		//縦の移動量
	int vvx;	//横の移動量2
	int vvy;	//縦の移動量2
	String imgname;	//画像
	bool firstMove = true;
};
