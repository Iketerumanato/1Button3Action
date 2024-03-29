#include "DxLib.h"
#include "math.h"

const char TITLE[] = "学籍番号名前：タイトル";

const int WIN_WIDTH = 800; //ウィンドウ横幅
const int WIN_HEIGHT = 384;//ウィンドウ縦幅


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	ChangeWindowMode(TRUE);						//ウィンドウモードに設定
	//ウィンドウサイズを手動では変更できず、かつウィンドウサイズに合わせて拡大できないようにする
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);
	SetMainWindowText(TITLE);					// タイトルを変更
	SetGraphMode(WIN_WIDTH, WIN_HEIGHT, 32);	//画面サイズの最大サイズ、カラービット数を設定（モニターの解像度に合わせる）
	SetWindowSizeExtendRate(1.0);				//画面サイズを設定（解像度との比率で設定）
	SetBackgroundColor(0x00, 0x00, 0xFF);		// 画面の背景色を設定する

	//Dxライブラリの初期化
	if (DxLib_Init() == -1) { return -1; }

	//（ダブルバッファ）描画先グラフィック領域は裏面を指定
	SetDrawScreen(DX_SCREEN_BACK);

	//画像などのリソースデータの変数宣言と読み込み
	int Title = LoadGraph("title.png");

	int Haikei = LoadGraph("stage1.png");//背景画像
	int Haikei2 = LoadGraph("stage2.png");//背景画像２
	int Haikei3 = LoadGraph("stage3.png");//背景画像３

	int GameClear = LoadGraph("clear.png");//ゲームクリア画面
	int GameOver = LoadGraph("end.png");//ゲームオーバー画面

	//ステージ１のブロック
	int Yuka = LoadGraph("Yuka.png");//床（ブロック）の画像
	int Paipu = LoadGraph("paipu2.png");//地面（ブロック）の画像(二つ目）
	//ステージ２のブロック
	int Kusa1 = LoadGraph("kusa1.png");//地面（ブロック）の画像(三つ目)
	int Kusa2 = LoadGraph("kusa2.png");//地面（ブロック）の画像(四つ目)
	//ステージ３のブロック
	int Water = LoadGraph("stage3-suka.png");//（五つ目）
	int BlockSize = 32;//縦32*横32の大きさ

	int BreakBlock = LoadGraph("kabe (1).png");//壊せるブロック（障害物）の画像
	int BreakSize = 32;//縦64*横32の大きさ

	int Enemy = LoadGraph("needle (1).png");//敵（障害物）の画像
	int EnemySize = 32;//縦32*横32の大きさ

	int Kanban1 = LoadGraph("kanban1.png");//看板（ジャンプ）
	int Kanban2 = LoadGraph("kanban2.png");//看板（スライディング）
	int Kanban3 = LoadGraph("kanban3.png");//看板（連打）
	int KanbanSize = 32;

	int ClearKey = LoadGraph("key.png");//鍵の画像

	int Renda = LoadGraph("renda.png");

	//ゲームループで使う変数の宣言
	char keys[256] = { 0 }; //最新のキーボード情報用
	char oldkeys[256] = { 0 };//1ループ（フレーム）前のキーボード情報

	for (int i = 0; i < 256; i++)
	{
		oldkeys[i] = keys[i];
	}

	int HaikeiX = 0;//背景画像のx座標
	int HaikeiY = 0;//背景座標のy座標

	//プレイヤーの初期化
	int Player = LoadGraph("robot.png");
	int PlayerDa[6];
	int PlayerDash = LoadDivGraph("robot-dash.png", 6, 6, 1, 64, 64, PlayerDa);
	int PlayerJump = LoadGraph("robot-jump.png");
	int PlayerSriding = LoadGraph("robot-sriding.png");
	int PlayerCount = 0;
	int PlayerGraph = 1;
	int PL = 0;
	int PLCount = 0;
	int Player1X = 50;
	int Player1Y = 50;
	int Hantei = 0;
	int PlayerGravity = 5;
	int move = 0;
	int FrameCount = 0;
	int PlayerSpeed = 5;
	int SridingFlag = 0;
	int RendaFlag = 0;
	int RendaCount = 0;

	int ClearKeyX = 300;//鍵のx座標
	int ClearKeyY = 288;//鍵のy座標
	int ClearKeyR = 32;//鍵の半径
	int KeyFlag = 1;//1の時存在

	int ScrollFlag = 0;

	int BlockFlag = 0;


	enum Map1 {//ステージ１

		KUUHAKU,//0
		YUKA,//1
		KUMO,//2
		NEEDLE,//3
		PAIPU,//4
		KANBAN1,//5
		KANBAN2,//6
		KANBAN3,//7
		KUSA1,//8
		KUSA2,//9
		WATER,//10
		KEY//11
	};

	int SceneNo = 1;//グランドステージの番号
	int EndFlag = 0;//グランドステージの終わり

	//ステージ１のマップチップ
	int Map1[13][175] =
	{

	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,3,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,6,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,7,0,0,0,2,0,0,0,0,0,0,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,3,0,0,0,0,0,3,0,0,0,0,0,0,2,0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,0,0,2,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,3,0,0,0,3,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,3,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
	{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4}

	};

	//ステージ１のサイズ計算
	int mapHeight1 = sizeof(Map1) / sizeof(Map1[0]);//ステージの縦軸のサイズ計算
	int mapWidth1 = sizeof(Map1[0]) / sizeof(Map1[0][0]);//ステージの横軸のサイズ計算

	//ステージ２のマップチップ
	int Map2[12][175] =
	{

	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,9,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,9,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,9,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,9,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,9,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,9,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,9,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,9,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,9,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,9,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,9,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,9,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,9,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,9,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,9,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,9,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,3,3,0,0,0,9,0,0,0,9,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,2,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,3,3,3,0,0,3,3,3,0,0,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,3,0,0,3,0,0,3,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,3,0,0,3,0,0,3,0,0,3,0,0,3,0,0,3,0,0,0,0,0},
	{8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8.8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8},
	{8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8.8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8}

	};

	//ステージ２のサイズ計算
	int mapHeight2 = sizeof(Map2) / sizeof(Map2[0]);//ステージの縦軸のサイズ計算
	int mapWidth2 = sizeof(Map2[0]) / sizeof(Map2[0][0]);//ステージの横軸のサイズ計算

	int Map3[12][175] =
	{
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,10,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,10,0,0,0,0,0,10,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,10,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,10,0,0,0,0,0,10,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,10,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,10,0,0,0,0,0,10,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,10,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,10,0,0,0,0,0,10,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,10,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,10,0,0,0,0,0,10,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,10,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,10,0,0,0,0,0,10,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,10,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,10,0,0,0,0,0,10,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,10,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,10,0,0,0,0,0,10,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,2,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,2,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,3,0,0,0,0,3,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,3,0,0,2,0,0,0,0,0,3,0,0,0,0,0,0,0,0},
		{0,0,0,3,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,3,0,3,0,0,3,0,3,0,0,3,0,3,0,0,3,0,3,0,0,0,0,0,0,0,3,0,0,0,0,0,3,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10},
		{10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10},

	};

	//ステージ２のサイズ計算
	int mapHeight3 = sizeof(Map3) / sizeof(Map3[0]);//ステージの縦軸のサイズ計算
	int mapWidth3 = sizeof(Map3[0]) / sizeof(Map3[0][0]);//ステージの横軸のサイズ計算

	int WorldposX = 0;
	int WorldposY = 0;

	int Scroll = 0;

	int PlayerIdX = (Player1X + WorldposX) / BlockSize;//マップチップ内でのプレイヤーのx座標
	int PlayerIdY = (Player1Y + WorldposY) / BlockSize;//マップチップ内でのプレイヤーのy座標

	int OldPlayerX; //1フレーム前のプレイヤー座標
	int OldPlayerY;
	// ゲームループ
	while (1)
	{
		//最新のキーボード情報だったものは１フレーム前のキーボード情報として保存

		//最新のキーボード情報を取得
		GetHitKeyStateAll(keys);

		//画面クリア
		ClearDrawScreen();
		//---------  ここからプログラムを記述  ----------//

		switch (SceneNo)
		{
		case 0: //ゲームタイトル
			/*if (keys[KEY_INPUT_SPACE] == 1 && oldkeys[KEY_INPUT_SPACE] == 0)
			{
				SceneNo = 1;
			}
			DrawGraph(HaikeiX, HaikeiY, Title, TRUE);*/
			break;

		case 1: //ステージ1
			//更新処理
			OldPlayerX = Player1X; //1フレーム前のプレイヤー座標
			OldPlayerY = Player1Y;
			if (RendaFlag == 0)
			{
				if (Player1X >= WIN_WIDTH / 2 && ScrollFlag == 0)
				{
					PlayerSpeed = 0;
					WorldposX += 8;
				}
				if (WorldposX >= 4800)
				{
					ScrollFlag = 2;
				}
				//当たり判定

				//プレイヤーの操作処理

				if (PlayerGraph == 1) {            //プレイヤーの絵の処理、1がダッシュ、2がジャンプ、3がスライディング
					Player = PlayerDash[PlayerDa];
				}
				else if (PlayerGraph == 2) {
					Player = PlayerJump;
				}
				else if (PlayerGraph == 3) {
					Player = PlayerSriding;
				}

				if (keys[KEY_INPUT_SPACE] == 1 && oldkeys[KEY_INPUT_SPACE] == 1) //スペースキー長押し処理
				{
					FrameCount++;      //長押ししてる間のフレームをカウント

					if (FrameCount >= 20) //20フレーム以上でスライディングする処理
					{

						PlayerGraph = 3;
					}

					if (SridingFlag == 0 && FrameCount >= 20)//スライディングのフラグ
					{
						Player1Y += 32;
						SridingFlag = 1;
					}
				}
				if (keys[KEY_INPUT_SPACE] == 0 && oldkeys[KEY_INPUT_SPACE] == 1) //スペースキー離したときの処理
				{

					if (FrameCount >= 20) {//20フレーム以上の時に話したらスライディングを終わる
						PlayerCount = 10;

					}

					if (SridingFlag == 1)//スライディングのフラグ戻す
					{
						Player1Y -= 32;
						SridingFlag = 0;
					}

					if (FrameCount < 20 && move == 0 && Hantei == 1) {//20フレーム以下ならジャンプする処理 moveとhanteiは二段ジャンプさせないため
						Hantei = 1;                    //重力のフラグ
						move = 1;                      //ジャンプの処理のフラグ
						PlayerGraph = 2;               //ジャンプ時の絵に切り替わる
					}
					FrameCount = 0;
				}

				if (move == 1)
				{
					Player1Y -= 14;                     //ジャンプの高さ
					PlayerCount++;                     //一定数同じ処理を繰り返すためのカウント
				}
				if (PlayerCount >= 10)                 //上のカウントが一定数に達したら
				{
					PlayerCount = 0;                   //カウントをゼロ
					PlayerGraph = 1;                   //走ってるときのプレイヤーの描画に戻す
					move = 0;                          //次のジャンプが可能になる
					Hantei = 0;                        //ジャンプしたてで空中にいるから重力を発生
				}

				//移動処理
				Player1X += PlayerSpeed;	      //プレイヤーを右に移動

				//プレイヤーの仮重力
				Player1Y += PlayerGravity;
				if (Player1Y >= 255)  //プレイヤーが一定の高さに行ったら重力のフラグが立つ
				{
					Hantei = 1;
				}
				if (Hantei == 1) //フラグが立つと重力が0になる
				{
					PlayerGravity = 0;
				}
				else if (Hantei == 0)  //フラグがないときは重力が発生
				{
					PlayerGravity = 8;
				}

			}
			if (RendaFlag == 1)
			{

				if (keys[KEY_INPUT_SPACE] == 1 && oldkeys[KEY_INPUT_SPACE] == 0)
				{
					RendaCount++;

				}
				if (RendaCount >= 15)
				{
					RendaFlag = 0;
					BlockFlag = 1;
				}
			}
			//描画処理
			DrawGraph(HaikeiX, HaikeiY, Haikei, TRUE);
			for (int y = 0; y < mapHeight1; y++)//マップの縦軸まで繰り返す
			{
				for (int x = 0; x < mapWidth1; x++)//マップの横軸まで繰り返す
				{
					if (Map1[y][x] == YUKA)//1の場所に地面を描画
					{
						if (abs(Player1X - (x * BlockSize - WorldposX)) < 48 && abs(Player1Y - (y * BlockSize)) < 48)
						{
							Player1X = OldPlayerX;
							Player1Y = OldPlayerY;
						}
						DrawGraph((x * BlockSize) - WorldposX, (y * BlockSize) - WorldposY, Yuka, TRUE);

					}

					if (Map1[y][x] == KUMO)//２の場所に壊せるブロックを描画
					{
						if (abs(Player1X - (x * BreakSize - WorldposX)) < 48 && abs(Player1Y - (y * BreakSize)) < 48 && BlockFlag == 0)
						{
							RendaFlag = 1;
						}
						DrawGraph((x * BreakSize) - WorldposX, (y * BreakSize) - WorldposY, BreakBlock, TRUE);
					}

					if (Map1[y][x] == NEEDLE)//３の場所に障害物を描画
					{
						DrawGraph((x * EnemySize) - WorldposX, (y * EnemySize) - WorldposY, Enemy, TRUE);
						if (abs(Player1X - (x * EnemySize - WorldposX)) < 32 + 16 && abs(Player1Y - (y * EnemySize)) < 32 + 16)
						{
							SceneNo = 10;
						}
					}

					if (Map1[y][x] == PAIPU)//4の場所に地面（パイプ）を描画
					{
						DrawGraph((x * BlockSize) - WorldposX, (y * BlockSize) - WorldposY, Paipu, TRUE);
					}

					if (Map1[y][x] == KANBAN1)//5の場所にジャンプの看板を描画
					{
						DrawGraph((x * KanbanSize) - WorldposX, (y * KanbanSize) - WorldposY, Kanban1, TRUE);
					}

					if (Map1[y][x] == KANBAN2)//6の場所にスライディングの看板を描画
					{
						DrawGraph((x * KanbanSize) - WorldposY, (y * KanbanSize) - WorldposY, Kanban2, TRUE);
					}

					if (Map1[y][x] == KANBAN3)//7の場所に壊せるブロックの看板を描画
					{
						DrawGraph((x * KanbanSize) - WorldposX, (y * KanbanSize) - WorldposY, Kanban3, TRUE);
					}
					if (Map1[y][x] == KEY)//11の鍵の描画
					{
						DrawGraph((x * BlockSize) - WorldposX, (y * BlockSize) - WorldposY, ClearKey, TRUE);
						if (abs(Player1X - (x * BlockSize - WorldposX)) < 32 && abs(Player1Y - (y * BlockSize)) < 32)
						{
							SceneNo = 2;
						}
					}
				}
			}
			if (PLCount >= 3) //プレイヤーの連番処理
			{
				PL++;
				PLCount = 0;
			}
			else
			{
				PLCount++;
			}
			if (PL >= 5)
			{
				PL = 0;
			}
			DrawGraph(Player1X, Player1Y, Player, TRUE); //プレイヤーの描画
			if (RendaFlag == 1)
			{
				DrawGraph(290, 50, Renda, TRUE);
			}
			break;
		case 10:
			DrawGraph(0, 0, GameOver, TRUE);
			break;
		case 2:

			//更新処理
			if (keys[KEY_INPUT_RIGHT] == 1 && oldkeys[KEY_INPUT_RIGHT] == 0)
			{
				if (PlayerIdX + 1 < mapWidth1)
				{
					if (Map1[PlayerIdY][PlayerIdX + 1] == KUUHAKU)
					{
						if (Player1X < WIN_WIDTH / 2)
						{
							Player1X += BlockSize;
						}

						else if (Player1X + WorldposX >= mapWidth1 * BlockSize - 400)
						{
							Player1X += BlockSize;
						}

						else
						{
							WorldposX += BlockSize;
						}
					}
				}
			}

			if (keys[KEY_INPUT_LEFT] == 1 && oldkeys[KEY_INPUT_LEFT] == 0)
			{

				if (PlayerIdX - 1 >= 0)
				{
					if (Map1[PlayerIdY][PlayerIdX - 1] == KUUHAKU)
					{
						if (WorldposX <= 0)
						{
							Player1X -= BlockSize;
						}

						else if (Player1X + WorldposX >= mapWidth1 * BlockSize - 400)
						{
							Player1X -= BlockSize;
						}

						else
						{
							WorldposX -= BlockSize;
						}
					}
				}
			}

			if (keys[KEY_INPUT_DOWN] == 1 && oldkeys[KEY_INPUT_DOWN] == 0)
			{
				if (PlayerIdY + 1 < mapHeight1)
				{
					if (Map1[PlayerIdY + 1][PlayerIdX] == KUUHAKU)
					{
						if (Player1Y < 6 * BlockSize)
						{
							Player1Y += BlockSize;
						}

						else if (Player1Y + WorldposY >= mapHeight1 * BlockSize - BlockSize * 6)
						{
							Player1Y += BlockSize;
						}

						else
						{
							WorldposY += BlockSize;
						}
					}
				}
			}

			if (keys[KEY_INPUT_UP] == 1 && oldkeys[KEY_INPUT_UP] == 0)
			{
				if (PlayerIdY - 1 >= 0)
				{
					if (Map1[PlayerIdY - 1][PlayerIdX] == KUUHAKU)
					{
						if (WorldposY <= 0)
						{
							Player1Y -= BlockSize;
						}

						else if (Player1Y + WorldposY >= mapHeight1 * BlockSize - BlockSize * 5)
						{
							Player1Y -= BlockSize;
						}

						else
						{
							WorldposY -= BlockSize;
						}
					}
				}
			}

			//描画処理
			for (int y = 0; y < mapHeight2; y++)//マップの縦軸まで繰り返す
			{
				for (int x = 0; x < mapWidth2; x++)//マップの横軸まで繰り返す
				{
					if (Map2[y][x] == YUKA)//1の場所に地面を描画
					{
						DrawGraph((x * BlockSize) - WorldposX, (y * BlockSize) - WorldposY, Yuka, TRUE);
					}

					if (Map2[y][x] == KUMO)//２の場所に壊せるブロックを描画
					{
						DrawGraph((x * BreakSize) - WorldposX, (y * BreakSize) - WorldposY, BreakBlock, TRUE);
					}

					if (Map2[y][x] == NEEDLE)//３の場所に障害物を描画
					{
						DrawGraph((x * EnemySize) - WorldposX, (y * EnemySize) - WorldposY, Enemy, TRUE);
					}

					if (Map2[y][x] == PAIPU)//4の場所に地面（パイプ）を描画
					{
						DrawGraph((x * BlockSize) - WorldposX, (y * BlockSize) - WorldposY, Paipu, TRUE);
					}

					if (Map2[y][x] == KANBAN1)//5の場所にジャンプの看板を描画
					{
						DrawGraph((x * KanbanSize) - WorldposX, (y * KanbanSize) - WorldposY, Kanban1, TRUE);
					}

					if (Map2[y][x] == KANBAN2)//6の場所にスライディングの看板を描画
					{
						DrawGraph((x * KanbanSize) - WorldposY, (y * KanbanSize) - WorldposY, Kanban2, TRUE);
					}

					if (Map2[y][x] == KANBAN3)//7の場所に壊せるブロックの看板を描画
					{
						DrawGraph((x * KanbanSize) - WorldposX, (y * KanbanSize) - WorldposY, Kanban3, TRUE);
					}

					if (Map2[y][x] == KUSA1)//8の場所に草を描画
					{
						DrawGraph((x * BlockSize) - WorldposX, (y * BlockSize) - WorldposY, Kusa1, TRUE);
					}

					if (Map2[y][x] == KUSA2)//9の場所に草を描画
					{
						DrawGraph((x * BlockSize) - WorldposX, (y * BlockSize) - WorldposY, Kusa2, TRUE);
					}

					if (Map2[y][x] == WATER)//10の場所に水？ブロックを描画
					{
						DrawGraph((x * BlockSize) - WorldposX, (y * BlockSize) - WorldposY, Water, TRUE);
					}
				}
			}
			break;
		case 3:
			//更新処理
			if (keys[KEY_INPUT_RIGHT] == 1 && oldkeys[KEY_INPUT_RIGHT] == 0)
			{
				if (PlayerIdX + 1 < mapWidth1)
				{
					if (Map1[PlayerIdY][PlayerIdX + 1] == KUUHAKU)
					{
						if (Player1X < WIN_WIDTH / 2)
						{
							Player1X += BlockSize;
						}

						else if (Player1X + WorldposX >= mapWidth1 * BlockSize - 400)
						{
							Player1X += BlockSize;
						}

						else
						{
							WorldposX += BlockSize;
						}
					}
				}
			}

			if (keys[KEY_INPUT_LEFT] == 1 && oldkeys[KEY_INPUT_LEFT] == 0)
			{

				if (PlayerIdX - 1 >= 0)
				{
					if (Map1[PlayerIdY][PlayerIdX - 1] == KUUHAKU)
					{
						if (WorldposX <= 0)
						{
							Player1X -= BlockSize;
						}

						else if (Player1X + WorldposX >= mapWidth1 * BlockSize - 400)
						{
							Player1X -= BlockSize;
						}

						else
						{
							WorldposX -= BlockSize;
						}
					}
				}
			}

			if (keys[KEY_INPUT_DOWN] == 1 && oldkeys[KEY_INPUT_DOWN] == 0)
			{
				if (PlayerIdY + 1 < mapHeight1)
				{
					if (Map1[PlayerIdY + 1][PlayerIdX] == KUUHAKU)
					{
						if (Player1Y < 6 * BlockSize)
						{
							Player1Y += BlockSize;
						}

						else if (Player1Y + WorldposY >= mapHeight1 * BlockSize - BlockSize * 6)
						{
							Player1Y += BlockSize;
						}

						else
						{
							WorldposY += BlockSize;
						}
					}
				}
			}

			if (keys[KEY_INPUT_UP] == 1 && oldkeys[KEY_INPUT_UP] == 0)
			{
				if (PlayerIdY - 1 >= 0)
				{
					if (Map1[PlayerIdY - 1][PlayerIdX] == KUUHAKU)
					{
						if (WorldposY <= 0)
						{
							Player1Y -= BlockSize;
						}

						else if (Player1Y + WorldposY >= mapHeight1 * BlockSize - BlockSize * 5)
						{
							Player1Y -= BlockSize;
						}

						else
						{
							WorldposY -= BlockSize;
						}
					}
				}
			}

			//描画処理
			for (int y = 0; y < mapHeight3; y++)//マップの縦軸まで繰り返す
			{
				for (int x = 0; x < mapWidth3; x++)//マップの横軸まで繰り返す
				{
					if (Map3[y][x] == YUKA)//1の場所に地面を描画
					{
						DrawGraph((x * BlockSize) - WorldposX, (y * BlockSize) - WorldposY, Yuka, TRUE);
					}

					if (Map3[y][x] == KUMO)//２の場所に壊せるブロックを描画
					{
						DrawGraph((x * BreakSize) - WorldposX, (y * BreakSize) - WorldposY, BreakBlock, TRUE);
					}

					if (Map3[y][x] == NEEDLE)//３の場所に障害物を描画
					{
						DrawGraph((x * EnemySize) - WorldposX, (y * EnemySize) - WorldposY, Enemy, TRUE);
					}

					if (Map3[y][x] == PAIPU)//4の場所に地面（パイプ）を描画
					{
						DrawGraph((x * BlockSize) - WorldposX, (y * BlockSize) - WorldposY, Paipu, TRUE);
					}

					if (Map3[y][x] == KANBAN1)//5の場所にジャンプの看板を描画
					{
						DrawGraph((x * KanbanSize) - WorldposX, (y * KanbanSize) - WorldposY, Kanban1, TRUE);
					}

					if (Map3[y][x] == KANBAN2)//6の場所にスライディングの看板を描画
					{
						DrawGraph((x * KanbanSize) - WorldposY, (y * KanbanSize) - WorldposY, Kanban2, TRUE);
					}

					if (Map3[y][x] == KANBAN3)//7の場所に壊せるブロックの看板を描画
					{
						DrawGraph((x * KanbanSize) - WorldposX, (y * KanbanSize) - WorldposY, Kanban3, TRUE);
					}

					if (Map3[y][x] == KUSA1)//8の場所に草を描画
					{
						DrawGraph((x * BlockSize) - WorldposX, (y * BlockSize) - WorldposY, Kusa1, TRUE);
					}

					if (Map3[y][x] == KUSA2)//9の場所に草を描画
					{
						DrawGraph((x * BlockSize) - WorldposX, (y * BlockSize) - WorldposY, Kusa2, TRUE);
					}

					if (Map3[y][x] == WATER)//10の場所に水？ブロックを描画
					{
						DrawGraph((x * BlockSize) - WorldposX, (y * BlockSize) - WorldposY, Water, TRUE);
					}
				}
			}
			break;
		}








		//---------  ここまでにプログラムを記述  ---------//
		ScreenFlip();//（ダブルバッファ）裏面
		// 20ミリ秒待機（疑似60FPS）
		WaitTimer(20);
		// Windows システムからくる情報を処理する
		if (ProcessMessage() == -1)
		{
			break;
		}
		// ＥＳＣキーが押されたらループから抜ける
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1)
		{
			break;
		}
	}
	//Dxライブラリ終了処理
	DxLib_End();

	return 0;
}